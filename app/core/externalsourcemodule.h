#ifndef EXTERNALSOURCEMODULE_H
#define EXTERNALSOURCEMODULE_H

#include <QPushButton>
#include "enginemodule.h"
#include "../filter/filter.h"

int inoutExternal(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void *streamData);

namespace Ui {
class ExternalSourceModule;
}

/*!
 * \brief Le module de lecture depuis une source externe.
 * Il se contente de lister les entrées audio disponibles et propose d'appliquer un gain sur celles-ci (utile pour certains microphones
 * intégrés très faibles.
 */
class ExternalSourceModule : public EngineModule
{

    Q_OBJECT

public:

    struct StreamData
    {
        qint8 multiplier;
        Filter * filter;
        RingBuffer *ringBuffer;
        bool pianoTone;
        int pianoFreq;
    };

    ExternalSourceModule(QWidget *parent = 0);
    ~ExternalSourceModule();
    void link(RingBuffer *ringBuffer, Filter *filter, RtAudio *audio);
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
    void refreshButtonPressed();
    void sourceChoiceChanged();
    void multiplierSliderChanged(int value);

private :
    QStringList getAvailableInputs();
    void openSource();

    StreamData m_streamData;
    QString m_currMic;

    // UI
    Ui::ExternalSourceModule *ui;

};

#endif // EXTERNALSOURCEMODULE_H
