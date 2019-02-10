#include "mididialog.h"
#include "ui_mididialog.h"
#include <QShowEvent>
#include <QModelIndex>
#include <QStringList>
#include <QFileDialog>

MidiDialog::MidiDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MidiDialog),
    m_midiEngine(NULL)
{
    ui->setupUi(this);

    layout()->setSizeConstraint( QLayout::SetFixedSize );

    connect(ui->listWidget_inputs, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(validateSelectedSource(QModelIndex)));
    connect(ui->pushButton_refreshList, SIGNAL(clicked(bool)), this, SLOT(refreshInputsList()));
    connect(ui->pushButton_reinitializeAssociations, SIGNAL(clicked(bool)), this, SLOT(reinitializeAssociations()));

    connect(ui->pushButton_midiConfig_open,     SIGNAL(pressed()), this, SLOT(openConfigFile()));
    connect(ui->pushButton_midiConfig_save,     SIGNAL(pressed()), this, SLOT(saveConfigToFile()));
    connect(ui->pushButton_midiConfig_newFile,  SIGNAL(pressed()), this, SLOT(createAndSaveConfigFile()));

    connect(ui->associationButton_nextSource,               SIGNAL(clicked(bool)), this, SLOT(associationButtonClicked()));
    connect(ui->associationButton_previousSource,           SIGNAL(clicked(bool)), this, SLOT(associationButtonClicked()));
    connect(ui->associationButton_play,                     SIGNAL(clicked(bool)), this, SLOT(associationButtonClicked()));
    connect(ui->associationButton_pause,                    SIGNAL(clicked(bool)), this, SLOT(associationButtonClicked()));
    connect(ui->associationButton_sourceSlider1,            SIGNAL(clicked(bool)), this, SLOT(associationButtonClicked()));
    connect(ui->associationButton_sourceSlider2,            SIGNAL(clicked(bool)), this, SLOT(associationButtonClicked()));
    connect(ui->associationButton_sampleBass,               SIGNAL(clicked(bool)), this, SLOT(associationButtonClicked()));
    connect(ui->associationButton_sampleMelody,             SIGNAL(clicked(bool)), this, SLOT(associationButtonClicked()));
    connect(ui->associationButton_sampleEffect,             SIGNAL(clicked(bool)), this, SLOT(associationButtonClicked()));
    connect(ui->associationButton_stopBass,                 SIGNAL(clicked(bool)), this, SLOT(associationButtonClicked()));
    connect(ui->associationButton_stopMelody,               SIGNAL(clicked(bool)), this, SLOT(associationButtonClicked()));
    connect(ui->associationButton_stopEffect,               SIGNAL(clicked(bool)), this, SLOT(associationButtonClicked()));
    connect(ui->associationButton_pianoButton,              SIGNAL(clicked(bool)), this, SLOT(associationButtonClicked()));
    connect(ui->associationButton_pianoSwitch,              SIGNAL(clicked(bool)), this, SLOT(associationButtonClicked()));
    connect(ui->associationButton_maximaButton,             SIGNAL(clicked(bool)), this, SLOT(associationButtonClicked()));
    connect(ui->associationButton_maximaSwitch,             SIGNAL(clicked(bool)), this, SLOT(associationButtonClicked()));
    connect(ui->associationButton_autoscaleButton,          SIGNAL(clicked(bool)), this, SLOT(associationButtonClicked()));
    connect(ui->associationButton_autoscaleSwitch,          SIGNAL(clicked(bool)), this, SLOT(associationButtonClicked()));
    connect(ui->associationButton_zoomPlus,                 SIGNAL(clicked(bool)), this, SLOT(associationButtonClicked()));
    connect(ui->associationButton_zoomMinus,                SIGNAL(clicked(bool)), this, SLOT(associationButtonClicked()));
    connect(ui->associationButton_autoadjust,               SIGNAL(clicked(bool)), this, SLOT(associationButtonClicked()));
    connect(ui->associationButton_periodSlider,             SIGNAL(clicked(bool)), this, SLOT(associationButtonClicked()));
    connect(ui->associationButton_synchroButton,            SIGNAL(clicked(bool)), this, SLOT(associationButtonClicked()));
    connect(ui->associationButton_synchroSwitch,            SIGNAL(clicked(bool)), this, SLOT(associationButtonClicked()));
    connect(ui->associationButton_prevFilter,               SIGNAL(clicked(bool)), this, SLOT(associationButtonClicked()));
    connect(ui->associationButton_nextFilter,               SIGNAL(clicked(bool)), this, SLOT(associationButtonClicked()));
    connect(ui->associationButton_filterAmplitudeSlider,    SIGNAL(clicked(bool)), this, SLOT(associationButtonClicked()));
    connect(ui->associationButton_filterFreq1,              SIGNAL(clicked(bool)), this, SLOT(associationButtonClicked()));
    connect(ui->associationButton_filterFreq2,              SIGNAL(clicked(bool)), this, SLOT(associationButtonClicked()));
    connect(ui->associationButton_lowerNote,                SIGNAL(clicked(bool)), this, SLOT(associationButtonClicked()));
    connect(ui->associationButton_upperNote,                SIGNAL(clicked(bool)), this, SLOT(associationButtonClicked()));

    m_midiSoftwareInputs["associationButton_nextSource"]            = MidiEngine::MidiNextSource;
    m_midiSoftwareInputs["associationButton_previousSource"]        = MidiEngine::MidiPreviousSource;
    m_midiSoftwareInputs["associationButton_play"]                  = MidiEngine::MidiPlay;
    m_midiSoftwareInputs["associationButton_pause"]                 = MidiEngine::MidiPause;
    m_midiSoftwareInputs["associationButton_sourceSlider1"]         = MidiEngine::MidiSourceSlider1;
    m_midiSoftwareInputs["associationButton_sourceSlider2"]         = MidiEngine::MidiSourceSlider2;
    m_midiSoftwareInputs["associationButton_sampleBass"]            = MidiEngine::MidiSampleBass;
    m_midiSoftwareInputs["associationButton_sampleMelody"]          = MidiEngine::MidiSampleMelody;
    m_midiSoftwareInputs["associationButton_sampleEffect"]          = MidiEngine::MidiSampleEffect;
    m_midiSoftwareInputs["associationButton_stopBass"]              = MidiEngine::MidiStopBass;
    m_midiSoftwareInputs["associationButton_stopMelody"]            = MidiEngine::MidiStopMelody;
    m_midiSoftwareInputs["associationButton_stopEffect"]            = MidiEngine::MidiStopEffect;
    m_midiSoftwareInputs["associationButton_pianoButton"]           = MidiEngine::MidiPianoToggle;
    m_midiSoftwareInputs["associationButton_pianoSwitch"]           = MidiEngine::MidiPianoCheckbox;
    m_midiSoftwareInputs["associationButton_maximaButton"]          = MidiEngine::MidiMaximaToggle;
    m_midiSoftwareInputs["associationButton_maximaSwitch"]          = MidiEngine::MidiMaximaCheckbox;
    m_midiSoftwareInputs["associationButton_autoscaleButton"]       = MidiEngine::MidiSpectrumAutoscaleToggle;
    m_midiSoftwareInputs["associationButton_autoscaleSwitch"]       = MidiEngine::MidiSpectrumAutoscaleCheckbox;
    m_midiSoftwareInputs["associationButton_zoomPlus"]              = MidiEngine::MidiSpectrumZoomPlus;
    m_midiSoftwareInputs["associationButton_zoomMinus"]             = MidiEngine::MidiSpectrumZoomMinus;
    m_midiSoftwareInputs["associationButton_autoadjust"]            = MidiEngine::MidiAutoAdjustWaveform;
    m_midiSoftwareInputs["associationButton_periodSlider"]          = MidiEngine::MidiWaveformPeriodSlider;
    m_midiSoftwareInputs["associationButton_synchroButton"]         = MidiEngine::MidiWaveformSynchroToggle;
    m_midiSoftwareInputs["associationButton_synchroSwitch"]         = MidiEngine::MidiWaveformSynchroSwitch;
    m_midiSoftwareInputs["associationButton_prevFilter"]            = MidiEngine::MidiPreviousFilter;
    m_midiSoftwareInputs["associationButton_nextFilter"]            = MidiEngine::MidiNextFilter;
    m_midiSoftwareInputs["associationButton_filterAmplitudeSlider"] = MidiEngine::MidiFilterAmplitudeSlider;
    m_midiSoftwareInputs["associationButton_filterFreq1"]           = MidiEngine::MidiFilterFrequencySlider1;
    m_midiSoftwareInputs["associationButton_filterFreq2"]           = MidiEngine::MidiFilterFrequencySlider2;
    m_midiSoftwareInputs["associationButton_lowerNote"]             = MidiEngine::MidiPianoMin;
    m_midiSoftwareInputs["associationButton_upperNote"]             = MidiEngine::MidiPianoMax;

    m_associationButtons[MidiEngine::MidiNextSource]                = ui->associationButton_nextSource;
    m_associationButtons[MidiEngine::MidiPreviousSource]            = ui->associationButton_previousSource;
    m_associationButtons[MidiEngine::MidiPlay]                      = ui->associationButton_play;
    m_associationButtons[MidiEngine::MidiPause]                     = ui->associationButton_pause;
    m_associationButtons[MidiEngine::MidiSourceSlider1]             = ui->associationButton_sourceSlider1;
    m_associationButtons[MidiEngine::MidiSourceSlider2]             = ui->associationButton_sourceSlider2;
    m_associationButtons[MidiEngine::MidiSampleBass]                = ui->associationButton_sampleBass;
    m_associationButtons[MidiEngine::MidiSampleMelody]              = ui->associationButton_sampleMelody;
    m_associationButtons[MidiEngine::MidiSampleEffect]              = ui->associationButton_sampleEffect;
    m_associationButtons[MidiEngine::MidiStopBass]                  = ui->associationButton_stopBass;
    m_associationButtons[MidiEngine::MidiStopMelody]                = ui->associationButton_stopMelody;
    m_associationButtons[MidiEngine::MidiStopEffect]                = ui->associationButton_stopEffect;
    m_associationButtons[MidiEngine::MidiPianoToggle]               = ui->associationButton_pianoButton;
    m_associationButtons[MidiEngine::MidiPianoCheckbox]             = ui->associationButton_pianoSwitch;
    m_associationButtons[MidiEngine::MidiMaximaToggle]              = ui->associationButton_maximaButton;
    m_associationButtons[MidiEngine::MidiMaximaCheckbox]            = ui->associationButton_maximaSwitch;
    m_associationButtons[MidiEngine::MidiSpectrumAutoscaleToggle]   = ui->associationButton_autoscaleButton;
    m_associationButtons[MidiEngine::MidiSpectrumAutoscaleCheckbox] = ui->associationButton_autoscaleSwitch;
    m_associationButtons[MidiEngine::MidiSpectrumZoomPlus]          = ui->associationButton_zoomPlus;
    m_associationButtons[MidiEngine::MidiSpectrumZoomMinus]         = ui->associationButton_zoomMinus;
    m_associationButtons[MidiEngine::MidiAutoAdjustWaveform]        = ui->associationButton_autoadjust;
    m_associationButtons[MidiEngine::MidiWaveformPeriodSlider]      = ui->associationButton_periodSlider;
    m_associationButtons[MidiEngine::MidiWaveformSynchroToggle]     = ui->associationButton_synchroButton;
    m_associationButtons[MidiEngine::MidiWaveformSynchroSwitch]     = ui->associationButton_synchroSwitch;
    m_associationButtons[MidiEngine::MidiPreviousFilter]            = ui->associationButton_prevFilter;
    m_associationButtons[MidiEngine::MidiNextFilter]                = ui->associationButton_nextFilter;
    m_associationButtons[MidiEngine::MidiFilterAmplitudeSlider]     = ui->associationButton_filterAmplitudeSlider;
    m_associationButtons[MidiEngine::MidiFilterFrequencySlider1]    = ui->associationButton_filterFreq1;
    m_associationButtons[MidiEngine::MidiFilterFrequencySlider2]    = ui->associationButton_filterFreq2;
    m_associationButtons[MidiEngine::MidiPianoMin]                  = ui->associationButton_lowerNote;
    m_associationButtons[MidiEngine::MidiPianoMax]                  = ui->associationButton_upperNote;
}

MidiDialog::~MidiDialog()
{
    delete ui;
}

void MidiDialog::link(MidiEngine *midiEngine) {
    m_midiEngine = midiEngine;

    connect(m_midiEngine, SIGNAL(midiInputLearned(MidiEngine::MidiSoftwareInput, int)), this, SLOT(midiInputLearned(MidiEngine::MidiSoftwareInput, int)));
    connect(m_midiEngine, SIGNAL(midiInputLearningFailed(MidiEngine::MidiSoftwareInput)), this, SLOT(midiInputLearningFailed(MidiEngine::MidiSoftwareInput)));

    const QFileInfo defaultFile(QCoreApplication::applicationDirPath() + QString("\\default.midicfg"));
    if (defaultFile.exists() && defaultFile.isFile())
        openConfigFile(QCoreApplication::applicationDirPath() + QString("\\default.midicfg"));
}

void MidiDialog::showEvent(QShowEvent * event) {
    if (!event->spontaneous()) {
        refreshInputsList();
        if (ui->listWidget_inputs->count() != 0)
            ui->label_currentSource->setText(ui->listWidget_inputs->item(0)->text());
        m_midiEngine->blockInputs();
    }

    QDialog::showEvent(event);
}

void MidiDialog::accept() {
    m_midiEngine->unblockInputs();

    QDialog::accept();
}

void MidiDialog::reject() {
    m_midiEngine->unblockInputs();

    QDialog::reject();
}

void MidiDialog::reinitializeAssociations() {
    m_midiEngine->clearAssociations();

    for (QMap< MidiEngine::MidiSoftwareInput, QPushButton * >::iterator it = m_associationButtons.begin() ; it != m_associationButtons.end() ; ++it) {
        QPushButton * button = *it;

        button->setText("Apprendre");
        button->setDown(false);
    }
}

void MidiDialog::refreshInputsList() {
    ui->listWidget_inputs->clear();
    ui->listWidget_inputs->addItems(m_midiEngine->getAvailableInputs());
}

void MidiDialog::validateSelectedSource(const QModelIndex & index) {
    m_midiEngine->selectSource(index.data().toString());
    reinitializeAssociations();

    ui->label_currentSource->setText(index.data().toString()); // On affiche le nom de la source sélectionnée
}

void MidiDialog::midiInputLearned(MidiEngine::MidiSoftwareInput input, int control) {
    ui->pushButton_midiConfig_save->setEnabled(true);
    QPushButton * button = m_associationButtons[input];

    button->setText("Appris: " + QString::number(control));
    button->setDown(false);
}

void MidiDialog::midiInputLearningFailed(MidiEngine::MidiSoftwareInput input) {
    ui->pushButton_midiConfig_save->setEnabled(true);
    QPushButton * button = m_associationButtons[input];

    button->setText("Echec");
    button->setDown(false);
}

void MidiDialog::associationButtonClicked() {
    QPushButton * sender = (QPushButton *) QObject::sender();

    m_midiEngine->learnInput(m_midiSoftwareInputs[sender->objectName()]);
    sender->setText("Sur écoute...");
    sender->setDown(true);
}

void MidiDialog::openConfigFile(QString filename){
    if (filename == "")
        filename = QFileDialog::getOpenFileName(this, "Ouvrir un fichier", QCoreApplication::applicationDirPath(), "Fichiers de configuration (*.midicfg)");

    if(filename != ""){
        reinitializeAssociations();
        ui->label_midiConfig_path->setText(QFileInfo(filename).baseName());
        m_midiEngine->openFile(filename);
        ui->pushButton_midiConfig_save->setEnabled(false);
    }
}

void MidiDialog::saveConfigToFile(){
    m_midiEngine->saveConfig();
    ui->pushButton_midiConfig_save->setEnabled(false);
}

void MidiDialog::createAndSaveConfigFile() {
   QString filename = QFileDialog::getSaveFileName(this, "Créer un fichier de configuration", QCoreApplication::applicationDirPath(), "Fichiers de configuration (*.midicfg)");

   if(filename != "") {
       ui->label_midiConfig_path->setText(QFileInfo(filename).baseName());
       m_midiEngine->createConfigFile(filename);
       ui->pushButton_midiConfig_save->setEnabled(false);
   }
}
