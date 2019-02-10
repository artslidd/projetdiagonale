#include <QtMath>
#include "constants.h"
#include "externalsourcemodule.h"
#include "ui_externalsourcemodule.h"
#include "../midi/midiengine.h"

int inoutExternal(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void *streamData)
{
    Q_UNUSED(streamTime);
    Q_UNUSED(status);

    float * beforeFilterBuffer = NULL;
    if (beforeFilterBuffer  == NULL) beforeFilterBuffer = new float[nBufferFrames]; // ...Here

    float * outputBufferFloat = (float *) outputBuffer;
    float * inputBufferFloat  = (float *) inputBuffer;

    ExternalSourceModule::StreamData & data = *((ExternalSourceModule::StreamData*)streamData);

    if (data.multiplier == 0)
        memcpy(beforeFilterBuffer, inputBuffer, nBufferFrames * sizeof(float));
    else for (unsigned int i = 0 ; i < nBufferFrames ; ++i)
        beforeFilterBuffer[i] = inputBufferFloat[i] * qPow(2, data.multiplier);

    if(data.pianoTone)
        EngineModule::addPianoTone(beforeFilterBuffer, nBufferFrames, data.pianoFreq);

    data.filter->applyOnBuffer((float *) beforeFilterBuffer, (float *) outputBuffer, nBufferFrames);
    data.ringBuffer->addBuffer((float *) outputBuffer, nBufferFrames);

    memcpy((void*) &outputBufferFloat[nBufferFrames], outputBuffer, nBufferFrames * sizeof(float));
    return 0;
}

ExternalSourceModule::ExternalSourceModule(QWidget *parent) : EngineModule(parent), ui(new Ui::ExternalSourceModule)
{
    ui->setupUi(this);

    m_streamData.ringBuffer = NULL;
    m_streamData.multiplier = 1;

    m_streamData.pianoTone = false;
    m_streamData.pianoFreq = 0.0f;

    connect(ui->pushButton_controls_play_microphone_2, SIGNAL(clicked(bool)), this, SLOT(playButtonPressed()));
    connect(ui->pushButton_controls_pause_microphone_2, SIGNAL(clicked(bool)), this, SLOT(pauseButtonPressed()));
    connect(ui->pushButton_microphone_refreshList_2, SIGNAL(clicked(bool)), this, SLOT(refreshButtonPressed()));
    connect(ui->comboBox_microphone, SIGNAL(currentTextChanged(QString)), this, SLOT(sourceChoiceChanged()));
    connect(ui->slider_microphone_volume_2, SIGNAL(valueChanged(int)), this, SLOT(multiplierSliderChanged(int)));
}

ExternalSourceModule::~ExternalSourceModule()
{
    delete ui;
}

void ExternalSourceModule::link(RingBuffer *ringBuffer, Filter * filter, RtAudio *audio)
{
    m_streamData.ringBuffer = ringBuffer;
    m_streamData.filter = filter;
    m_audio = audio;

    ui->comboBox_microphone->addItems(getAvailableInputs());
    m_currMic = ui->comboBox_microphone->currentText();
}

void ExternalSourceModule::openSource()
{
    unsigned int devices = m_audio->getDeviceCount();
    int iDevice = -1;
    int oDevice = -1;

    RtAudio::DeviceInfo info;
    for (unsigned int i = 0 ; i < devices ; i++) {
        info = m_audio->getDeviceInfo(i);
        if (info.probed == true) {
            if (info.name == m_currMic.toStdString())
                iDevice = i;
            else if (info.isDefaultOutput)
                oDevice = i;
        }
    }

    if (iDevice < 0 || oDevice < 0)
        return;

    // Paramétrage
    m_bufferFrames = 128;
    RtAudio::StreamParameters iParams, oParams;
    iParams.deviceId = iDevice;
    iParams.nChannels = 1;
    oParams.deviceId = oDevice;
    oParams.nChannels = 2;

    RtAudio::StreamOptions options;
    options.flags = RTAUDIO_NONINTERLEAVED;

    try {
        int type = (sizeof(float) == 4) ? RTAUDIO_FLOAT32 : RTAUDIO_FLOAT64;
        m_audio->openStream(&oParams, &iParams, type, FREQUENCE_ECHANTILLONNAGE, &m_bufferFrames, &inoutExternal, (void *)&m_streamData, &options);
    } catch (RtAudioError& e) {
        std::cout << "Error : " << e.getMessage() << std::endl;
        return;
    }
}

void ExternalSourceModule::activateAndPlay()
{
    m_activated = true;

    openSource();

    if(m_audio->isStreamOpen())
        m_audio->startStream();
    else
        return;

    if(m_audio->isStreamRunning())
    {
        ui->pushButton_controls_play_microphone_2->setEnabled(false);
        ui->pushButton_controls_pause_microphone_2->setEnabled(true);
    }

}

void ExternalSourceModule::deactivate()
{
    if (m_audio->isStreamRunning())
        m_audio->stopStream();

     if (m_audio->isStreamOpen())
         m_audio->closeStream();

     ui->pushButton_controls_play_microphone_2->setEnabled(true);
     ui->pushButton_controls_pause_microphone_2->setEnabled(false);

     m_activated = false;
}

void ExternalSourceModule::enablePianoTone(int freq){

    m_streamData.pianoTone = true;
    m_streamData.pianoFreq = freq;

}

void ExternalSourceModule::disablePianoTone(){

    m_streamData.pianoTone = false;
    m_streamData.pianoFreq = 0.0f;

}

void ExternalSourceModule::playButtonPressed()
{
    if(m_activated)
    {
        if(m_audio->isStreamOpen())
        {
            if(!m_audio->isStreamRunning())
                m_audio->startStream();
            ui->pushButton_controls_play_microphone_2->setEnabled(false);
            ui->pushButton_controls_pause_microphone_2->setEnabled(true);
        }
        else
            activateAndPlay();
    }
    else
        emit activationRequest(this);
}

void ExternalSourceModule::pauseButtonPressed()
{
    if (m_audio->isStreamRunning())
        m_audio->stopStream();

    ui->pushButton_controls_play_microphone_2->setEnabled(true);
    ui->pushButton_controls_pause_microphone_2->setEnabled(false);
}

void ExternalSourceModule::refreshButtonPressed()
{
    ui->comboBox_microphone->clear();
    ui->comboBox_microphone->addItems(getAvailableInputs());
}

void ExternalSourceModule::sourceChoiceChanged()
{
    if(ui->comboBox_microphone->currentText() != m_currMic)
    {
        m_currMic = ui->comboBox_microphone->currentText();
        if(m_activated)
        {
            if(m_audio->isStreamOpen())
            {
                if (m_audio->isStreamRunning())
                    m_audio->stopStream();
                m_audio->abortStream();
            }


            ui->pushButton_controls_play_microphone_2->setEnabled(true);
            ui->pushButton_controls_pause_microphone_2->setEnabled(false);

            openSource();
        }
    }
}

void ExternalSourceModule::multiplierSliderChanged(int value)
{
   m_streamData.multiplier = value;
}

QStringList ExternalSourceModule::getAvailableInputs()
{
    QStringList result;

    unsigned int devices = m_audio->getDeviceCount();

    RtAudio::DeviceInfo info;
    for (unsigned int i = 0 ; i < devices ; i++) {

        info = m_audio->getDeviceInfo(i);
        if (info.probed == true &&  info.inputChannels != 0) {
            if (info.isDefaultInput)
                result.push_front(QString(info.name.c_str()));
            else
                result.push_back(QString(info.name.c_str()));
        }

    }

    return result;
}

void ExternalSourceModule::pause() {
    pauseButtonPressed();
}

bool ExternalSourceModule::midiInputEvent(MidiEngine::MidiSoftwareInput input, int value)
{
    switch (input) {
    case MidiEngine::MidiPlay:
        if (m_activated)
            playButtonPressed();
        else
            emit activationRequest(this);

        return true;
    case MidiEngine::MidiSourceSlider1:
        ui->slider_microphone_volume_2->setValue(3 * value / 127);
    case MidiEngine::MidiSourceSlider2:
        return true;
    default:
        return false;
    }
}
