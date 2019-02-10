#include "engine.h"

#include <QBoxLayout>

Engine::Engine(QObject *parent) : QObject(parent), m_currentBuffer(FREQUENCE_ECHANTILLONNAGE * WAVEFORM_MAX_PERIOD * 2), m_currentModule(NULL), m_externalSourceModule(NULL), m_basicSignalModule(NULL), m_fileModule(NULL)
{

}

Engine::~Engine() {
    m_analyser.stopAnalyser();
    m_analyser.wait(500);
}


void Engine::link(Filter *filter, SpectrumWidget * spectrum, WaveformWidget * waveform, ExternalSourceModule *externalSourceModule, BasicSignalModule *basicSignalModule, FileModule *fileModule, SamplerModule *samplerModule) {
    waveform->setBuffer(&m_currentBuffer);

    connect(&m_analyser, SIGNAL(calculationComplete(QVector<float>)), spectrum, SLOT(spectrumChanged(QVector<float>)));

    m_analyser.setBuffer(&m_currentBuffer);
    m_analyser.start();

    m_externalSourceModule = externalSourceModule;
    m_basicSignalModule = basicSignalModule;
    m_fileModule = fileModule;
    m_samplerModule = samplerModule;

    connect(m_externalSourceModule, SIGNAL(activationRequest(EngineModule*)), this, SLOT(moduleActivated(EngineModule*)));
    connect(m_basicSignalModule, SIGNAL(activationRequest(EngineModule*)), this, SLOT(moduleActivated(EngineModule*)));
    connect(m_fileModule, SIGNAL(activationRequest(EngineModule*)), this, SLOT(moduleActivated(EngineModule*)));
    connect(m_samplerModule, SIGNAL(activationRequest(EngineModule*)), this, SLOT(moduleActivated(EngineModule*)));

    m_externalSourceModule->link(&m_currentBuffer, filter, &m_audio);
    m_basicSignalModule->link(&m_currentBuffer, filter, &m_audio);
    m_fileModule->link(&m_currentBuffer, filter, &m_audio);
    m_samplerModule->link(&m_currentBuffer, filter, &m_audio);
}

void Engine::moduleActivated(EngineModule *module)
{
    if(m_currentModule != NULL)
        m_currentModule->deactivate();

    module->activateAndPlay(); // TODO : une valeur de retour à checker ?
    m_currentModule = module;
}

void Engine::connectSpectrum(QCheckBox *checkBox_maximaDetection, QCheckBox *checkBox_piano){
    connect(checkBox_maximaDetection, SIGNAL(toggled(bool)), &m_analyser, SLOT(setMaximaDetection(bool)));
    connect(checkBox_piano, SIGNAL(toggled(bool)), &m_analyser, SLOT(setPiano(bool)));
}

bool Engine::midiInputEvent(MidiEngine::MidiSoftwareInput input, int value) {
    switch (input) {
    case MidiEngine::MidiPause:
        if (value > 0 && m_currentModule != NULL)
            m_currentModule->deactivate();
        return true;
    default:
        return false;
    }
}
