#ifndef MIDIDIALOG_H
#define MIDIDIALOG_H

#include <QDialog>
#include <QMap>
#include "../midi/midiengine.h"

namespace Ui {
class MidiDialog;
}

/*!
 * \brief Classe d'interface utilisateur gérant la panneau de configuration MIDI.
 * Cette classe ne fait que traduire les actions de l'utilisateur en commande pour le moteur MIDI MidiEngine.
 * Elle n'effectue aucun traitement elle-même. Son objectif est notamment de permettre à l'utilisateur
 * d'associer à chaque bouton de son périphérique MIDI un rôle dans l'application.
 */
class MidiDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MidiDialog(QWidget *parent = 0);
    ~MidiDialog();

    /*!
     * \brief Lien avec le moteur MIDI à effectuer juste après l'instanciation
     * \param midiEngine Pointeur vers le moteur MIDI de l'application
     */
    void link(MidiEngine * midiEngine);

    void showEvent(QShowEvent * event) Q_DECL_OVERRIDE;
    void accept() Q_DECL_OVERRIDE;
    void reject() Q_DECL_OVERRIDE;

public slots:

    //! Slot demandant la remise à zéro des boutons d'association (ils passent tous sur 'Apprendre')
    void reinitializeAssociations();

    //! Slot permettant de gérer un double clic sur une source MIDI dans la liste (il active la source en question auprès de RtMidi)
    void validateSelectedSource(const QModelIndex & index);
    //! Demande à RtMidi de lister de nouveau les périphériques MIDI et rafraîchit la liste en conséquence
    void refreshInputsList();

    //! Slot permettant de réagir à un clic sur un bouton d'association. Il utilise les fonctions Qt et une QMap pour retrouver de quel bouton il s'agit
    void associationButtonClicked();

    /*!
     * \brief Slot gérant le fait qu'un contrôle MIDI vient d'être appris
     * \param input Le rôle du contrôle
     * \param control L'identifiant du contrôle sur l'appareil
     */
    void midiInputLearned(MidiEngine::MidiSoftwareInput input, int control);
    /*!
     * \brief Slot gérant l'échec de l'apprentissage d'un contrôle MIDI
     * \param input Le rôle qui vient de perdre son association
     */
    void midiInputLearningFailed(MidiEngine::MidiSoftwareInput input);

    //! Ouvre un fichier de configuration, le charge et modifier les états des boutons
    void openConfigFile(QString filename = "");
    //! Enregistre l'état actuel dans le fichier de configuration actuellement chargé
    void saveConfigToFile();
    //! Crée un fichier de configuration et enregistre l'état actuel dedans
    void createAndSaveConfigFile();

private:

    Ui::MidiDialog *ui;

    //! Map permettant de retrouver le rôle d'un bouton à partir de son nom, utilisé lorsqu'un bouton est appuyé, sans que l'on sache lequel
    QMap< QString, MidiEngine::MidiSoftwareInput > m_midiSoftwareInputs;
    //! Map permettant, connaissant un rôle donné, de retrouver le bouton qui lui est associé pour pouvoir l'actualiser
    QMap< MidiEngine::MidiSoftwareInput, QPushButton * > m_associationButtons;

    //! Pointeur vers le moteur MIDI de l'application
    MidiEngine * m_midiEngine;
};

#endif // MIDIDIALOG_H
