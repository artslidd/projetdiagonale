#ifndef ENGINE_H
#define ENGINE_H

#include <QObject>
#include <QStringList>
#include <QVector>
#include <QTableWidget>
#include <QCheckBox>

#include "RtAudio.h"
#include "sndfile.h"
#include "../filter/filter.h"
#include "../widgets/waveformwidget.h"
#include "../widgets/spectrumwidget.h"
#include "../tools/ringbuffer.h"
#include "spectrumanalyser.h"
#include "../constants.h"
#include "externalsourcemodule.h"
#include "basicsignalmodule.h"
#include "filemodule.h"
#include "samplermodule.h"
#include "../midi/midiengine.h"

/*!
 * \brief Le moteur de l'application, orchestre les différents EngineModule correspondant aux sources audio.
 * Cette classe a pour vocation de servir de contexte à RtAudio pour fonctionner. En particulier,
 * elle donne la main tour à tour aux différents modules de l'application (signal de base, fichier, micro, samples)
 * pour que ceux-ci configurent RtAudio et lui fournissent des fonctions inOut appropriées à ce que l'on désire faire.
 */
class Engine : public QObject
{
    Q_OBJECT

public:
    // Construction
    explicit Engine(QObject *parent = 0);
    ~Engine();

    // Initialisation
    //! Lors de l'initialisation du moteur, il est nécessaire de lui fournir la plupart des acteurs de l'application afin qu'il puisse les connecter et les faire travailler ensemble
    void link(Filter * filter, SpectrumWidget * spectrum, WaveformWidget * waveform, ExternalSourceModule *externalSourceModule, BasicSignalModule *basicSignalModule, FileModule *fileModule, SamplerModule *samplerModule);


public slots:
    /*!
     * \brief Demande au moteur de gérer un événement MIDI
     * \param input Le rôle du contrôle qui vient d'être sollicité
     * \param value La nouvellr valeur du contrôle
     * \return Vrai si l'événement a été géré par le moteur
     */
    bool midiInputEvent(MidiEngine::MidiSoftwareInput input, int value);

    /*!
     * \brief Indique au moteur qu'un module vient d'être sélectionné pour activation
     * \param module Pointeur vers le module en question
     */
    void moduleActivated(EngineModule *module);
    /*!
     * \brief Fonction permettant de connecter les différentes checkbox sous les spectre au SpectrumAnalyser
     * \param checkBox_maximaDetection La checkbox "Maxima"
     * \param checkBox_piano La checkbox "Piano"
     */
    void connectSpectrum(QCheckBox *checkBox_maximaDetection, QCheckBox *checkBox_piano);

private:

    // Analyser
    //! L'analyseur de spectre contenu dans le moteur
    SpectrumAnalyser m_analyser;

    // Data being read
    //! Le buffer principal du programme, contenu dans le moteur
    RingBuffer m_currentBuffer;

    // RTAudio
    //! L'instant de RtAudio permettant au programme de fonctionner
    RtAudio m_audio;

    //! Pointeur vers le module actif
    EngineModule *m_currentModule;
    //! Pointeur vers le module source externe
    ExternalSourceModule *m_externalSourceModule;
    //! Pointeur vers le module signal de base
    BasicSignalModule *m_basicSignalModule;
    //! Pointeur vers le module fichier
    FileModule *m_fileModule;
    //! Pointeur vers le module samples
    SamplerModule *m_samplerModule;
};

#endif // ENGINE_H
