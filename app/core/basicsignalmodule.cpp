#include <QtMath>
#include "constants.h"
#include "basicsignalmodule.h"
#include "ui_basicsignalmodule.h"
#include "../tools/frequenciesmanager.h"
#include "../midi/midiengine.h"

int inoutSignal(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void *streamData)
{
    Q_UNUSED(streamTime);
    Q_UNUSED(status);
    Q_UNUSED(inputBuffer);

    BasicSignalModule::StreamData & data = *((BasicSignalModule::StreamData*)streamData);

    float * beforeFilterBuffer = NULL;
    if (beforeFilterBuffer  == NULL) beforeFilterBuffer = new float[nBufferFrames]; // ...Here

    if (data.on) {
        static quint64 time = 1;
        static int oldFrequency = data.basicSignalFrequency;

        if (oldFrequency != data.basicSignalFrequency) {
            time += time * (oldFrequency - (data.basicSignalFrequency)) / (data.basicSignalFrequency);
            oldFrequency = data.basicSignalFrequency;
        }

        for (unsigned int i = 0 ; i < nBufferFrames ; ++i) {
            const qreal subTime = ((qreal) ((++time) * (data.basicSignalFrequency) % FREQUENCE_ECHANTILLONNAGE)) / FREQUENCE_ECHANTILLONNAGE;

            if (data.basicSignalMorphingSinus == 0.0) {
                const qreal square = subTime < 0.5 ? -1.0 : 1.0;
                const qreal triangle = subTime < 0.5 ? 4.0 * subTime - 1.0 : 3.0 - 4.0 * subTime;

                beforeFilterBuffer[i] = 0.30f * ((data.basicSignalMorphingSquare) * square + (1.0f - (data.basicSignalMorphingSquare)) * triangle);
            } else {
                const qreal sinus = qSin(2 * 3.14159265359 * subTime);
                const qreal triangle = subTime < 0.5 ? 4.0 * subTime - 1.0 : 3.0 - 4.0 * subTime;

                beforeFilterBuffer[i] = 0.30f * ((data.basicSignalMorphingSinus) * sinus + (1.0f - (data.basicSignalMorphingSinus)) * triangle);
            }
        }
    } else {
        for (unsigned int i = 0 ; i < nBufferFrames ; ++i)
            beforeFilterBuffer[i] = 0.0f;
    }

    if(data.pianoTone)
        EngineModule::addPianoTone(beforeFilterBuffer, nBufferFrames, data.pianoFreq);

    data.filter->applyOnBuffer((float *) beforeFilterBuffer, (float *) outputBuffer, nBufferFrames);
    data.ringBuffer->addBuffer((float *) outputBuffer, nBufferFrames);

    memcpy((void*) &((float *) outputBuffer)[nBufferFrames], outputBuffer, nBufferFrames * sizeof(float));
    return 0;
}

BasicSignalModule::BasicSignalModule(QWidget *parent) : EngineModule(parent), ui(new Ui::BasicSignalModule), m_basicSignalFrequencies(SPECTRUM_NUMBER_OF_BARS)
{
    ui->setupUi(this);

    // Fréquences
    m_basicSignalFrequencies = FrequenciesManager::getPianoFrequencies();

    // Stream data
    m_streamData.ringBuffer = NULL;
    m_streamData.filter = NULL;
    m_streamData.basicSignalFrequency = m_basicSignalFrequencies[0];
    m_streamData.basicSignalMorphingSinus = 1;
    m_streamData.basicSignalMorphingSquare = 0;

    m_streamData.pianoTone = false;
    m_streamData.pianoFreq = 0.0f;

    m_streamData.on = false;

    // Widgets
    connect(ui->pushButton_controls_play_signal, SIGNAL(clicked(bool)), this, SLOT(playButtonPressed()));
    connect(ui->pushButton_controls_pause_signal, SIGNAL(clicked(bool)), this, SLOT(pauseButtonPressed()));
    connect(ui->slider_signal_frequency, SIGNAL(valueChanged(int)), this, SLOT(frequencySliderChanged(int)));
    connect(ui->slider_signal_morphing, SIGNAL(valueChanged(int)), this, SLOT(morphingSliderChanged(int)));
}

BasicSignalModule::~BasicSignalModule()
{
    delete ui;
}

void BasicSignalModule::link(RingBuffer *ringBuffer, Filter * filter, RtAudio *audio)
{
    m_streamData.ringBuffer = ringBuffer;
    m_streamData.filter = filter;
    m_audio = audio;
}

void BasicSignalModule::activateAndPlay()
{
    m_activated = true;

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
        m_audio->openStream(&oParams, NULL, type, FREQUENCE_ECHANTILLONNAGE, &m_bufferFrames, &inoutSignal, (void *)&m_streamData, &options);
    } catch (RtAudioError& e) {
        std::cout << "Error : " << e.getMessage() << std::endl;
        return;
    }

    if(m_audio->isStreamOpen())
        m_audio->startStream();
    else
        return;
}

void BasicSignalModule::deactivate()
{
    if(m_activated)
    {
        if (m_audio->isStreamRunning())
            m_audio->stopStream();

         if (m_audio->isStreamOpen())
             m_audio->closeStream();

         m_activated = false;
         m_streamData.on = false;
    }

     ui->pushButton_controls_play_signal->setEnabled(true);
     ui->pushButton_controls_pause_signal->setEnabled(false);
}



void BasicSignalModule::enablePianoTone(int freq){
    if (!m_audio->isStreamRunning())
        emit activationRequest(this);

    m_streamData.pianoTone = true;
    m_streamData.pianoFreq = freq;
}

void BasicSignalModule::disablePianoTone(){
    m_streamData.pianoTone = false;
    m_streamData.pianoFreq = 0.0f;
}

void BasicSignalModule::playButtonPressed()
{
    m_streamData.on = true;

    ui->pushButton_controls_play_signal->setEnabled(false);
    ui->pushButton_controls_pause_signal->setEnabled(true);

    if (!m_activated)
        emit activationRequest(this);
}

void BasicSignalModule::pauseButtonPressed()
{
    m_streamData.on = false;

    ui->pushButton_controls_play_signal->setEnabled(true);
    ui->pushButton_controls_pause_signal->setEnabled(false);
}

void BasicSignalModule::frequencySliderChanged(int value) {

    m_streamData.basicSignalFrequency = m_basicSignalFrequencies[value];

    const QString str = (m_streamData.basicSignalFrequency < 1000) ? QString::number(m_streamData.basicSignalFrequency) + " Hz" : QString::number(((qreal)m_streamData.basicSignalFrequency)/1000.0,'f',1) + " kHz";

    ui->label_signal_frequency->setText(str);
}

void BasicSignalModule::morphingSliderChanged(int value) {

    if (value >= 50) {
        m_streamData.basicSignalMorphingSinus = 0;
        m_streamData.basicSignalMorphingSquare = (((qreal)value) - 50.0) / 50.0;
    } else {
        m_streamData.basicSignalMorphingSquare = 0;
        m_streamData.basicSignalMorphingSinus = 1.0 - ((qreal)value) / 50.0;
    }
}

void BasicSignalModule::pause() {
    pauseButtonPressed();
}

bool BasicSignalModule::midiInputEvent(MidiEngine::MidiSoftwareInput input, int value)
{
    switch (input) {
    case MidiEngine::MidiPlay:
        if (m_activated)
            playButtonPressed();
        else
            emit activationRequest(this);

        return true;
    case MidiEngine::MidiSourceSlider1:
        ui->slider_signal_frequency->setValue(67 * value / 127 + 1);
        return true;
    case MidiEngine::MidiSourceSlider2:
        ui->slider_signal_morphing->setValue(100 * value / 127);
        return true;
    default:
        return false;
    }
}


