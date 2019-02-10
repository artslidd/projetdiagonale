#ifndef INSTRUMENTSELECTOR_H
#define INSTRUMENTSELECTOR_H

#include <QObject>
#include <QPushButton>

#include "../core/samplermodule.h"

/*!
 * \brief Classe des boutons d'instruments. Elle hérite de QPushButton.
 */


class InstrumentSelector : public QPushButton
{
    Q_OBJECT
public:
    InstrumentSelector(QWidget *parent);

public slots:
    /*!
     * \brief Slot appelé lorsque le bouton est cliqué.
     * Il incrémente de 1 l’état du bouton, met à jour la valeur textuelle du bouton et active l’accord de la colonne associée si ce dernier n’est pas déjà activé.
     */
    void setState();
    /*!
     * \brief Slot activé suite au signal updateAllButtons, il met à jour la couleur du bouton en fonction de l’accord de la colonne.
     */
    void setChord();
    /*!
     * \brief Slot activé suite au signal updateAllButtons, il met à jour la valeur du bouton. Slot utilisé par la méthode random de samplerModule par exemple.
     */
    void updateState();

private slots:
    //! Remet à zéro le bouton
    void reset();


private:
    //! Numero de l'instrument
    int m_state;
    //! Nom de l'accord
    QString m_chord;
    //! Etat de l'accord associé
    int m_chord_state;
    //! Pointeur vers le SamplerModule
    SamplerModule *module;


};

#endif // INSTRUMENTSELECTOR_H
