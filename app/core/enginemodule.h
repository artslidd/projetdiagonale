#ifndef ENGINEMODULE_H
#define ENGINEMODULE_H

#include <QWidget>
#include "RtAudio.h"
#include "tools/ringbuffer.h"
#include "../midi/midiengine.h"

class Filter;

/*!
 * \brief Classe abstraite représentant une source audio dans le programme.
 * Chaque classe fille doit pouvoir demander à s'activer (quand on appuie sur un élément de son interface graphique),
 * et doit pouvoir gérer son activation effective tout comme sa désactivation.
 * Un autre prérequis pour être un module est de pouvoir superposer à sa sortie audio des sinus lors de l'appui
 * sur des touches de piano.
 */
class EngineModule : public QWidget
{
    Q_OBJECT
public:
    explicit EngineModule(QWidget *parent = 0);

    /*!
     * \brief Liaison du module au buffer principal du programme, ainsi qu'à RtAudio et au filtre actif
     * \param ringBuffer Le buffer principal
     * \param filter Le filtre actualisé en direct
     * \param audio La classe RtAudio
     */
    virtual void link(RingBuffer *ringBuffer, Filter * filter, RtAudio *audio) = 0;
    //! Signale au module qu'il a la main pour créer tous ses flux et commencer à jouer
    virtual void activateAndPlay() = 0;
    //! Signale au module qu'il doit se fermer et laisser totalement la main à un autre module
    virtual void deactivate() = 0;

    //! Fonction utilitaire permettant aux modules de superposer à leur sortie les sinusoïdes associées aux touches des pianos
    static void addPianoTone(float *buffer, unsigned int nBufferFrames, int freq);

public slots:
    /*!
     * \brief Fonction demandant au module de gérer un événement MIDI
     * \param input Le rôle du contrôle MIDI sollicité
     * \param value La nouvelle valeur du contrôle
     * \return True si l'événement a été géré par le module
     */
    virtual bool midiInputEvent(MidiEngine::MidiSoftwareInput input, int value) = 0;

    /*!
     * \brief Demande au module de commencer à superposer un sinus à sa sortie actuelle
     * \param freq La fréquence du sinus à ajouter
     */
    virtual void enablePianoTone(int freq) = 0;
    //! Demande au module d'arrêter de superposer un sinus à sa sortie
    virtual void disablePianoTone() = 0;

signals:
    /*!
     * \brief Signal envoyé par le module pour demander la main sur RtAudio
     * \param module Pointeur sur le module demandant la main
     */
    void activationRequest(EngineModule *module);

protected:
    //! Pointeur vers l'objet principal RtAudio
    RtAudio *m_audio;
    //! Le nombre de frames reçues à chaque appel de inOut()
    unsigned int m_bufferFrames;
    //! Booléen valant true si le module est actif
    bool m_activated;

};

#endif // ENGINEMODULE_H
