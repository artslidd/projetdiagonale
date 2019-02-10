#ifndef ENHANCEDFFTLWRAPPER_H
#define ENHANCEDFFTLWRAPPER_H

#include "fftreal_wrapper.h"

class FFTREAL_EXPORT EnhancedFFTWrapper : public FFTRealWrapper
{
public:

    EnhancedFFTWrapper();
    ~EnhancedFFTWrapper();

    unsigned int getInputLength();
    unsigned int getOutputLength();

    void calculateFFT(const DataType in[], DataType out[]);

private:

    DataType * m_dataReal;
    DataType * m_dataImag;

};

#endif // ENHANCEDFFTLWRAPPER_H
