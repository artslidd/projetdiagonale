#ifndef FILEMODULE_H
#define FILEMODULE_H

#include <QTimer>
#include <QPushButton>
#include "sndfile.h"
#include "enginemodule.h"
#include "../widgets/progresswidget.h"
#include "../filter/filter.h"

int inoutFile(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void *streamData);

namespace Ui {
class FileModule;
}

/*!
 * \brief Le module de lecture depuis un fichier.
 * Pour le moment, la lecture est restreinte aux fichiers .wav. Une progress bar modifiée permet de naviguer dans le fichier
 */
class FileModule : public EngineModule
{

    Q_OBJECT

public:

    struct StreamData
    {
        SNDFILE * file;
        bool monochannel;
        Filter * filter;
        RingBuffer * currentBuffer;
        bool pianoTone;
        int pianoFreq;
    };

    FileModule(QWidget *parent = 0);
    ~FileModule();
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
    void openButtonPressed();
    void updateProgress();
    void setFileProgress(float seconds);

private :
    void openFile();

    StreamData m_streamData;
    QString m_filename;
    QTimer m_timerProgress;
    bool m_endOfFile;

    // UI
    Ui::FileModule *ui;
    ProgressWidget m_progressWidget;
};

#endif // FILEMODULE_H
