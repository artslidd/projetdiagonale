#include <QtMath>
#include <QMessageBox>
#include <QFile>
#include <QTime>
#include "constants.h"
#include "samplermodule.h"
#include "sndfile.h"
#include "ui_samplermodule.h"
#include "stdlib.h"
#include "../midi/midiengine.h"
#include <QFileDialog>

#include <QDebug>
using namespace std;

int inoutSampler(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void *streamData)
{
    Q_UNUSED(streamTime);
    Q_UNUSED(status);
    Q_UNUSED(inputBuffer);

    SamplerModule::StreamData & data = *((SamplerModule::StreamData*)streamData);

    float * beforeFilterBuffer = NULL;
    if (beforeFilterBuffer  == NULL) beforeFilterBuffer = new float[nBufferFrames]; // ...Memory leak here

    static int column = 0;
    static quint64 time = 0;
    static int loopNumber = data.loopCount;

    if (!data.timeUpdated){
        column = data.sliderValue[0];
        time = data.sliderValue[1];
        ((SamplerModule*)data.samplerModule)->timeUpdateDone();
    }

    QVector<int> indexes;
    int state1 = data.buttonsState[0][column];
    int state2 = data.buttonsState[1][column];
    int state3 = data.buttonsState[2][column];
    int chor = data.buttonsState[3][column];

    if(chor != 0){
        if(state1 != 0){
            indexes.push_back(3*(chor-1)+state1-1);
        }
        if(state2 != 0){
            indexes.push_back(15 + 3*(chor-1)+state2-1);
        }
        if(state3 != 0){
            indexes.push_back(30 + 3*(chor-1)+state3-1);
        }
    }

    if (indexes.isEmpty()) {
        for (unsigned int i = 0 ; i < nBufferFrames ; ++i)
            beforeFilterBuffer[i] = 0.0f;
    } else {
        memcpy(beforeFilterBuffer, data.samples[indexes[0]] + time, nBufferFrames * sizeof(float));
        for (int i = 1 ; i < indexes.size() ; ++i) {
            for (unsigned int j = 0 ; j < nBufferFrames ; ++j)
                beforeFilterBuffer[j] += data.samples[indexes[i]][time + j];
        }
    }
    ((SamplerModule*)data.samplerModule)->moveLine(time,column);
    time += nBufferFrames;
    if (time > 44100 * 4 - nBufferFrames){
        time = 0;
        column = (column+1)%16;
        if(loopNumber!=0){
            if (column==max(data.loopStates[0],data.loopStates[1])-1){
                column=min(data.loopStates[0],data.loopStates[1])-1;
                loopNumber--;
            }
        }else{
            if (column==max(data.loopStates[0],data.loopStates[1])%16){
                loopNumber=data.loopCount;
            }
        }

    }

    if(data.pianoTone)
        EngineModule::addPianoTone(beforeFilterBuffer, nBufferFrames, data.pianoFreq);

    data.filter->applyOnBuffer((float *) beforeFilterBuffer, (float *) outputBuffer, nBufferFrames);
    data.ringBuffer->addBuffer((float *) outputBuffer, nBufferFrames);

    memcpy((void*) &((float *) outputBuffer)[nBufferFrames], outputBuffer, nBufferFrames * sizeof(float));
    return 0;
}

SamplerModule::SamplerModule(QWidget *parent) : EngineModule(parent), ui(new Ui::SamplerModule), m_style(0),  m_line_pos(64), last_clicked(0)
{
    ui->setupUi(this);

    // Stream data
    m_streamData.ringBuffer = NULL;
    m_streamData.filter = NULL;
    m_streamData.samples = NULL;
    m_streamData.pianoTone = false;
    m_streamData.pianoFreq = 0.0f;
    m_streamData.samplerModule = this;
    m_streamData.loopStates[0] = 1;
    m_streamData.loopStates[1] = 17;

    for(int i=0; i<4; i++){
        for(int j=0; j<16; j++){
            m_streamData.buttonsState[i][j] = 0;
        }
    }

    m_streamData.loopCount = 2;

    //m_styles et m_samples

    QVector< QVector< QVector< QVector<QString> > > > samples;
    m_samples = samples;
    QVector<struct Style> styles;
    m_styles = styles;

    int nb_styles = 0;
    int nbmax_samples = 0;
    int nbmax_instruments = 0;
    int nbmax_chords = 0;



    //Le fichier où sont contenues les infos
    ifstream fichier("samples.txt", ios::binary);

    QMessageBox msgBox;

    if(fichier)
    {
        //Definition des styles
        msgBox.setText("Le fichier 'samples.txt' est mal rédigé :\n");
        string ligne;
        fichier >> ligne;

        if(ligne != "Nom")
        {
            msgBox.setText(msgBox.text() + "'Nom' introuvable\n");
        }
        while(ligne != "Samples")
        {
            //On trouve le nom du style
            struct Style style;
            nb_styles++;
            fichier >> ligne >> ligne;
            style.name = QString::fromStdString(ligne);

            // Ajout du style au dropdown menu
            ui->styleSelector->addItem(style.name);

            fichier >> ligne;
            //On trouve les accords du style
            if(ligne == "Accords")
            {
                fichier >> ligne >> ligne;
                while (ligne != "Instruments")
                {
                    style.nb_chords++;
                    if(style.nb_chords>10)
                    {
                        msgBox.setText(msgBox.text() + "'Instruments' introuvable\n");
                    }
                    style.chords.push_back(QString::fromStdString(ligne));
                    fichier >> ligne;
                }
                fichier >> ligne >> ligne;
                while (ligne != "Nom" && ligne != "Samples")
                {
                    style.nb_instruments++;
                    if(style.nb_instruments>10)
                    {
                        msgBox.setText(msgBox.text() + "'Nom' ou 'Samples' introuvable\n");
                    }
                    style.instruments.push_back(QString::fromStdString(ligne));
                    fichier >> ligne;
                }
            }else
            {
                msgBox.setText(msgBox.text() + "'Accords' introuvable\n");
            }
            if (nbmax_instruments<style.nb_instruments)
            {
                nbmax_instruments=style.nb_instruments;
            };
            if (nbmax_chords<style.nb_chords)
            {
                nbmax_chords=style.nb_chords;
            };
            m_styles.push_back(style);
        }
        fichier >> ligne;

        //Importation des adresses des samples : on les suppose rangés

        //Calcul de la taille du tableau de Samples

        int pos = fichier.tellg();
        fichier >> ligne;
        while (ligne!="Fin")
        {
            string chara(1,ligne[ligne.size()-1]);
            istringstream convert(chara);
            int chiffre;
            convert >> chiffre;
            if (chiffre>nbmax_samples){
                nbmax_samples = chiffre;
            }
            fichier >> ligne >> ligne >> ligne;
        }
        fichier.seekg(pos);

        //Adressage des samples

        string samples[nb_styles][nbmax_instruments][nbmax_chords][nbmax_samples];

        string sampleadress;
        bool errCritChor = false;
        bool errCritInst = false;
        bool errCritStyl = false;
        for (int i=0;i<nb_styles;i++)
        {
            bool existStyl = false;
            m_samples.push_back(QVector< QVector< QVector<QString> > >());
            for (int j=0;j<m_styles[i].nb_instruments;j++)
            {
                bool existInst = false;
                m_samples[i].push_back(QVector< QVector<QString> >());
                m_styles[i].nb_samples.push_back(QVector<int>());
                for (int k=0;k<m_styles[i].nb_chords;k++)
                {
                    bool existChor = false;
                    m_samples[i][j].push_back(QVector <QString>());
                    m_styles[i].nb_samples[j].push_back(0);
                    QString textTemp = "";
                    for (int p=0;p<nbmax_samples;p++)
                    {
                        ostringstream convert;
                        convert << p+1;
                        string n = convert.str();
                        sampleadress = m_styles[i].name.toStdString() + "-" + m_styles[i].instruments[j].toStdString() + "-" + m_styles[i].chords[k].toStdString() + "-" + n;
                        fichier >> ligne;
                        if (sampleadress == ligne){
                            fichier >> ligne >> ligne;
                            m_samples[i][j][k].push_back(QString::fromStdString(ligne));
                            pos = fichier.tellg();
                            m_styles[i].nb_samples[j][k]++;
                            m_styles[i].total_samples++;
                            if(textTemp!=""){
                                msgBox.setText(msgBox.text() + textTemp);
                                textTemp = "";
                            }
                            existChor = true;
                            existInst = true;
                            existStyl = true;
                        }
                        else{
                            fichier.seekg(pos);
                            textTemp = textTemp + QString::fromStdString(sampleadress) + " est manquant\n";
                        }
                    }if(existChor == false){errCritChor=true;}
                }if(existInst == false){errCritInst=true;}
            }if(existStyl == false){errCritStyl=true;}
        }
        if(errCritChor){
            msgBox.setText(msgBox.text() + "Les samples d'un accord sont manquants. Cette erreur peut être critique.\n");
        }
        if(errCritInst){
            msgBox.setText(msgBox.text() + "Les samples d'un instrument sont manquants. Cette erreur peut être critique.\n");
        }
        if(errCritStyl){
            msgBox.setText(msgBox.text() + "Les samples d'un style sont manquants. Cette erreur peut être critique.\n");
        }
        if(msgBox.text()!="Le fichier 'samples.txt' est mal rédigé :\n"){
            msgBox.setText(msgBox.text() + "Veuillez revoir le fichier.");
            msgBox.exec();
        }
        fichier.close();
    }else
    {
        msgBox.setText("Erreur : Impossible de trouver le fichier 'samples.txt'. Vérifiez qu'il existe un fichier 'samples.txt' dans le répertoire de travail.");
        msgBox.exec();
    }

    // Widgets
    ui->loopCountBox->addItem("0");
    ui->loopCountBox->addItem("1");
    ui->loopCountBox->addItem("2");
    ui->loopCountBox->addItem("3");
    ui->loopCountBox->addItem("4");
    ui->loopCountBox->setCurrentIndex(2);

    connect(ui->loopCountBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeLoop(int)));
    connect(ui->resetButton, SIGNAL(clicked()), this, SLOT(reset()));

    // Style selector

    connect(ui->styleSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(setStyle(int)));
    connect(this, SIGNAL(instrumentChanged(QString)), ui->label_inst_1, SLOT(setText(QString)));
    connect(this, SIGNAL(instrumentChanged_2(QString)), ui->label_inst_2, SLOT(setText(QString)));
    connect(this, SIGNAL(instrumentChanged_3(QString)), ui->label_inst_3, SLOT(setText(QString)));

    //Connect pour changer d'accord
    // Accord 01 - Instruments 1-2-3
    connect(ui->btn_accords_01, SIGNAL(clicked()), this, SLOT(changeChord_01()));
    connect(this, SIGNAL(updateChord_01()), ui->btn_accords_01, SLOT(updateChor()));
    connect(ui->btn_accords_01, SIGNAL(chorChanged()), ui->btn_inst1_01, SLOT(setChord()));
    connect(ui->btn_accords_01, SIGNAL(chorChanged()), ui->btn_inst2_01, SLOT(setChord()));
    connect(ui->btn_accords_01, SIGNAL(chorChanged()), ui->btn_inst3_01, SLOT(setChord()));

    // Accord 02 - Instruments 1-2-3
    connect(ui->btn_accords_02, SIGNAL(clicked()), this, SLOT(changeChord_02()));
    connect(this, SIGNAL(updateChord_02()), ui->btn_accords_02, SLOT(updateChor()));
    connect(ui->btn_accords_02, SIGNAL(chorChanged()), ui->btn_inst1_02, SLOT(setChord()));
    connect(ui->btn_accords_02, SIGNAL(chorChanged()), ui->btn_inst2_02, SLOT(setChord()));
    connect(ui->btn_accords_02, SIGNAL(chorChanged()), ui->btn_inst3_02, SLOT(setChord()));

    // Accord 03 - Instruments 1-2-3
    connect(ui->btn_accords_03, SIGNAL(clicked()), this, SLOT(changeChord_03()));
    connect(this, SIGNAL(updateChord_03()), ui->btn_accords_03, SLOT(updateChor()));
    connect(ui->btn_accords_03, SIGNAL(chorChanged()), ui->btn_inst1_03, SLOT(setChord()));
    connect(ui->btn_accords_03, SIGNAL(chorChanged()), ui->btn_inst2_03, SLOT(setChord()));
    connect(ui->btn_accords_03, SIGNAL(chorChanged()), ui->btn_inst3_03, SLOT(setChord()));

    // Accord 04 - Instruments 1-2-3
    connect(ui->btn_accords_04, SIGNAL(clicked()), this, SLOT(changeChord_04()));
    connect(this, SIGNAL(updateChord_04()), ui->btn_accords_04, SLOT(updateChor()));
    connect(ui->btn_accords_04, SIGNAL(chorChanged()), ui->btn_inst1_04, SLOT(setChord()));
    connect(ui->btn_accords_04, SIGNAL(chorChanged()), ui->btn_inst2_04, SLOT(setChord()));
    connect(ui->btn_accords_04, SIGNAL(chorChanged()), ui->btn_inst3_04, SLOT(setChord()));

    // Accord 05 - Instruments 1-2-3
    connect(ui->btn_accords_05, SIGNAL(clicked()), this, SLOT(changeChord_05()));
    connect(this, SIGNAL(updateChord_05()), ui->btn_accords_05, SLOT(updateChor()));
    connect(ui->btn_accords_05, SIGNAL(chorChanged()), ui->btn_inst1_05, SLOT(setChord()));
    connect(ui->btn_accords_05, SIGNAL(chorChanged()), ui->btn_inst2_05, SLOT(setChord()));
    connect(ui->btn_accords_05, SIGNAL(chorChanged()), ui->btn_inst3_05, SLOT(setChord()));

    // Accord 06 - Instruments 1-2-3
    connect(ui->btn_accords_06, SIGNAL(clicked()), this, SLOT(changeChord_06()));
    connect(this, SIGNAL(updateChord_06()), ui->btn_accords_06, SLOT(updateChor()));
    connect(ui->btn_accords_06, SIGNAL(chorChanged()), ui->btn_inst1_06, SLOT(setChord()));
    connect(ui->btn_accords_06, SIGNAL(chorChanged()), ui->btn_inst2_06, SLOT(setChord()));
    connect(ui->btn_accords_06, SIGNAL(chorChanged()), ui->btn_inst3_06, SLOT(setChord()));

    // Accord 07 - Instruments 1-2-3
    connect(ui->btn_accords_07, SIGNAL(clicked()), this, SLOT(changeChord_07()));
    connect(this, SIGNAL(updateChord_07()), ui->btn_accords_07, SLOT(updateChor()));
    connect(ui->btn_accords_07, SIGNAL(chorChanged()), ui->btn_inst1_07, SLOT(setChord()));
    connect(ui->btn_accords_07, SIGNAL(chorChanged()), ui->btn_inst2_07, SLOT(setChord()));
    connect(ui->btn_accords_07, SIGNAL(chorChanged()), ui->btn_inst3_07, SLOT(setChord()));

    // Accord 08 - Instruments 1-2-3
    connect(ui->btn_accords_08, SIGNAL(clicked()), this, SLOT(changeChord_08()));
    connect(this, SIGNAL(updateChord_08()), ui->btn_accords_08, SLOT(updateChor()));
    connect(ui->btn_accords_08, SIGNAL(chorChanged()), ui->btn_inst1_08, SLOT(setChord()));
    connect(ui->btn_accords_08, SIGNAL(chorChanged()), ui->btn_inst2_08, SLOT(setChord()));
    connect(ui->btn_accords_08, SIGNAL(chorChanged()), ui->btn_inst3_08, SLOT(setChord()));

    // Accord 09 - Instruments 1-2-3
    connect(ui->btn_accords_09, SIGNAL(clicked()), this, SLOT(changeChord_09()));
    connect(this, SIGNAL(updateChord_09()), ui->btn_accords_09, SLOT(updateChor()));
    connect(ui->btn_accords_09, SIGNAL(chorChanged()), ui->btn_inst1_09, SLOT(setChord()));
    connect(ui->btn_accords_09, SIGNAL(chorChanged()), ui->btn_inst2_09, SLOT(setChord()));
    connect(ui->btn_accords_09, SIGNAL(chorChanged()), ui->btn_inst3_09, SLOT(setChord()));

    // Accord 10 - Instruments 1-2-3
    connect(ui->btn_accords_10, SIGNAL(clicked()), this, SLOT(changeChord_10()));
    connect(this, SIGNAL(updateChord_10()), ui->btn_accords_10, SLOT(updateChor()));
    connect(ui->btn_accords_10, SIGNAL(chorChanged()), ui->btn_inst1_10, SLOT(setChord()));
    connect(ui->btn_accords_10, SIGNAL(chorChanged()), ui->btn_inst2_10, SLOT(setChord()));
    connect(ui->btn_accords_10, SIGNAL(chorChanged()), ui->btn_inst3_10, SLOT(setChord()));

    // Accord 11 - Instruments 1-2-3
    connect(ui->btn_accords_11, SIGNAL(clicked()), this, SLOT(changeChord_11()));
    connect(this, SIGNAL(updateChord_11()), ui->btn_accords_11, SLOT(updateChor()));
    connect(ui->btn_accords_11, SIGNAL(chorChanged()), ui->btn_inst1_11, SLOT(setChord()));
    connect(ui->btn_accords_11, SIGNAL(chorChanged()), ui->btn_inst2_11, SLOT(setChord()));
    connect(ui->btn_accords_11, SIGNAL(chorChanged()), ui->btn_inst3_11, SLOT(setChord()));

    // Accord 12 - Instruments 1-2-3
    connect(ui->btn_accords_12, SIGNAL(clicked()), this, SLOT(changeChord_12()));
    connect(this, SIGNAL(updateChord_12()), ui->btn_accords_12, SLOT(updateChor()));
    connect(ui->btn_accords_12, SIGNAL(chorChanged()), ui->btn_inst1_12, SLOT(setChord()));
    connect(ui->btn_accords_12, SIGNAL(chorChanged()), ui->btn_inst2_12, SLOT(setChord()));
    connect(ui->btn_accords_12, SIGNAL(chorChanged()), ui->btn_inst3_12, SLOT(setChord()));

    // Accord 13 - Instruments 1-2-3
    connect(ui->btn_accords_13, SIGNAL(clicked()), this, SLOT(changeChord_13()));
    connect(this, SIGNAL(updateChord_13()), ui->btn_accords_13, SLOT(updateChor()));
    connect(ui->btn_accords_13, SIGNAL(chorChanged()), ui->btn_inst1_13, SLOT(setChord()));
    connect(ui->btn_accords_13, SIGNAL(chorChanged()), ui->btn_inst2_13, SLOT(setChord()));
    connect(ui->btn_accords_13, SIGNAL(chorChanged()), ui->btn_inst3_13, SLOT(setChord()));

    // Accord 14 - Instruments 1-2-3
    connect(ui->btn_accords_14, SIGNAL(clicked()), this, SLOT(changeChord_14()));
    connect(this, SIGNAL(updateChord_14()), ui->btn_accords_14, SLOT(updateChor()));
    connect(ui->btn_accords_14, SIGNAL(chorChanged()), ui->btn_inst1_14, SLOT(setChord()));
    connect(ui->btn_accords_14, SIGNAL(chorChanged()), ui->btn_inst2_14, SLOT(setChord()));
    connect(ui->btn_accords_14, SIGNAL(chorChanged()), ui->btn_inst3_14, SLOT(setChord()));

    // Accord 15 - Instruments 1-2-3
    connect(ui->btn_accords_15, SIGNAL(clicked()), this, SLOT(changeChord_15()));
    connect(this, SIGNAL(updateChord_15()), ui->btn_accords_15, SLOT(updateChor()));
    connect(ui->btn_accords_15, SIGNAL(chorChanged()), ui->btn_inst1_15, SLOT(setChord()));
    connect(ui->btn_accords_15, SIGNAL(chorChanged()), ui->btn_inst2_15, SLOT(setChord()));
    connect(ui->btn_accords_15, SIGNAL(chorChanged()), ui->btn_inst3_15, SLOT(setChord()));

    // Accord 16 - Instruments 1-2-3
    connect(ui->btn_accords_16, SIGNAL(clicked()), this, SLOT(changeChord_16()));
    connect(this, SIGNAL(updateChord_16()), ui->btn_accords_16, SLOT(updateChor()));
    connect(ui->btn_accords_16, SIGNAL(chorChanged()), ui->btn_inst1_16, SLOT(setChord()));
    connect(ui->btn_accords_16, SIGNAL(chorChanged()), ui->btn_inst2_16, SLOT(setChord()));
    connect(ui->btn_accords_16, SIGNAL(chorChanged()), ui->btn_inst3_16, SLOT(setChord()));

    // Connect pour changer de sample d'instrument
    // Instrument 1
    connect(ui->btn_inst1_01, SIGNAL(clicked()), ui->btn_accords_01, SLOT(updateChor()));
    connect(ui->btn_inst1_02, SIGNAL(clicked()), ui->btn_accords_02, SLOT(updateChor()));
    connect(ui->btn_inst1_03, SIGNAL(clicked()), ui->btn_accords_03, SLOT(updateChor()));
    connect(ui->btn_inst1_04, SIGNAL(clicked()), ui->btn_accords_04, SLOT(updateChor()));
    connect(ui->btn_inst1_05, SIGNAL(clicked()), ui->btn_accords_05, SLOT(updateChor()));
    connect(ui->btn_inst1_06, SIGNAL(clicked()), ui->btn_accords_06, SLOT(updateChor()));
    connect(ui->btn_inst1_07, SIGNAL(clicked()), ui->btn_accords_07, SLOT(updateChor()));
    connect(ui->btn_inst1_08, SIGNAL(clicked()), ui->btn_accords_08, SLOT(updateChor()));
    connect(ui->btn_inst1_09, SIGNAL(clicked()), ui->btn_accords_09, SLOT(updateChor()));
    connect(ui->btn_inst1_10, SIGNAL(clicked()), ui->btn_accords_10, SLOT(updateChor()));
    connect(ui->btn_inst1_11, SIGNAL(clicked()), ui->btn_accords_11, SLOT(updateChor()));
    connect(ui->btn_inst1_12, SIGNAL(clicked()), ui->btn_accords_12, SLOT(updateChor()));
    connect(ui->btn_inst1_13, SIGNAL(clicked()), ui->btn_accords_13, SLOT(updateChor()));
    connect(ui->btn_inst1_14, SIGNAL(clicked()), ui->btn_accords_14, SLOT(updateChor()));
    connect(ui->btn_inst1_15, SIGNAL(clicked()), ui->btn_accords_15, SLOT(updateChor()));
    connect(ui->btn_inst1_16, SIGNAL(clicked()), ui->btn_accords_16, SLOT(updateChor()));

    // Instrument 2
    connect(ui->btn_inst2_01, SIGNAL(clicked()), ui->btn_accords_01, SLOT(updateChor()));
    connect(ui->btn_inst2_02, SIGNAL(clicked()), ui->btn_accords_02, SLOT(updateChor()));
    connect(ui->btn_inst2_03, SIGNAL(clicked()), ui->btn_accords_03, SLOT(updateChor()));
    connect(ui->btn_inst2_04, SIGNAL(clicked()), ui->btn_accords_04, SLOT(updateChor()));
    connect(ui->btn_inst2_05, SIGNAL(clicked()), ui->btn_accords_05, SLOT(updateChor()));
    connect(ui->btn_inst2_06, SIGNAL(clicked()), ui->btn_accords_06, SLOT(updateChor()));
    connect(ui->btn_inst2_07, SIGNAL(clicked()), ui->btn_accords_07, SLOT(updateChor()));
    connect(ui->btn_inst2_08, SIGNAL(clicked()), ui->btn_accords_08, SLOT(updateChor()));
    connect(ui->btn_inst2_09, SIGNAL(clicked()), ui->btn_accords_09, SLOT(updateChor()));
    connect(ui->btn_inst2_10, SIGNAL(clicked()), ui->btn_accords_10, SLOT(updateChor()));
    connect(ui->btn_inst2_11, SIGNAL(clicked()), ui->btn_accords_11, SLOT(updateChor()));
    connect(ui->btn_inst2_12, SIGNAL(clicked()), ui->btn_accords_12, SLOT(updateChor()));
    connect(ui->btn_inst2_13, SIGNAL(clicked()), ui->btn_accords_13, SLOT(updateChor()));
    connect(ui->btn_inst2_14, SIGNAL(clicked()), ui->btn_accords_14, SLOT(updateChor()));
    connect(ui->btn_inst2_15, SIGNAL(clicked()), ui->btn_accords_15, SLOT(updateChor()));
    connect(ui->btn_inst2_16, SIGNAL(clicked()), ui->btn_accords_16, SLOT(updateChor()));

    // Instrument 3
    connect(ui->btn_inst3_01, SIGNAL(clicked()), ui->btn_accords_01, SLOT(updateChor()));
    connect(ui->btn_inst3_02, SIGNAL(clicked()), ui->btn_accords_02, SLOT(updateChor()));
    connect(ui->btn_inst3_03, SIGNAL(clicked()), ui->btn_accords_03, SLOT(updateChor()));
    connect(ui->btn_inst3_04, SIGNAL(clicked()), ui->btn_accords_04, SLOT(updateChor()));
    connect(ui->btn_inst3_05, SIGNAL(clicked()), ui->btn_accords_05, SLOT(updateChor()));
    connect(ui->btn_inst3_06, SIGNAL(clicked()), ui->btn_accords_06, SLOT(updateChor()));
    connect(ui->btn_inst3_07, SIGNAL(clicked()), ui->btn_accords_07, SLOT(updateChor()));
    connect(ui->btn_inst3_08, SIGNAL(clicked()), ui->btn_accords_08, SLOT(updateChor()));
    connect(ui->btn_inst3_09, SIGNAL(clicked()), ui->btn_accords_09, SLOT(updateChor()));
    connect(ui->btn_inst3_10, SIGNAL(clicked()), ui->btn_accords_10, SLOT(updateChor()));
    connect(ui->btn_inst3_11, SIGNAL(clicked()), ui->btn_accords_11, SLOT(updateChor()));
    connect(ui->btn_inst3_12, SIGNAL(clicked()), ui->btn_accords_12, SLOT(updateChor()));
    connect(ui->btn_inst3_13, SIGNAL(clicked()), ui->btn_accords_13, SLOT(updateChor()));
    connect(ui->btn_inst3_14, SIGNAL(clicked()), ui->btn_accords_14, SLOT(updateChor()));
    connect(ui->btn_inst3_15, SIGNAL(clicked()), ui->btn_accords_15, SLOT(updateChor()));
    connect(ui->btn_inst3_16, SIGNAL(clicked()), ui->btn_accords_16, SLOT(updateChor()));

    //Lecture et Pause
    connect(ui->pausePlayButton, SIGNAL(clicked()), this, SLOT(play()));
    connect(ui->randomButton, SIGNAL(clicked()), this, SLOT(random()));

    //Time Slider

    connect(ui->timeSlider, SIGNAL(sliderPressed()),this, SLOT(checkStreamStatus()));
    connect(ui->timeSlider, SIGNAL(valueChanged(int)), this, SLOT(updateTime(int)));
    connect(ui->timeSlider, SIGNAL(sliderReleased()),this, SLOT(checkStreamStatus()));

    //Exportation

    connect(ui->exportButton, SIGNAL(clicked(bool)),this, SLOT(exportMusic()));
    connect(ui->saveButton, SIGNAL(clicked(bool)),this,SLOT(saveWavFile()) );

    setStyle(0);
}

void SamplerModule::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QLineF line(m_line_pos, ui->btn_inst1_01->y()-10, m_line_pos, ui->btn_accords_01->y()+ui->btn_accords_01->height()+10);
    painter.drawLine(line);
}

SamplerModule::~SamplerModule()
{
    delete ui;

    if (m_streamData.samples == NULL)
        return;

    for (int i = 0 ; i < m_styles[0].total_samples ; ++i)
        delete[] (m_streamData.samples[i]);

    delete[] m_streamData.samples;
}

void SamplerModule::link(RingBuffer *ringBuffer, Filter * filter, RtAudio *audio)
{
    m_streamData.ringBuffer = ringBuffer;
    m_streamData.filter = filter;
    m_audio = audio;
}


void SamplerModule::enablePianoTone(int freq){
    m_streamData.pianoTone = true;
    m_streamData.pianoFreq = freq;
}

void SamplerModule::disablePianoTone(){
    m_streamData.pianoTone = false;
    m_streamData.pianoFreq = 0.0f;
}

void SamplerModule::setStyle(int style){

    m_style = style;

    emit instrumentChanged(m_styles[m_style].instruments[0]);
    emit instrumentChanged_2(m_styles[m_style].instruments[1]);
    emit instrumentChanged_3(m_styles[m_style].instruments[2]);

    //Chargement des samples du style

    if (m_activated){
        this->deactivate(); //On coupe la lecture
    }

    SNDFILE * sf = NULL; //contenant le futur fichier lu
    SF_INFO info; //contenant les infos du futur fichier lu
    info.format = 0; //décrit un fichier wav
    float * buffer = new float[nChannels*sampleSize];

    m_streamData.samples = new float*[m_styles[m_style].total_samples];//on créé le tableau de samples
    QString msg = "Les fichiers :\n";

    int ind = 0;
    for (int j=0;j<m_styles[m_style].nb_instruments;j++){
        for (int k=0;k<m_styles[m_style].nb_chords;k++){
            for (int l=0;l<m_styles[m_style].nb_samples[j][k];l++){

                m_streamData.samples[ind] = new float[sampleSize];//On crée le tableau de float
                QString qadress = "Samples/" + m_samples[m_style][j][k][l];
                const char* adress = qadress.toStdString().c_str();

                sf = sf_open(adress,SFM_READ,&info);//On ouvre le bon fichier
                if (sf == NULL){
                    msg += m_samples[m_style][j][k][l] + "\n";
                    for (int m=0;m<sampleSize;m++){
                        m_streamData.samples[ind][m]=0.0f;
                    }
                }else{
                    for (int m = info.channels * info.frames ; m < 2*sampleSize ; ++m){
                        buffer[m] = 0.0f; //au cas ou les samples sont trop petits
                    }
                    sf_read_float(sf,buffer,2*sampleSize);//on copie le fichier lu en stéréo dans le buffer
                    sf_close(sf);// On ferme le fichier ouvert
                    for (int m =0;m<sampleSize;m++){
                        m_streamData.samples[ind][m]=buffer[2*m];//On importe en mono dans le streamdata
                    }
                }
                ind++;
            }
        }
    }

    if(msg != "Les fichiers :\n"){
        QMessageBox msgBox;
        msgBox.setText(msg + "n'ont pas pu se charger");
        msgBox.exec();
    }

    delete[] buffer;
    emit resetState();
    updateTime(0);
    ui->timeSlider->setSliderPosition(0);

}

void SamplerModule::changeChord_01(){
    m_streamData.buttonsState[3][0] = (m_streamData.buttonsState[3][0]+1)%(m_styles[m_style].chords.size()+1);
    emit updateChord_01();
}

void SamplerModule::changeChord_02(){
    m_streamData.buttonsState[3][1] = (m_streamData.buttonsState[3][1]+1)%(m_styles[m_style].chords.size()+1);
    emit updateChord_02();
}

void SamplerModule::changeChord_03(){
    m_streamData.buttonsState[3][2] = (m_streamData.buttonsState[3][2]+1)%(m_styles[m_style].chords.size()+1);
    emit updateChord_03();
}

void SamplerModule::changeChord_04(){
    m_streamData.buttonsState[3][3] = (m_streamData.buttonsState[3][3]+1)%(m_styles[m_style].chords.size()+1);
    emit updateChord_04();
}

void SamplerModule::changeChord_05(){
    m_streamData.buttonsState[3][4] = (m_streamData.buttonsState[3][4]+1)%(m_styles[m_style].chords.size()+1);
    emit updateChord_05();
}

void SamplerModule::changeChord_06(){
    m_streamData.buttonsState[3][5] = (m_streamData.buttonsState[3][5]+1)%(m_styles[m_style].chords.size()+1);
    emit updateChord_06();
}

void SamplerModule::changeChord_07(){
    m_streamData.buttonsState[3][6] = (m_streamData.buttonsState[3][6]+1)%(m_styles[m_style].chords.size()+1);
    emit updateChord_07();
}

void SamplerModule::changeChord_08(){
    m_streamData.buttonsState[3][7] = (m_streamData.buttonsState[3][7]+1)%(m_styles[m_style].chords.size()+1);
    emit updateChord_08();
}

void SamplerModule::changeChord_09(){
    m_streamData.buttonsState[3][8] = (m_streamData.buttonsState[3][8]+1)%(m_styles[m_style].chords.size()+1);
    emit updateChord_09();
}

void SamplerModule::changeChord_10(){
    m_streamData.buttonsState[3][9] = (m_streamData.buttonsState[3][9]+1)%(m_styles[m_style].chords.size()+1);
    emit updateChord_10();
}

void SamplerModule::changeChord_11(){
    m_streamData.buttonsState[3][10] = (m_streamData.buttonsState[3][10]+1)%(m_styles[m_style].chords.size()+1);
    emit updateChord_11();
}

void SamplerModule::changeChord_12(){
    m_streamData.buttonsState[3][11] = (m_streamData.buttonsState[3][11]+1)%(m_styles[m_style].chords.size()+1);
    emit updateChord_12();
}

void SamplerModule::changeChord_13(){
    m_streamData.buttonsState[3][12] = (m_streamData.buttonsState[3][12]+1)%(m_styles[m_style].chords.size()+1);
    emit updateChord_13();
}

void SamplerModule::changeChord_14(){
    m_streamData.buttonsState[3][13] = (m_streamData.buttonsState[3][13]+1)%(m_styles[m_style].chords.size()+1);
    emit updateChord_14();
}

void SamplerModule::changeChord_15(){
    m_streamData.buttonsState[3][14] = (m_streamData.buttonsState[3][14]+1)%(m_styles[m_style].chords.size()+1);
    emit updateChord_15();
}

void SamplerModule::changeChord_16(){
    m_streamData.buttonsState[3][15] = (m_streamData.buttonsState[3][15]+1)%(m_styles[m_style].chords.size()+1);
    emit updateChord_16();
}


QVector<SamplerModule::Style> SamplerModule::getStyle(){
    return m_styles;
}

QString SamplerModule::getChordTitle(int chordSelect){
    chordStateId = m_streamData.buttonsState[3][chordSelect];
    if(chordStateId==0){
        return "off";
    }else{
        return m_styles[m_style].chords[chordStateId-1];
    }
}

void SamplerModule::setButtonState(int i, int j, int state){
    m_streamData.buttonsState[i][j] = state;
}

void SamplerModule::setLoopStates(int btnIndex){
    if(last_clicked && m_streamData.loopStates[0] != btnIndex){
        m_streamData.loopStates[1] = btnIndex;
    }else if(!last_clicked && m_streamData.loopStates[1] != btnIndex) {
        m_streamData.loopStates[0] = btnIndex;
    }

    last_clicked=(last_clicked+1)%2;

    emit UpdateAllLoopStates();
}

int SamplerModule::getButtonState(int i, int j){
    return m_streamData.buttonsState[i][j];
}

int SamplerModule::getLoopStates(int i){
    return m_streamData.loopStates[i];
}

int SamplerModule::getInstNbSamples(int inst, int chord){
    return m_styles[m_style].nb_samples[inst][chord];
}

void SamplerModule::reset(){
    if(m_activated){
        ui->timeSlider->setTracking(true);
        this->deactivate();
    }
    emit resetState();
    updateTime(0);
    ui->timeSlider->setSliderPosition(0);
}

void SamplerModule::activateAndPlay()
{
    m_activated = true;


    ui->timeSlider->setTracking(false);

    // Output
    int oDevice = m_audio->getDefaultOutputDevice();

    if (oDevice < 0)
        return;

    // Paramétrage
    m_bufferFrames = 256;
    RtAudio::StreamParameters oParams;
    oParams.deviceId = oDevice;
    oParams.nChannels = 2;

    RtAudio::StreamOptions options;
    options.flags = RTAUDIO_NONINTERLEAVED;

    try {
        int type = (sizeof(float) == 4) ? RTAUDIO_FLOAT32 : RTAUDIO_FLOAT64;
        m_audio->openStream(&oParams, NULL, type, FREQUENCE_ECHANTILLONNAGE, &m_bufferFrames, &inoutSampler, (void *)&m_streamData, &options);
    } catch (RtAudioError& e) {
        std::cout << "Error : " << e.getMessage() << std::endl;
        return;
    }

    if(m_audio->isStreamOpen())
        m_audio->startStream();
    else
        return;
}

void SamplerModule::deactivate()
{
    if(m_activated)
    {
        if (m_audio->isStreamRunning())
            m_audio->stopStream();

        if (m_audio->isStreamOpen())
            m_audio->closeStream();

        m_activated = false;

        ui->timeSlider->setTracking(true);

        ui->pausePlayButton->setStyleSheet("QPushButton{"
                                      "background-color: none;"
                                      "border:none;"
                                      "background-image: url(\":/play-button.png\");"
                                      "background-repeat: no-repeat;"
                                      "background-position: center center;"
                                      "color: white;"
                                      "height:28px;"
                                      "width: 28px;"
                                      "padding: 2px;}");
        ui->pausePlayButton->setToolTip("Play");
    }
}

void SamplerModule::moveLine(int time,int column){
    m_line_pos_min = ui->btn_inst1_01->x();
    m_line_pos_max = ui->btn_inst1_16->x()+ui->btn_inst1_16->width();
    double avance = (double)time/(44100*4);
    int pos_p = m_line_pos;
    int pos = m_line_pos_min + column*ui->btn_inst1_16->width()+floor(avance*ui->btn_inst1_01->width());
    QTime current_time(0,0,0,0);
    int timeMSec = (int) floor((column+avance)*4000);
    int sliderPosition = 1378.4*column+floor(((double)time)/128.75);
    current_time = current_time.addMSecs(timeMSec);
    ui->timeLabel->setText(current_time.toString("mm:ss,zzz"));
    m_line_pos = pos%m_line_pos_max;
    if(m_line_pos < m_line_pos_min){
        m_line_pos = m_line_pos_min;
    }
    if(pos_p!=m_line_pos){
        ui->timeSlider->setSliderPosition(sliderPosition);
        this->update();
    }
}

void SamplerModule::play() {
    if(!m_activated){
        emit activationRequest(this);
        ui->pausePlayButton->setStyleSheet("QPushButton{"
                                      "background-color: none;"
                                      "border:none;"
                                      "background-image: url(\":/pause-button.png\");"
                                      "background-repeat: no-repeat;"
                                      "background-position: center center;"
                                      "color: white;"
                                      "height:28px;"
                                      "width: 28px;"
                                      "padding: 2px;}");
        ui->pausePlayButton->setToolTip("Pause");
    }else{
        this->deactivate();
    }
}

void SamplerModule::checkStreamStatus() {
    if(m_activated){
        this->deactivate();
        wasPlaying = true;
    }else{
        if(wasPlaying){
            emit activationRequest(this);
            wasPlaying = false;
        }
    }

}

void SamplerModule::updateTime(int value){

    m_streamData.sliderValue[1] = (value*128)%(fe*4);
    int column =0;
    int val = value*128;
    while((val-(4*fe))>0){
        val -= (4*fe);
        column++;
    };
    m_streamData.sliderValue[0] = column;
    m_streamData.timeUpdated = false;
    followSlider(m_streamData.sliderValue[1], m_streamData.sliderValue[0]);

}

void SamplerModule::followSlider(int time,int column){
    m_line_pos_min = ui->btn_inst1_01->x();
    if (m_line_pos_min==0){m_line_pos_min=76;}; //cas de départ

    m_line_pos_max = ui->btn_inst1_16->x()+ui->btn_inst1_16->width();
    double avance = (double)time/(fe*4);
    int pos_p = m_line_pos;
    int pos = m_line_pos_min + column*ui->btn_inst1_16->width()+floor(avance*ui->btn_inst1_01->width());
    QTime current_time(0,0,0,0);
    int timeMSec = (int) floor((column+avance)*4000);
    current_time = current_time.addMSecs(timeMSec);
    ui->timeLabel->setText(current_time.toString("mm:ss,zzz"));
    m_line_pos = pos%m_line_pos_max;
    if(m_line_pos < m_line_pos_min){
        m_line_pos = m_line_pos_min;
    }
    if(pos_p!=m_line_pos){
        this->update();
    }
}

void SamplerModule::timeUpdateDone(){
    m_streamData.timeUpdated = true;
}

void SamplerModule::keyPressEvent(QKeyEvent * event )
{
    if(event->key() == Qt::Key_P){
        this->play();
    }else if(event->key() == Qt::Key_R || event->key() == Qt::Key_S){
        random();
    }
    if(event->key() == Qt::Key_C){
        reset();
    }
}

void SamplerModule::random(){
    for(int j=0; j<16; j++){
        m_streamData.buttonsState[3][j] = rand() % 4 + 1;
    }

    for(int i=0; i<3; i++){
        for(int j=0; j<16; j++){
            m_streamData.buttonsState[i][j] = rand() % (m_styles[m_style].nb_samples[i][m_streamData.buttonsState[3][j]]+1);
            if(i==2 && m_streamData.buttonsState[2][j] == 0 && m_streamData.buttonsState[0][j] == 0 &&  m_streamData.buttonsState[1][j] == 0){
                m_streamData.buttonsState[2][j] = 1;
            }
        }
    }

    emit updateAllButtons();
}

void SamplerModule::changeLoop(int loopCount){
    m_streamData.loopCount = loopCount;
}

void SamplerModule::exportMusic(){

    if(m_activated){
        this->deactivate();
    }
    QString filename = QFileDialog::getSaveFileName(this,"Exporter",QCoreApplication::applicationDirPath(),"Fichiers wav (*.wav)");

    ui->saveButton->setEnabled(true);

    m_currentWavFileName = filename;

    saveWavFile();
}

void SamplerModule::saveWavFile(){

    QFile file(m_currentWavFileName);

    if(!file.open(QIODevice::WriteOnly)){
        QMessageBox msgBox;
        msgBox.setText("Impossible d'écrire dans le fichier wav. Peut-être n'avez-vous pas les droits d'écriture dans le dossier spécifié.");
        msgBox.exec();

        return;
    }

    //Génération du Header

    QDataStream out(&file);
    out.setByteOrder(QDataStream::LittleEndian);
    // RIFF chunk
    out.writeRawData("RIFF", 4);
    out << quint32(0); // Placeholder for the RIFF chunk size (filled by close())
    out.writeRawData("WAVE", 4);

    // Format description chunk
    out.writeRawData("fmt ", 4);
    out << quint32(16); // "fmt " chunk size (always 16 for PCM)
    out << quint16(3); // data format (1 => PCM) (3 => IEEE Files)
    out << quint16(nChannels); //1 = mono, 2 = stereo
    out << quint32(fe); //fe
    out << quint32(fe*nBlockAlign); // bytes per second
    out << quint16(nBlockAlign); // Block align
    out << quint16(nBitsPerSample); // Significant Bits Per Sample

    // Data chunk
    out.writeRawData("data", 4);
    out << quint32(0); // Placeholder for the data chunk size (filled by close())

    Q_ASSERT(file.size() == 44);

    // Must be 44 for WAV PCM


    float * beforeFilterBuffer = NULL;
    if (beforeFilterBuffer  == NULL) beforeFilterBuffer = new float[4*nChannels*fe];

    int trackSize = 16 + m_streamData.loopCount*abs(m_streamData.loopStates[0]-m_streamData.loopStates[1]);
    if(max(m_streamData.loopStates[0],m_streamData.loopStates[1])==17){
        trackSize = 16;
    }
    float * trackBuffer = new float[nChannels*4*fe*trackSize];
    for(int i=0;i<nChannels*4*fe*trackSize;i++){
        trackBuffer[i]=0;
    }

    QVector<int> indexes;

    if (m_streamData.loopCount>0 && max(m_streamData.loopStates[0],m_streamData.loopStates[1])!=17){
        int inf = min(m_streamData.loopStates[0],m_streamData.loopStates[1])-1;
        int sup = max(m_streamData.loopStates[0],m_streamData.loopStates[1])-1;
        for(int i=0;i<inf;i++){
            indexes.push_back(i);
        }
        for(int i=0;i<m_streamData.loopCount+1;i++){
            for(int j=inf;j<sup;j++){
                indexes.push_back(j);
            }
        }
        for(int i=sup;i<16;i++){
            indexes.push_back(i);
        }
    }else{
        for(int i=0;i<16;i++){
            indexes.push_back(i);
        }
    }
    for(int i=0;i<trackSize;i++){
        int chord = m_streamData.buttonsState[3][indexes[i]];
        for(int inst=0;inst<3;inst++){
            if(m_streamData.buttonsState[inst][indexes[i]]){
                for(int p=0;p<4*fe;p++){
                    trackBuffer[i*4*fe*nChannels+2*p] += m_streamData.samples[15*inst+3*(chord-1)+m_streamData.buttonsState[inst][indexes[i]]-1][p];
                    trackBuffer[i*4*fe*nChannels+2*p+1] += m_streamData.samples[15*inst+3*(chord-1)+m_streamData.buttonsState[inst][indexes[i]]-1][p];
                }
            }
            qDebug()<<inst;
        }
        qDebug()<<i;
    }
    for(int i=0;i<nChannels*4*fe*trackSize;i++){
        out<<trackBuffer[i];
    }
    out.setByteOrder(QDataStream::LittleEndian);
    quint32 filesize = file.size();
    file.seek(4);
    out << quint32(filesize - 8);

    // data chunk size
    file.seek(40);
    out << quint32(filesize - 44);

    file.close();
}

bool SamplerModule::midiInputEvent(MidiEngine::MidiSoftwareInput input, int value)
{
    Q_UNUSED(value);

    switch (input) {
    case MidiEngine::MidiPlay:
    case MidiEngine::MidiSourceSlider1:
    case MidiEngine::MidiSourceSlider2:
        return true;
    case MidiEngine::MidiSampleBass:
        if (value > 0)
            //ui->tristate_bass->setState();
            return true;
    case MidiEngine::MidiSampleMelody:
        if (value > 0)
            // ui->tristate_guitar->setState();
            return true;
    case MidiEngine::MidiSampleEffect:
        if (value > 0)
            // ui->tristate_soundeffect->setState();
            return true;
    case MidiEngine::MidiStopBass:
        if (value > 0)
            // ui->tristate_bass->reset();
            return true;
    case MidiEngine::MidiStopMelody:
        if (value > 0)
            //  ui->tristate_guitar->reset();
            return true;
    case MidiEngine::MidiStopEffect:
        if (value > 0)
            // ui->tristate_soundeffect->reset();
            return true;
    default:
        return false;
    }
}
