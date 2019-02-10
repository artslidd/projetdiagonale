#include <QtMath>
#include "constants.h"
#include "enginemodule.h"

EngineModule::EngineModule(QWidget *parent) : QWidget(parent), m_audio(NULL), m_bufferFrames(0), m_activated(false)
{
}

void EngineModule::addPianoTone(float *buffer, unsigned int nBufferFrames, int freq){

    static quint64 time = 1;
    static int oldFrequency = freq;

    if (oldFrequency != freq) {
        time += time * (oldFrequency - freq) / freq;
        oldFrequency = freq;
    }

    for (unsigned int i = 0 ; i < nBufferFrames ; ++i) {
        const qreal subTime = ((qreal) ((++time) * freq % FREQUENCE_ECHANTILLONNAGE)) / FREQUENCE_ECHANTILLONNAGE;
        const qreal sinus = qSin(2 * 3.14159265359 * subTime);
        buffer[i] = qBound(-1.0f, (float)(buffer[i] + 0.30f * sinus), 1.0f);
    }

}
