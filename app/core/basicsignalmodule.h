#ifndef BASICSIGNALMODULE_H
#define BASICSIGNALMODULE_H

#include "enginemodule.h"
#include "../filter/filter.h"

int inoutSignal(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void *streamData);

namespace Ui {
class BasicSignalModule;
}

/*!
 * \brief Le module de génération de signaux de base.
 * Il s'occupe de générer des sinus, triangles et rectangles à la fréquence demandée et peut effectuer une transition "continue" entre ces signaux.
 */
class BasicSignalModule : public EngineModule
{
    Q_OBJECT

public:

    struct StreamData
    {
        float basicSignalMorphingSinus;
        float basicSignalMorphingSquare;
        int  basicSignalFrequency;
        Filter * filter;
        RingBuffer *ringBuffer;
        bool pianoTone;
        int pianoFreq;
        bool on;
    };

    explicit BasicSignalModule(QWidget *parent = 0);
    ~BasicSignalModule();
    void link(RingBuffer *ringBuffer, Filter * filter, RtAudio *audio);
    void activateAndPlay();
    void deactivate();
    void pause();

public slots:
    bool midiInputEvent(MidiEngine::MidiSoftwareInput input, int value);

    void enablePianoTone(int freq);
    void disablePianoTone();

private slots:
    void playButtonPressed();
    void pauseButtonPressed();
    void frequencySliderChanged(int value);
    void morphingSliderChanged(int value);

private:

    // UI
    Ui::BasicSignalModule *ui;

    // Fréquences
    QVector<float> m_basicSignalFrequencies;

    StreamData m_streamData;
};

#endif // BASICSIGNALMODULE_H
