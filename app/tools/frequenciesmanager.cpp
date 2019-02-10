#include "frequenciesmanager.h"
#include "../constants.h"
#include <QtMath>
#include <QDebug>

QVector<float> FrequenciesManager::getPianoFrequencies() {
    static QVector<float> result;

    if (!result.isEmpty())
        return result;

    const float step = 1.059463094359295264561825294f;

    for (int i = 0 ; i < SPECTRUM_NUMBER_OF_OCTAVES ; ++i) {
        const float x = NOTES_START_FREQUENCY * qPow(2, i);

        // La
        if (i != 0)
            result.push_back(x);
        // Si
        if (i != 0)
            result.push_back(x * step);

        // Demi-ton
        result.push_back(x * step * step);
        // Do
        result.push_back(x * step * step * step);
        // Demi-ton
        result.push_back(x * step * step * step * step);
        // Ré
        result.push_back(x * step * step * step * step * step);
        // Demi-ton
        result.push_back(x * step * step * step * step * step * step );
        // Mi
        result.push_back(x * step * step * step * step * step * step * step);
        // Fa
        result.push_back(x * step * step * step * step * step * step * step * step);
        // Demi-ton
        result.push_back(x * step * step * step * step * step * step * step * step * step);
        // Sol
        result.push_back(x * step * step * step * step * step * step * step * step * step * step);
        // Demi-ton
        result.push_back(x * step * step * step * step * step * step * step * step * step * step * step);
    }

    return result;
}

QVector<float> FrequenciesManager::getPianoBounds() {
    static QVector<float> result;

    if (!result.isEmpty())
        return result;

    const QVector<float> frequencies = getPianoFrequencies();
    const float halfTone = 1.059463094359295264561825294f;

    result.push_back(frequencies.first() / qSqrt(halfTone));

    for (int i = 0 ; i < frequencies.size() - 1 ; ++i) {
        result.push_back(qSqrt(frequencies[i]*frequencies[i+1]));
    }

    result.push_back(frequencies.last() * qSqrt(halfTone));

    return result;
}

int FrequenciesManager::getIndexInPianoFrequenciesFromFreq(float f) {
    const QVector<float> frequencies = getPianoBounds();

    unsigned int lower = 0;
    unsigned int upper = frequencies.size() - 1;

    if (f > frequencies[upper])
        return -1;

    while (upper - lower > 1) {
        const unsigned int test = (lower + upper) / 2;

        if (frequencies[test] > f)
            upper = test;
        else
            lower = test;
    }

    if (frequencies[lower] > f)
        return upper;
    else
        return lower;
}

int FrequenciesManager::getIndexInLinearFrequenciesFromFreq(float f) {
    if (f > 6723.f)
        return -1;

    const int result = SPECTRUM_NUMBER_OF_BARS * (f - 123.f) / 6600.f;

    return qBound(0, result, SPECTRUM_NUMBER_OF_BARS - 1);
}

QVector<double> FrequenciesManager::getPianoWidths() {
    static QVector<double> result;

    if (!result.isEmpty()) {
        return result;
    } else {
        const double widths[] = {0.017482517, 0.017482517, 0.006993007, 0.017482517, 0.015734266, 0.015734266, 0.017482517, 0.01048951, 0.015734266, 0.01048951, 0.013986014, 0.015734266, 0.017482517, 0.017482517, 0.006993007, 0.017482517, 0.015734266, 0.015734266, 0.017482517, 0.01048951, 0.015734266, 0.01048951, 0.013986014, 0.015734266, 0.017482517, 0.017482517, 0.006993007, 0.017482517, 0.015734266, 0.015734266, 0.017482517, 0.01048951, 0.015734266, 0.01048951, 0.013986014, 0.015734266, 0.017482517, 0.017482517, 0.006993007, 0.017482517, 0.015734266, 0.015734266, 0.017482517, 0.01048951, 0.015734266, 0.01048951, 0.013986014, 0.015734266, 0.017482517, 0.017482517, 0.006993007, 0.017482517, 0.015734266, 0.015734266, 0.017482517, 0.01048951, 0.015734266, 0.01048951, 0.013986014, 0.015734266, 0.017482517, 0.017482517, 0.006993007, 0.017482517, 0.015734266, 0.017482517, 0.013986014, 0.019230769};
        for (int i = 0 ; i < 68 ; ++i)
            result.push_back(widths[i]);

        return result;
    }
}

int FrequenciesManager::getIndexInPianoFrequenciesFromPosition(double x, double y) {
    static QVector<double> pianoPositions;

    if (pianoPositions.isEmpty()) {
        const QVector<double> widths = FrequenciesManager::getPianoWidths();

        pianoPositions.push_back(0);
        for (int i = 1 ; i < widths.size() ; ++i)
            pianoPositions.push_back(pianoPositions[i-1] + widths[i-1]);
    }

    unsigned int lower = 0, upper = 67;
    while (upper - lower > 1) {
        const unsigned int test = (lower + upper) / 2;
        if (pianoPositions[test] > x)
            upper = test;
        else
            lower = test;
    }

    int index = pianoPositions[lower] > x ? upper + 1 : lower + 1;
    if (y > 5.0 / 9.0) {
        switch ((index - 2) % 12) {
        case 0:
        case 2:
        case 5:
        case 7:
        case 9:
            if (x - pianoPositions[index - 1] > 0.017482517 / 2.0)
                index++;
            else
                index--;
        default:
            break;
        }
    }

    return index;
}

QPointF FrequenciesManager::getPositionInPianoFromIndex(int index) {
    QPointF result;

    static QVector<double> pianoPositions;

    if (pianoPositions.isEmpty()) {
        const QVector<double> widths = FrequenciesManager::getPianoWidths();

        pianoPositions.push_back(0);
        for (int i = 1 ; i < widths.size() ; ++i)
            pianoPositions.push_back(pianoPositions[i-1] + widths[i-1]);
    }

    result.rx() = pianoPositions[index - 1];

    switch ((index - 2) % 12) {
    case 0:
    case 2:
    case 5:
    case 7:
    case 9:
        result.ry() = 0.40f;
        break;
    case 6:
    case 8:
        result.rx() -= 0.0026f;
        result.ry() = 0.84f;
        break;
    case 1:
    case 3:
    case 10:
        result.rx() -= 0.003f;
        result.ry() = 0.84f;
        break;
    default:
        result.rx() += 0.007f;
        result.ry() = 0.84f;
        break;
    }

    return result;
}
