/***************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as
** published by the Free Software Foundation, either version 2.1. This
** program is distributed in the hope that it will be useful, but WITHOUT
** ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
** FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
** for more details. You should have received a copy of the GNU General
** Public License along with this program. If not, see
** <http://www.gnu.org/licenses/>.
**
***************************************************************************/

#include "fftreal_wrapper.h"
#include "enhancedfft_wrapper.h"

// FFTReal code generates quite a lot of 'unused parameter' compiler warnings,
// which we suppress here in order to get a clean build output.
#if defined Q_CC_MSVC
#    pragma warning(disable:4100)
#elif defined Q_CC_GNU
#    pragma GCC diagnostic ignored "-Wunused-parameter"
#elif defined Q_CC_MWERKS
#    pragma warning off (10182)
#endif

#include "FFTRealFixLen.h"
#include <QtMath>

class FFTRealWrapperPrivate {
public:
    FFTRealFixLen<FFTLengthPowerOfTwo> m_fft;
};


FFTRealWrapper::FFTRealWrapper()
    :   m_private(new FFTRealWrapperPrivate)
{
    m_fftOutput = new DataType[(unsigned int) qPow(2,FFTLengthPowerOfTwo)];
}

FFTRealWrapper::~FFTRealWrapper()
{
    delete m_private;
    delete[] m_fftOutput;
}

unsigned int  FFTRealWrapper::getInputLength() {
    return (unsigned int) qPow(2,FFTLengthPowerOfTwo);
}

unsigned int  FFTRealWrapper::getOutputLength() {
    return ((unsigned int) qPow(2,FFTLengthPowerOfTwo)) / 2;
}

void FFTRealWrapper::calculateFFT(DataType in[], DataType out[])
{
    m_private->m_fft.do_fft(m_fftOutput, in);

    const unsigned int size = ((unsigned int) qPow(2,FFTLengthPowerOfTwo)) / 2;
    for (unsigned int i = 0 ; i < size ; ++i) {
        const float real = m_fftOutput[i];
        const float imag = m_fftOutput[i + size];

        out[i] = (real * real + imag * imag);
    }

}

EnhancedFFTWrapper::EnhancedFFTWrapper()
{
    m_dataReal = new DataType[getOutputLength()];
    m_dataImag = new DataType[getOutputLength()];
}

EnhancedFFTWrapper::~EnhancedFFTWrapper()
{
    delete[] m_dataReal;
    delete[] m_dataImag;
}

unsigned int  EnhancedFFTWrapper::getInputLength() {
    return 13 * (unsigned int) qPow(2,FFTLengthPowerOfTwo) / 4;
}

unsigned int  EnhancedFFTWrapper::getOutputLength() {
    return ((unsigned int) qPow(2,FFTLengthPowerOfTwo)) / 2;
}

void EnhancedFFTWrapper::calculateFFT(const DataType in[], DataType out[])
{
    const unsigned int size = ((unsigned int) qPow(2,FFTLengthPowerOfTwo)) / 2;

    // Première passe
    m_private->m_fft.do_fft(m_fftOutput, in);

    for (unsigned int i = 0 ; i < size ; ++i) {
        m_dataReal[i] = m_fftOutput[i];
        m_dataImag[i] = m_fftOutput[i + size];
    }

    // Deuxième passe
    m_private->m_fft.do_fft(m_fftOutput, &(in[3 * (unsigned int) qPow(2,FFTLengthPowerOfTwo) / 4]));

    for (unsigned int i = 0 ; i < size ; ++i) {
        m_dataReal[i] += m_fftOutput[i];
        m_dataImag[i] += m_fftOutput[i + size];
    }

    // Troisième passe
    //*
    m_private->m_fft.do_fft(m_fftOutput, &(in[6 * (unsigned int) qPow(2,FFTLengthPowerOfTwo) / 4]));

    for (unsigned int i = 0 ; i < size ; ++i) {
        m_dataReal[i] += m_fftOutput[i];
        m_dataImag[i] += m_fftOutput[i + size];
    }
    //*/

    // Quatrième passe
    m_private->m_fft.do_fft(m_fftOutput, &(in[9 * (unsigned int) qPow(2,FFTLengthPowerOfTwo) / 4]));

    for (unsigned int i = 0 ; i < size ; ++i) {
        m_dataReal[i] += m_fftOutput[i];
        m_dataImag[i] += m_fftOutput[i + size];

        out[i] = m_dataReal[i] * m_dataReal[i] + m_dataImag[i] * m_dataImag[i];
    }
}
