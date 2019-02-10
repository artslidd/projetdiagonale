#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "filterwidget.h"
#include "../tools/frequenciesmanager.h"

#include <QtGlobal>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), m_pushButton_about(this)
{
    ui->setupUi(this);

    setWindowTitle("Projet MISS");
    setWindowIcon(QIcon(QPixmap(":/dial_back.png")));
    centralWidget()->setStyleSheet("QWidget {"
                                   "    color: white;"
                                   "}"
                                   ""
                                   "QWidget#" + centralWidget()->objectName() + " {"
                                   "    background-image: url(\":/background.png\");"
                                   "}"
                                   ""
                                   "QFrame#frame_filter_panel {"
                                   "    border-image: url(\":/filter_panel.png\") 0 0 0 0 stretch stretch;"
                                   "}"
                                   ""
                                   "QComboBox {"
                                   "    border-color: grey;"
                                   "    background-color: black;"
                                   "}"
                                   ""
                                   "QProgressBar::chunk {"
                                   "    background: #9a94e7;"
                                   "}"
                                   ""
                                   "QComboBox QListView {"
                                   "    border-color: grey;"
                                   "    background-color: black;"
                                   "}"
                                   ""
                                   "QLabel#label_filter_level, QLabel#label_filter_freq {"
                                   "    color: black;"
                                   "}"
                                   ""
                                   "QLabel#label_filter_6dB {"
                                   "    color: #999999;"
                                   "}"
                                   ""
                                   "QFrame#frame_waveform_freq, QFrame#frame_waveform_synchro, QFrame#frame_basicsignalfrequency, QFrame#frame_filter_freq {"
                                   "    border-image: url(\":/frame_background.png\") 0 0 0 0 stretch stretch;"
                                   "}");


	// Tabs
    ui->layout_tab_microphone->addWidget(&m_externalSourceModule);
    ui->layout_tab_signal->addWidget(&m_basicSignalModule);
    ui->layout_tab_file->addWidget(&m_fileModule);
    ui->layout_tab_sampler->addWidget(&m_samplerModule);
    ui->stackedWidget_source->setCurrentIndex(0);
    ui->stackedWidget_source->setContentsMargins(0,0,0,0);
    connect(ui->sourceSelector, SIGNAL(newTabSelected(int)), ui->stackedWidget_source, SLOT(setCurrentIndex(int)));

    // MIDI
    m_midiDialog.link(&m_midiEngine);
    connect(ui->sourceSelector, SIGNAL(midiConfigPanelRequested()), &m_midiDialog, SLOT(exec()));
    connect(&m_midiEngine, SIGNAL(midiInputEvent(MidiEngine::MidiSoftwareInput,int)), this, SLOT(midiInputEvent(MidiEngine::MidiSoftwareInput,int)));

	// Filter
	ui->layout_filtrage->addWidget(&m_filterWidget);

    connect(ui->dial_filter_amplitude, SIGNAL(valueChanged(int)), &m_filterWidget, SLOT(amplitudeChanged(int)));
    connect(ui->doubleSlider, SIGNAL(singleChange(int)), &m_filterWidget, SLOT(frequencyChangedSingle(int)));
    connect(ui->doubleSlider, SIGNAL(doubleChange(int,int)), &m_filterWidget, SLOT(frequencyChangedDouble(int,int)));
    connect(&m_filterWidget, SIGNAL(singleMode()), ui->doubleSlider, SLOT(switchToSingle()));
    connect(&m_filterWidget, SIGNAL(doubleMode()), ui->doubleSlider, SLOT(switchToDouble()));

	connect(ui->radioButton_desactive, SIGNAL(toggled(bool)), &m_filterWidget, SLOT(setNone(bool)));
	connect(ui->radioButton_passebande, SIGNAL(toggled(bool)), &m_filterWidget, SLOT(setBandPass(bool)));
	connect(ui->radioButton_passebas, SIGNAL(toggled(bool)), &m_filterWidget, SLOT(setLowPass(bool)));
	connect(ui->radioButton_passehaut, SIGNAL(toggled(bool)), &m_filterWidget, SLOT(setHighPass(bool)));
	connect(ui->radioButton_coupebande, SIGNAL(toggled(bool)), &m_filterWidget, SLOT(setBandCut(bool)));

    connect(ui->checkBox_filter_attenuation, SIGNAL(clicked(bool)), &m_filterWidget, SLOT(attenuationSet(bool)));

	// Waveform
	ui->layout_visualisation->addWidget(&m_waveformWidget);

    connect(ui->slider_visualization_period, SIGNAL(singleChange(int)), &m_waveformWidget, SLOT(setDisplayedInterval(int)));
	connect(&m_waveformWidget, SIGNAL(displayedStringChanged(QString)), ui->label_visualization_period, SLOT(setText(QString)));
	connect(ui->checkBox_visualization_synchro, SIGNAL(toggled(bool)), &m_waveformWidget, SLOT(enableTrigger(bool)));
    connect(ui->pushButton_visualization_autoAdjust, SIGNAL(clicked(bool)), ui->checkBox_visualization_synchro, SLOT(check()));
	connect(ui->pushButton_visualization_autoAdjust, SIGNAL(clicked(bool)), &m_waveformWidget, SLOT(automaticAdjust()));
    connect(&m_waveformWidget, SIGNAL(displayedIntervalChanged(int)), ui->slider_visualization_period, SLOT(setLeftCursor(int)));

    connect(ui->sourceSelector, SIGNAL(hideWaveform()), &m_waveformWidget, SLOT(hide()));
    connect(ui->sourceSelector, SIGNAL(showWaveform()), &m_waveformWidget, SLOT(show()));

	// Spectrum
	ui->layout_spectrum->addWidget(&m_spectrumWidget);
    connect(ui->checkBox_spectrum_autoscale, SIGNAL(stateChanged(int)), &m_spectrumWidget, SLOT(setAutoscale(int)));
    connect(ui->checkBox_spectrum_piano, SIGNAL(stateChanged(int)), ui->switchable_label, SLOT(switchMode()));
    connect(ui->checkBox_spectrum_piano, SIGNAL(stateChanged(int)), &m_spectrumWidget, SLOT(switchMode()));

    // Piano tones
    connect(ui->switchable_label, SIGNAL(pianoKeyPressed(int)), &m_externalSourceModule, SLOT(enablePianoTone(int)));
    connect(ui->switchable_label, SIGNAL(pianoKeyReleased()), &m_externalSourceModule, SLOT(disablePianoTone()));

    connect(ui->switchable_label, SIGNAL(pianoKeyPressed(int)), &m_basicSignalModule, SLOT(enablePianoTone(int)));
    connect(ui->switchable_label, SIGNAL(pianoKeyReleased()), &m_basicSignalModule, SLOT(disablePianoTone()));

    connect(ui->switchable_label, SIGNAL(pianoKeyPressed(int)), &m_fileModule, SLOT(enablePianoTone(int)));
    connect(ui->switchable_label, SIGNAL(pianoKeyReleased()), &m_fileModule, SLOT(disablePianoTone()));

    connect(ui->switchable_label, SIGNAL(pianoKeyPressed(int)), &m_samplerModule, SLOT(enablePianoTone(int)));
    connect(ui->switchable_label, SIGNAL(pianoKeyReleased()), &m_samplerModule, SLOT(disablePianoTone()));

	// Mega link
    m_engine.link(m_filterWidget.getFilter(), &m_spectrumWidget, &m_waveformWidget, &m_externalSourceModule, &m_basicSignalModule, &m_fileModule, &m_samplerModule);
    m_engine.connectSpectrum(ui->checkBox_spectrum_maxima, ui->checkBox_spectrum_piano);

    // "About" message box
    m_pushButton_about.setText("À propos");
    m_pushButton_about.raise();
    connect(&m_pushButton_about, SIGNAL(clicked(bool)), this, SLOT(aboutMessageBox()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::midiInputEvent(MidiEngine::MidiSoftwareInput input, int value)
{
    switch (input) {
    case MidiEngine::MidiNextSource:
        if (value > 0)
            ui->sourceSelector->nextSource();
        return true;
    case MidiEngine::MidiPreviousSource:
        if (value > 0)
            ui->sourceSelector->previousSource();
        return true;
    case MidiEngine::MidiPianoToggle:
        ui->checkBox_spectrum_piano->toggle();
        return true;
    case MidiEngine::MidiPianoCheckbox:
        ui->checkBox_spectrum_piano->setChecked(value > 0);
        return true;
    case MidiEngine::MidiMaximaToggle:
        ui->checkBox_spectrum_maxima->toggle();
        return true;
    case MidiEngine::MidiMaximaCheckbox:
        ui->checkBox_spectrum_maxima->setChecked(value > 0);
        return true;
    case MidiEngine::MidiSpectrumAutoscaleToggle:
        ui->checkBox_spectrum_autoscale->toggle();
        return true;
    case MidiEngine::MidiSpectrumAutoscaleCheckbox:
        ui->checkBox_spectrum_autoscale->setChecked(value > 0);
        return true;
    case MidiEngine::MidiSpectrumZoomPlus:
        m_spectrumWidget.zoomIn();
        return true;
    case MidiEngine::MidiSpectrumZoomMinus:
        m_spectrumWidget.zoomOut();
        return true;
    case MidiEngine::MidiAutoAdjustWaveform:
        ui->pushButton_visualization_autoAdjust->click();
        return true;
    case MidiEngine::MidiWaveformPeriodSlider:
        ui->slider_visualization_period->setLeftCursor(100 * value / 127);
        return true;
    case MidiEngine::MidiWaveformSynchroToggle:
        ui->checkBox_visualization_synchro->toggle();
        return true;
    case MidiEngine::MidiWaveformSynchroSwitch:
        ui->checkBox_visualization_synchro->setChecked(value > 0);
        return true;
    case MidiEngine::MidiPreviousFilter:
        if (value > 0) {
            if (ui->radioButton_desactive->isChecked())
                ui->radioButton_coupebande->click();
            else if (ui->radioButton_passebas->isChecked())
                ui->radioButton_desactive->click();
            else if (ui->radioButton_passehaut->isChecked())
                ui->radioButton_passebas->click();
            else if (ui->radioButton_passebande->isChecked())
                ui->radioButton_passehaut->click();
            else
                ui->radioButton_passebande->click();
        }
        return true;
    case MidiEngine::MidiNextFilter:
        if (value > 0) {
            if (ui->radioButton_desactive->isChecked())
                ui->radioButton_passebas->click();
            else if (ui->radioButton_passebas->isChecked())
                ui->radioButton_passehaut->click();
            else if (ui->radioButton_passehaut->isChecked())
                ui->radioButton_passebande->click();
            else if (ui->radioButton_passebande->isChecked())
                ui->radioButton_coupebande->click();
            else
                ui->radioButton_desactive->click();
        }
        return true;
    case MidiEngine::MidiFilterAmplitudeSlider:
        ui->dial_filter_amplitude->setValue(12 * value / 127);
        return true;
    case MidiEngine::MidiFilterFrequencySlider1:
        ui->doubleSlider->setLeftCursor(100 * value / 127);
        return true;
    case MidiEngine::MidiFilterFrequencySlider2:
        ui->doubleSlider->setRightCursor(100 * value / 127);
        return true;
    case MidiEngine::MidiPianoOn:
        if (value < 0 || value >= FrequenciesManager::getPianoFrequencies().size())
            return true;
        {
            const int freq = (int) FrequenciesManager::getPianoFrequencies().at(value);
            m_basicSignalModule.enablePianoTone(freq);
            m_externalSourceModule.enablePianoTone(freq);
            m_fileModule.enablePianoTone(freq);
            m_samplerModule.enablePianoTone(freq);
        }
        return true;
    case MidiEngine::MidiPianoOff:
        m_basicSignalModule.disablePianoTone();
        m_externalSourceModule.disablePianoTone();
        m_fileModule.disablePianoTone();
        m_samplerModule.disablePianoTone();
        return true;
    default:
        if (m_engine.midiInputEvent(input, value))
            return true;

        switch (ui->sourceSelector->currentIndex()) {
        case 0:
            return m_basicSignalModule.midiInputEvent(input, value);
        case 1:
            return m_externalSourceModule.midiInputEvent(input, value);
        case 2:
            return m_fileModule.midiInputEvent(input, value);
        default:
            return m_samplerModule.midiInputEvent(input, value);
        }
    }
}

void MainWindow::aboutMessageBox(){
    QMessageBox::about(NULL, "A propos", "<center>"
                                             "<p>"
                                                "<img src=\":/la_diagonale.png\">"
                                             "</p>"
                                             "<p>"
                                                "<img src=\":/centralesupelec.png\">"
                                             "</p>"
                                             "<p>Logiciel réalisé dans le cadre d'un projet CentraleSupélec, à la demande de l'Université Paris-Saclay.</p>"
                                             "<p>"
                                                 "<b>Bibliothèques utilisées :</b>"
                                                 "<br>Qt 5.7.0, sous licence GNU LGPL"
                                                 "<br>FFTReal, sous licence GNU LGPL"
                                                 "<br>RTAudio, sous licence libre"
                                                 "<br>RTMidi, sous licence libre"
                                                 "<br>LibSNFFile, sous licence GNU LGPL"
                                             "</p>"
                                             "<p>"
                                                 "<b>Ressources :</b>"
                                                 "<br>Icônes de Anton Saputro (CC BY 3.0)"
                                                 "<br>Boutons de base de l'interface de Osmic d'opengameart.com (CC BY 3.0)"
                                                 "<br>Icônes du sampler de Daily Overview et Eugen Buzuk sous licence libre"
                                                 "<br>Images des sources de signal par Alessandro Rei et Sergio Sanchez Lopez"
                                             "</p>"
                                             "<p>"
                                                 "<b>Développeurs :</b>"
                                                 "<br>Version 1.0 :"
                                                 "<br>Guillaume LAPORTE"
                                                 "<br>Robin LANGE"
                                                 "<br><br>Version 2.0 :"
                                                 "<br>Rafaël Lévêque"
                                                 "<br>Robin Fourcade"
                                                 "<br><br>Version 3.0 :"
                                                 "<br>Arthur Belleville"
                                                 "<br>Freddy Phung"
                                             "</p>"
                                             "<p>Sous la supervision de M. Philippe BENABES</p>"
                                         "</center>");
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    m_pushButton_about.move(event->size().width() - m_pushButton_about.size().width() - 12, -5);

    QMainWindow::resizeEvent(event);
}
