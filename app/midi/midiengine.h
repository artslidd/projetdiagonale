#ifndef MIDIENGINE_H
#define MIDIENGINE_H

#include "RtMidi.h"
#include <QObject>
#include <QStringList>
#include <QMap>

void midiEngineCallback(double deltaTime, std::vector< unsigned char > *message, void *engine);

/*!
 * \brief La classe permettant de connecter un périphérique MIDI à l'application.
 * Cette classe est totalement configurée par le MidiDialog. Ses entrées sont reçues depuis
 * le callback de RtMidi et ses sorties sont envoyées à la MainWindow via un signal.
 */
class MidiEngine : public QObject
{
    Q_OBJECT

public:
    //! Cette énumération résume les différents rôles dans l'application que peut avoir un bouton sur un périphérique MIDI
    enum MidiSoftwareInput {
        MidiPianoMin = -4,
        MidiPianoMax = -3,
        MidiPianoOn = -2,
        MidiPianoOff = -1,
        MidiNextSource = 0,
        MidiPreviousSource,
        MidiPlay,
        MidiPause,
        MidiSourceSlider1,
        MidiSourceSlider2,
        MidiSampleBass,
        MidiSampleMelody,
        MidiSampleEffect,
        MidiStopBass,
        MidiStopMelody,
        MidiStopEffect,
        MidiPianoToggle,
        MidiPianoCheckbox,
        MidiMaximaToggle,
        MidiMaximaCheckbox,
        MidiSpectrumAutoscaleToggle,
        MidiSpectrumAutoscaleCheckbox,
        MidiSpectrumZoomPlus,
        MidiSpectrumZoomMinus,
        MidiAutoAdjustWaveform,
        MidiWaveformPeriodSlider,
        MidiWaveformSynchroToggle,
        MidiWaveformSynchroSwitch,
        MidiPreviousFilter,
        MidiNextFilter,
        MidiFilterAmplitudeSlider,
        MidiFilterFrequencySlider1,
        MidiFilterFrequencySlider2
    };

    MidiEngine();
    ~MidiEngine();

    /*!
     * \brief Fournit la liste des périphériques MIDI actuellement disponibles
     * \return La liste des périphériques, référencés par leur nom
     */
    QStringList getAvailableInputs();
    /*!
     * \brief Fonction à appeler pour attacher le MidiEngine à une source MIDI, si une configuration de touches est déjà paramétrée, le moteur commence immédiatement à émettre des signaux lors des appuis sur le contrôleur MIDI
     * \param source Le nom de la source à écouter
     */
    void selectSource(const QString& source);

    /*!
     * \brief Passe le moteur MIDI en mode apprentissage pour le rôle fourni en paramètre.
     * Le prochain appui sur une touche du contrôleur MIDI enregistrera la correspondance rôle/touche.
     * Si le rôle était attribué auparavant à une autre touche du contrôleur MIDI, un
     * signal \a midiInputLearningFailed() est émis pour le rôle qui a perdu son attribution.
     * Dans tous les cas, un \a midiInputLearned() sera émis lors du prochain appui sur une touche.
     * \param input Le rôle qu'aura la prochaine touche MIDI sollicitée
     */
    void learnInput(MidiSoftwareInput input);

    /*!
     * \brief Fonction appelée par le callback de RtMidi chaque fois qu'un contrôle est sollicité sur le périphérique MIDI
     * \param control L'identifiant du contrôle sollicité (fourni par le périphérique MIDI)
     * \param value La nouvelle valeur du contrôle
     */
    void processControlInput(int control, unsigned char value);
    /*!
     * \brief Fonction appelée par le callback de RtMidi chaque fois qu'une note est sollicitée sur le périphérique MIDI
     * \param note L'identifiant de la note
     * \param state Son nouvel état
     */
    void processNoteInput(int note, bool state);

    /*!
     * \brief Demande au moteur de charger un fichier de configuration (toutes les associations actuelles seront effacées puis restaurées depuis le fichier)
     * \param filename Le chemin d'accès du fichier
     */
    void openFile(QString filename);
    /*!
     * \brief Crée un nouveau fichier de configuration avec toutes les associations actuelles
     * \param filename Le nom du nouveau fichier
     */
    void createConfigFile(QString filename);
    //! Ecrase les associations dans le dernier fichier de configuration ouvert
    void saveConfig();

public slots:

    //! Réinitialise toutes les associations rôle/contrôle actuelles
    void clearAssociations();

    //! Demande au moteur de ne plus envoyer de \a midiInputEvent() jusqu'à nouvel ordre
    void blockInputs();
    //! Demande au moteur d'envoyer de nouveau des \a midiInputEvent() jusqu'à nouvel ordre
    void unblockInputs();

signals:

    /*!
     * \brief Signale un appui sur un contrôle du périphérique MIDI en prenant soin de transmettre le rôle de ce contrôle
     * \param input Le rôle du contrôle
     * \param value La valeur qu'il vient de prendre
     */
    void midiInputEvent(MidiEngine::MidiSoftwareInput input, int value);

    /*!
     * \brief Signale le fait qu'un contrôle MIDI vient d'être associé à un rôle
     * \param input Le rôle venant d'être associé
     * \param value L'identifiant du contrôle sollicité (fourni par le périphérique MIDI)
     */
    void midiInputLearned(MidiEngine::MidiSoftwareInput input, int value);
    /*!
     * \brief Signale le fait qu'un contrôle MIDI vient d'être désaffecté d'un rôle
     * \param input Le rôle venant d'être désaffecté
     */
    void midiInputLearningFailed(MidiEngine::MidiSoftwareInput input);

private:

    //! Le nom du fichier actuellement ouvert et dans lequel \a saveConfig() va écrire
    QString m_currentConfigFileName;

    //! Le pointeur vers le module RtMidi utilisé par le moteur
    RtMidiIn * m_rtMidi;

    //! Booléen valant true lorsque l'on est en mode apprentissage
    bool m_learningControl;
    //! En mode apprentissage, contient le rôle qui sera attribué au prochain contrôle MIDI sollicité
    MidiSoftwareInput m_inputBeingLearned;

    //! Booléen valant true si le moteur doit arrêter d'émettre des \a midiInputEvent()
    bool m_inputsBlocked;

    //! Map permettant de savoir quel est le rôle d'un contrôle MIDI
    QMap< int, MidiSoftwareInput > m_inputUsingControl;
    //! Map permettant de savoir quel est le contrôle MIDI actuellement associé à un rôle donné
    QMap< MidiSoftwareInput, int > m_controlAssociatedToInput;

    //! La note la plus basse du périphérique MIDI
    int m_lowerNote;
    //! La note la plus haute du périphérique MIDI
    int m_upperNote;
};

#endif // MIDIENGINE_H
