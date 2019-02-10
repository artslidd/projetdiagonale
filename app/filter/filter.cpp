#include "filter.h"

#include <QDebug>
#include <QtMath>
#include "../constants.h"

Filter::Filter() : m_applyAttenuation(false)
{
    reset();
}

void Filter::reset() {
    m_type = Filter::DESACTIVE;
    m_amplitude = 12.0f;
    m_f1 = 1.25f * NOTES_START_FREQUENCY * qPow(128.f / 1.25f / 1.25f, 25.0f / 100.0f);
    m_f2 = 1.25f * NOTES_START_FREQUENCY * qPow(128.f / 1.25f / 1.25f, 75.0f / 100.0f);

    update();
}

void Filter::applyBiquad(float *in, float *out, unsigned int nBufferFrames, int numBiquad){

    QVector<float> coeffs, a1, a2;
    QVector<float> &lastIn = a1, &lastOut = a2;

    if(numBiquad == 1){
        coeffs = m_coeffsManager.getBiquad1(m_type >= 1 ? m_type - 1 : 0, m_amplitude, m_f1, m_f2, m_applyAttenuation);
        lastIn = m_lastBuffer1;
        lastOut = m_lastValues1;
    }
    else{
        coeffs = m_coeffsManager.getBiquad2(m_type >= 1 ? m_type - 1 : 0, m_amplitude, m_f1, m_f2, m_applyAttenuation);
        lastIn = m_lastBuffer2;
        lastOut = m_lastValues2;
    }

    QVector<float> xCoeffs = coeffs.mid(0, 3);
    QVector<float> yCoeffs = coeffs.mid(4, 2);

    for(int i = 0 ; i < yCoeffs.size() ; ++i){
        yCoeffs[i] = -yCoeffs[i];
    }

    for (int i = 0 ; i < qMax(xCoeffs.size() - 1, yCoeffs.size()) ; ++i) {
        out[i] = 0.0f;

        for (int iX = 0 ; iX < xCoeffs.size() ; ++iX)
            out[i] += xCoeffs[iX] * ((i - iX >= 0) ? in[i - iX] : lastIn[iX - i - 1]);

        for (int iY = 1 ; iY <= yCoeffs.size() ; ++iY)
            out[i] += yCoeffs[iY - 1] * ((i - iY >= 0) ? out[i - iY] : lastOut[iY - i - 1]);
    }

    for (unsigned int i = qMax(xCoeffs.size() - 1, yCoeffs.size()) ; i < nBufferFrames ; ++i) {
        out[i] = 0.0f;

        for (int iX = 0 ; iX < xCoeffs.size() ; ++iX)
            out[i] += xCoeffs[iX] * in[i - iX];

        for (int iY = 1 ; iY <= yCoeffs.size() ; ++iY)
            out[i] += yCoeffs[iY - 1] * out[i - iY];

        out[i] = qBound(-1.0f, out[i], 1.0f);
    }
}

void Filter::applyOnBuffer(float *in, float *out, unsigned int nBufferFrames) {

    if(m_type == DESACTIVE){
        memcpy(out, in, nBufferFrames*sizeof(float));

        // Atténuation
        if(m_applyAttenuation){
            for(unsigned int i = 0 ; i < nBufferFrames ; i++)
                out[i] *= 0.5;//7071;
        }

    }else{

        float *temp = new float[nBufferFrames];
        applyBiquad(in, temp, nBufferFrames, 1);

        for (int i = 0 ; i < 3 - 1 ; ++i)
            m_lastBuffer1[i] = in[nBufferFrames - i - 1];

        for (int i = 0 ; i < 2 ; ++i)
            m_lastValues1[i] = temp[nBufferFrames - i - 1];

        applyBiquad(temp, out, nBufferFrames, 2);

        // Sauvegarde des dernières valeurs
        for (int i = 0 ; i < 3 - 1 ; ++i)
            m_lastBuffer2[i] = temp[nBufferFrames - i - 1];

        for (int i = 0 ; i < 2 ; ++i)
            m_lastValues2[i] = out[nBufferFrames - i - 1];

        delete temp;
    }
}

void Filter::setAmplitude(int amplitude) {
    m_amplitude = amplitude;

    update();
}

void Filter::setFrequencySingle(float f) {
    m_f1 = f;

    update();
}

void Filter::setFrequencyDouble(float f1, float f2) {
    m_f1 = f1;
    m_f2 = f2;

    update();
}

void Filter::update() {

    m_lastBuffer1.resize(3 - 1);
    m_lastValues1.resize(2);

    m_lastBuffer2.resize(3 - 1);
    m_lastValues2.resize(2);
}

int Filter::getNumberOfBars() {
    if (m_type == Filter::PASSE_BAS || m_type == Filter::PASSE_HAUT)
        return 2;
    else if(m_type == Filter::DESACTIVE)
        return 1;

    return 3;
}

float Filter::getBarLength(int i) {
    switch (m_type) {
    case Filter::DESACTIVE:
        return 100;
        break;
    case Filter::PASSE_BAS:
    case Filter::PASSE_HAUT:
        return (i == 0) ? qLn(m_f1/89.0f) * 100.f / qLn(9300.0f/89.0f) : 100 - qLn(m_f1/89.0f) * 100.f / qLn(9300.0f/89.0f);
        break;
    case Filter::PASSE_BANDE:
    case Filter::COUPE_BANDE:
        if (i == 0)
            return qLn(m_f1/89.0f) * 100.f / qLn(9300.0f/89.0f);
        else if (i == 1)
            return qLn(m_f2 / m_f1) * 100.f / qLn(9300.0f/89.0f);
        else
            return 100.f - qLn(m_f1/89.0f) * 100.f / qLn(9300.0f/89.0f) - qLn(m_f2 / m_f1) * 100.f / qLn(9300.0f/89.0f);
        break;
    default:
        Q_ASSERT(false);
        break;
    }

    return 0.0f;
}

int Filter::getBarValue(int i) {
    switch (m_type) {
    case Filter::DESACTIVE:
        return 100;
        break;
    case Filter::PASSE_BAS:
        return (i == 0) ? 100 : 100 * m_amplitude / 12;
        break;
    case Filter::PASSE_HAUT:
        return (i == 1) ? 100 : 100 * m_amplitude / 12;
        break;
    case Filter::PASSE_BANDE:
        return (i == 1) ? 100 : 100 * m_amplitude / 12;
        break;
    case Filter::COUPE_BANDE:
        return (i == 1) ? 100 * m_amplitude / 12: 100;
        break;
    default:
        Q_ASSERT(false);
        break;
    }

    return 0.0f;
}

void Filter::setType(int type) {

    if(m_type != type) {
        m_type = type;
        update();
    }

}

void Filter::setAttenuation(bool attenuation){
    m_applyAttenuation = attenuation;
}
