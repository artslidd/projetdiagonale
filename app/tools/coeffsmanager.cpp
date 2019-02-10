#include <QtMath>
#include <QFile>
#include <QMessageBox>
#include "coeffsmanager.h"

#include <QDebug>

CoeffsManager::CoeffsManager()
{
    QFile lowpass(":/lowpass.dat");
    QFile highpass(":/highpass.dat");
    QFile bandpass(":/bandpass.dat");
    QFile bandstop(":/bandstop.dat");

    for(int i = 0 ; i < 4 ; i++)
        m_coeffs[i] = NULL;

    if(!lowpass.open(QIODevice::ReadOnly) ||
       !highpass.open(QIODevice::ReadOnly) ||
       !bandpass.open(QIODevice::ReadOnly) ||
       !bandstop.open(QIODevice::ReadOnly))
    {
        QMessageBox msgBox;
        msgBox.setText("Erreur critique lors du chargement des coefficients de filtre.");
        msgBox.exec();

        m_coeffsLoadingOK = false;

    }else{

        m_coeffs[LOWPASS]  = new float[NB_BIQUADS*AMP_STEPS*FREQ_STEPS*6];
        m_coeffs[HIGHPASS] = new float[NB_BIQUADS*AMP_STEPS*FREQ_STEPS*6];
        m_coeffs[BANDPASS] = new float[NB_BIQUADS*AMP_STEPS*FREQ_STEPS*(FREQ_STEPS - 1) / 2 *6];
        m_coeffs[BANDSTOP] = new float[NB_BIQUADS*AMP_STEPS*FREQ_STEPS*(FREQ_STEPS - 1) / 2 *6];

        lowpass.read((char*)m_coeffs[LOWPASS], NB_BIQUADS*AMP_STEPS*FREQ_STEPS*6 * 4);
        highpass.read((char*)m_coeffs[HIGHPASS], NB_BIQUADS*AMP_STEPS*FREQ_STEPS*6 * 4);
        bandpass.read((char*)m_coeffs[BANDPASS], NB_BIQUADS*AMP_STEPS*FREQ_STEPS*(FREQ_STEPS - 1) / 2 *6*4);
        bandstop.read((char*)m_coeffs[BANDSTOP], NB_BIQUADS*AMP_STEPS*FREQ_STEPS*(FREQ_STEPS - 1) / 2 *6*4);

        m_coeffsLoadingOK = true;
    }

    lowpass.close();
    highpass.close();
    bandpass.close();
    bandstop.close();
}

CoeffsManager::~CoeffsManager(){

    delete m_coeffs[LOWPASS];
    delete m_coeffs[HIGHPASS];
    delete m_coeffs[BANDPASS];
    delete m_coeffs[BANDSTOP];
}

QVector<float> CoeffsManager::getBiquad1(int type, unsigned int a, float f1, float f2, bool attenuation){

    QVector<float> coeffs(6);

    if(a == AMP_STEPS || !m_coeffsLoadingOK){ // Cas ou le filtre est desactive (amplitude = max ou coeffs pas chargés)
        coeffs.fill(0);
        coeffs[0] = 1.0;
        coeffs[3] = 1.0;
    }else{
        memcpy(coeffs.data(), &m_coeffs[type][getIndex(type,AMP_STEPS - a - 1,f1,f2)], 4*6);
    }

    if(attenuation){
        for (int i = 0 ; i < 3 ; i++)
            coeffs[i] *= 0.707106781;
    }

    return coeffs;
}

QVector<float> CoeffsManager::getBiquad2(int type, unsigned int a, float f1, float f2, bool attenuation){

    QVector<float> coeffs(6);

    if(a == AMP_STEPS || !m_coeffsLoadingOK){ // Cas ou le filtre est desactive (amplitude = max ou coeffs pas chargés)
        coeffs.fill(0);
        coeffs[0] = 1.0;
        coeffs[3] = 1.0;
    }else{
        memcpy(coeffs.data(), &m_coeffs[type][getIndex(type,AMP_STEPS - a - 1,f1,f2)+6], 4*6);
    }

    if(attenuation){
        for (int i = 0 ; i < 3 ; i++)
            coeffs[i] *= 0.707106781;
    }

    return coeffs;
}

unsigned int CoeffsManager::getIndex(int type, unsigned int a, float f1, float f2){

    unsigned int k_f1 = FREQ_STEPS*qLn(f1/FMIN)/qLn(FMAX/FMIN);

    if(type == BANDPASS || type == BANDSTOP){
        unsigned int k_f2 = FREQ_STEPS*qLn(f2/FMIN)/qLn(FMAX/FMIN);

        if (k_f2 <= k_f1)  {
            if (k_f1 == 17)
                k_f1 = 16;

            k_f2 = k_f1 + 1;
        }

        int res = a * FREQ_STEPS*(FREQ_STEPS-1)/2 * NB_BIQUADS * 6 + (k_f1*FREQ_STEPS - k_f1 * (k_f1 + 1) / 2)*NB_BIQUADS*6 + (k_f2 - k_f1 - 1)*NB_BIQUADS*6;

        Q_ASSERT(res >= 0 && res < NB_BIQUADS*AMP_STEPS*FREQ_STEPS*(FREQ_STEPS - 1) / 2 *6);

        return (unsigned int) res;
    }else{
        int res = a*FREQ_STEPS*NB_BIQUADS*6 + k_f1*NB_BIQUADS*6;

        Q_ASSERT(res >= 0 && res < NB_BIQUADS*AMP_STEPS*FREQ_STEPS*6);

        return (unsigned int) res;
    }

}
