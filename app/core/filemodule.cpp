#include <QtMath>
#include <QFileDialog>
#include "constants.h"
#include "filemodule.h"
#include "ui_filemodule.h"
#include "../midi/midiengine.h"

#include <QMessageBox>

#include <QDebug>

int inoutFile(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void *streamData)
{
    static int count = 0;
    static float * bichannelBuffer = NULL;
    static float * beforeFilterBuffer = NULL;
    if (bichannelBuffer     == NULL) bichannelBuffer = new float[nBufferFrames * 2]; // Memory leak here
    if (beforeFilterBuffer  == NULL) beforeFilterBuffer = new float[nBufferFrames];  // And here

    count++;

    Q_UNUSED(streamTime);
    Q_UNUSED(status);
    Q_UNUSED(inputBuffer);

    FileModule::StreamData *data = (FileModule::StreamData*)streamData;

    if (data->monochannel) {
        if (sf_read_float(data->file, (float*) beforeFilterBuffer, nBufferFrames) != nBufferFrames) {
            return 1;
        }
    } else {
        if (sf_read_float(data->file, (float*) bichannelBuffer, nBufferFrames * 2) != nBufferFrames * 2) {
            return 1;
        }

        float * ptrOut      = (float *) beforeFilterBuffer;
        const float * end   = &(bichannelBuffer[nBufferFrames*2]);
        for (float * ptrIn = &(bichannelBuffer[0]) ; ptrIn < end ; ptrIn += 2)
            *(ptrOut++) = *ptrIn;
    }

    if(data->pianoTone)
        EngineModule::addPianoTone(beforeFilterBuffer, nBufferFrames, data->pianoFreq);

    data->filter->applyOnBuffer((float *) beforeFilterBuffer, (float *) outputBuffer, nBufferFrames);
    data->currentBuffer->addBuffer((float*) outputBuffer, nBufferFrames);

    memcpy((void*) &(((float*)outputBuffer)[nBufferFrames]), outputBuffer, nBufferFrames * sizeof(float));
    return 0;
}


FileModule::FileModule(QWidget *parent) : EngineModule(parent), ui(new Ui::FileModule)
{
    ui->setupUi(this);

    m_streamData.currentBuffer = NULL;
    m_streamData.file = NULL;

    m_streamData.pianoTone = false;
    m_streamData.pianoFreq = 0.0f;

    ui->layout_file_progress->addWidget(&m_progressWidget);
    m_progressWidget.link(ui->label_file_progressText);
    m_progressWidget.reset();

    connect(ui->pushButton_controls_play_file, SIGNAL(clicked(bool)), this, SLOT(playButtonPressed()));
    connect(ui->pushButton_controls_pause_file, SIGNAL(clicked(bool)), this, SLOT(pauseButtonPressed()));
    connect(ui->pushButton_controls_openFile, SIGNAL(clicked(bool)), this, SLOT(openButtonPressed()));
    connect(&m_progressWidget, SIGNAL(userRequest(float)), this, SLOT(setFileProgress(float)));
    connect(&m_timerProgress, SIGNAL(timeout()), this, SLOT(updateProgress()));
}

FileModule::~FileModule()
{
    delete ui;
}

void FileModule::link(RingBuffer *ringBuffer, Filter *filter, RtAudio *audio)
{
    m_streamData.currentBuffer = ringBuffer;
    m_streamData.filter = filter;
    m_audio = audio;

}


void FileModule::activateAndPlay()
{
    m_activated = true;

    openFile();

    if(m_audio->isStreamOpen())
        m_audio->startStream();
    else
        return;

    if(m_audio->isStreamRunning())
    {
        ui->pushButton_controls_play_file->setEnabled(false);
        ui->pushButton_controls_pause_file->setEnabled(true);
        m_timerProgress.start(250);
    }
}

void FileModule::deactivate()
{
    if (m_audio->isStreamRunning())
        m_audio->stopStream();

     if (m_audio->isStreamOpen())
         m_audio->closeStream();

     if(m_streamData.file != NULL)
     {
         sf_close(m_streamData.file);
         m_streamData.file = NULL;
     }

     m_timerProgress.stop();
     ui->pushButton_controls_play_file->setEnabled(true);
     ui->pushButton_controls_pause_file->setEnabled(false);

     m_activated = false;

     m_endOfFile = false;
}

void FileModule::enablePianoTone(int freq){
    m_streamData.pianoTone = true;
    m_streamData.pianoFreq = freq;

}

void FileModule::disablePianoTone(){
    m_streamData.pianoTone = false;
    m_streamData.pianoFreq = 0.0f;
}

void FileModule::playButtonPressed()
{
    if(m_activated)
    {

        if(m_audio->isStreamOpen())
        {
            if(!m_audio->isStreamRunning()){

                if(m_endOfFile){
                    sf_seek(m_streamData.file, 0, SEEK_SET);
                    m_endOfFile = false;
                }

                m_audio->startStream();
            }

            if(m_audio->isStreamRunning())
            {
                m_timerProgress.start(250);
                ui->pushButton_controls_play_file->setEnabled(false);
                ui->pushButton_controls_pause_file->setEnabled(true);
            }
        }
        else
            activateAndPlay();
    }
    else
        emit activationRequest(this);
}

void FileModule::pauseButtonPressed()
{
    if (m_audio->isStreamRunning())
        m_audio->stopStream();

    ui->pushButton_controls_play_file->setEnabled(true);
    ui->pushButton_controls_pause_file->setEnabled(false);
}

void FileModule::openButtonPressed()
{
    QString filename = QFileDialog::getOpenFileName(this, "Ouvrir un fichier", QCoreApplication::applicationDirPath(), "Fichiers audio (*.wav)");

    if (filename != "")
    {
        ui->pushButton_controls_openFile->setText(QFileInfo(filename).baseName());
        m_filename = filename;

        if(m_activated)
        {
            if(m_audio->isStreamOpen())
            {
                if(m_audio->isStreamRunning())
                    m_audio->stopStream();
                m_audio->closeStream();
                if(m_streamData.file != NULL)
                {
                    sf_close(m_streamData.file);
                    m_streamData.file = NULL;
                }
                m_endOfFile = false;
                openFile();
                ui->pushButton_controls_play_file->setEnabled(true);
                ui->pushButton_controls_pause_file->setEnabled(false);
            }
        }
    }
}

void FileModule::updateProgress() {

    m_endOfFile = m_progressWidget.setProgressInSeconds(sf_seek(m_streamData.file, 0, SEEK_CUR) / FREQUENCE_ECHANTILLONNAGE);

    if(m_endOfFile){
        ui->pushButton_controls_play_file->setEnabled(true);
        ui->pushButton_controls_pause_file->setEnabled(false);
    }
}

void FileModule::setFileProgress(float seconds)
{
    if(m_streamData.file != NULL)
        sf_seek(m_streamData.file, (int)(seconds * FREQUENCE_ECHANTILLONNAGE), SEEK_SET);
}

void FileModule::openFile()
{
    if (m_filename == "")
        return;

    // Output
    int oDevice = m_audio->getDefaultOutputDevice();

    if (oDevice < 0)
        return;

    SF_INFO info;

    // Parametrage de libsnfile et ouverture du fichier
    info.format = 0;

    if(m_streamData.file != NULL)
    {
        sf_close(m_streamData.file);
        m_streamData.file = NULL;
    }

    m_streamData.file = sf_open(m_filename.toStdString().c_str(), SFM_READ, &info);

    if(m_streamData.file == NULL)
    {
        QMessageBox msgBox;
        msgBox.setText("Une erreur est survenue à l'ouverture du fichier, assurez vous que le chemin d'accès ne contient pas de caractères spéciaux (y compris des accents)");
        msgBox.exec();

        std::cout << "Error opening file : " << sf_strerror(NULL) << std::endl;
        return;
    }

    if(info.channels > 2)
    {
        std::cout << "Error opening file : File must be have a maximum of two channels" << std::endl;
        return;
    }

    m_progressWidget.setTotalLength(info.frames / FREQUENCE_ECHANTILLONNAGE);


    // Paramétrage du stream
    m_bufferFrames = 256;
    RtAudio::StreamParameters oParams;
    oParams.deviceId = oDevice;
    oParams.nChannels = 2;

    RtAudio::StreamOptions options;
    options.flags = RTAUDIO_NONINTERLEAVED;

    m_streamData.monochannel = (info.channels == 1);

    if (m_audio->isStreamOpen())
        m_audio->closeStream(); // Au cas où le flux n'aurait pas été fermé avant


    // Ouverture du flux
    try {
        int type = (sizeof(float) == 4) ? RTAUDIO_FLOAT32 : RTAUDIO_FLOAT64;
        m_audio->openStream(&oParams, NULL, type, FREQUENCE_ECHANTILLONNAGE, &m_bufferFrames, &inoutFile, (void *)&m_streamData, &options);
    } catch (RtAudioError& e) {
        std::cout << "Error : " << e.getMessage() << std::endl;
        return;
    }
}

void FileModule::pause() {
    pauseButtonPressed();
}

bool FileModule::midiInputEvent(MidiEngine::MidiSoftwareInput input, int value)
{
    switch (input) {
    case MidiEngine::MidiPlay:
        if (m_activated)
            playButtonPressed();
        else
            emit activationRequest(this);
        return true;
    case MidiEngine::MidiSourceSlider1:
        m_progressWidget.setProgress(1000 * value / 127);
    case MidiEngine::MidiSourceSlider2:
        return true;
    default:
        return false;
    }
}

