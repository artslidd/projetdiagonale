#include <QDebug>
#include <vector>
#include <QtMath>

#include "spectrumanalyser.h"
#include "../tools/frequenciesmanager.h"

SpectrumAnalyser::SpectrumAnalyser(QObject *parent) : QThread(parent),
    m_sourceRingBuffer(NULL),
    m_piano(false),
    m_maximaDetection(false),
    m_result(SPECTRUM_NUMBER_OF_BARS),
    m_result_maximum(SPECTRUM_NUMBER_OF_BARS),
    m_running(true)
{
    m_inputBuffer  = new float[m_fftWrapper.getInputLength ()];
    m_outputBuffer = new float[m_fftWrapper.getOutputLength()];

    const unsigned int inputLength  = m_fftWrapper.getInputLength ();
    m_window = new float[inputLength];

    for (unsigned int i = 0 ; i < inputLength; ++i) {
        const float theta = 6.28318530718f * ((float) i / (float)(inputLength - 1));
        m_window[i] = 0.85f + 0.15f * (0.54f - 0.46f * qCos(theta));
    }

    qRegisterMetaType< QVector<float> >("FloatVector");
}

SpectrumAnalyser::~SpectrumAnalyser()
{
    delete[] m_inputBuffer;
    delete[] m_outputBuffer;

    delete[] m_window;
}

void SpectrumAnalyser::setBuffer(RingBuffer *buffer) {
    m_sourceRingBuffer = buffer;
}

void SpectrumAnalyser::run() {
    const unsigned int inputLength  = m_fftWrapper.getInputLength ();
    const unsigned int outputLength = m_fftWrapper.getOutputLength();

    while (m_running) {
        // Récupération du buffer d'entrée
        m_sourceRingBuffer->willReadInSeparateProcess();
        memcpy(m_inputBuffer, m_sourceRingBuffer->rawData() + m_sourceRingBuffer->getSize() - inputLength, inputLength*sizeof(float));
        m_sourceRingBuffer->finishedReadingInSeparateProcess();

        // Fenêtre
        for (unsigned int i = 0 ; i < inputLength ; ++i) {
            m_inputBuffer[i] *= m_window[i];
        }

        // Calcul
        m_fftWrapper.calculateFFT(m_inputBuffer, m_outputBuffer);

        // Traitement
        m_result.fill(0.0f);

        if (m_maximaDetection)
            m_result_maximum.fill(false);

        unsigned int i = 0;
        while ((float) i * 22050.0f / (float) (outputLength - 1) < NOTES_START_FREQUENCY) {
            i++;
        }

        if (m_piano) {
            if (!m_maximaDetection) {
                for ( ; i < outputLength ; ++i) {
                    const float freq = (float) i * 22050.0f / (float) (outputLength - 1);
                    const int barIndex = FrequenciesManager::getIndexInPianoFrequenciesFromFreq(freq);

                    if (barIndex < 0)
                        break;

                    m_result[barIndex] += m_outputBuffer[i];
                }
            } else {
                for ( ; i < outputLength ; ++i) {
                    const float freq = (float) i * 22050.0f / (float) (outputLength - 1);
                    const int barIndex = FrequenciesManager::getIndexInPianoFrequenciesFromFreq(freq);

                    if (barIndex < 0)
                        break;

                    m_result[barIndex] += m_outputBuffer[i];

                    const float prev = (i == 0 ? 0 : m_outputBuffer[i-1]);
                    const float next = (i == outputLength ? 0 : m_outputBuffer[i+1]);
                    if (m_outputBuffer[i] > prev && m_outputBuffer[i] > next)
                        m_result_maximum[barIndex] = true;
                }
            }
        } else {
            if (!m_maximaDetection) {
                for ( ; i < outputLength ; ++i) {
                    const float freq = (float) i * 22050.0f / (float) (outputLength - 1);
                    const int barIndex = FrequenciesManager::getIndexInLinearFrequenciesFromFreq(freq);

                    if (barIndex < 0)
                        break;

                    m_result[barIndex] += m_outputBuffer[i];
                }
            } else {
                for ( ; i < outputLength ; ++i) {
                    const float freq = (float) i * 22050.0f / (float) (outputLength - 1);
                    const int barIndex = FrequenciesManager::getIndexInLinearFrequenciesFromFreq(freq);

                    if (barIndex < 0)
                        break;

                    m_result[barIndex] += m_outputBuffer[i];

                    const float prev = (i == 0 ? 0 : m_outputBuffer[i-1]);
                    const float next = (i == outputLength ? 0 : m_outputBuffer[i+1]);
                    if (m_outputBuffer[i] > prev && m_outputBuffer[i] > next)
                        m_result_maximum[barIndex] = true;
                }
            }
        }

        if (m_maximaDetection) {
            for (int i = 1 ; i < m_result.size() - 1 ; ++i)
                if (m_result[i] < m_result[i-1] || m_result[i] < m_result[i+1])
                    m_result_maximum[i] = false;


            for (int i = 0 ; i < m_result.size() ; ++i)
                m_result[i] = m_result_maximum[i] ? sqrtf(m_result[i]) : 0.0f;
        } else {
            for (QVector<float>::iterator it = m_result.begin() ; it != m_result.end() ; ++it)
                *it = sqrtf(*it);
        }

        emit calculationComplete(m_result);

        this->msleep(25);
    }
}

void SpectrumAnalyser::stopAnalyser() {
    m_running = false;
}

void SpectrumAnalyser::setMaximaDetection(bool maximaDetection){
    m_maximaDetection = maximaDetection;
}

void SpectrumAnalyser::setPiano(bool piano){
    m_piano = piano;
}

