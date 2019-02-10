#ifndef LOOPCHECKBOX_H
#define LOOPCHECKBOX_H

#include <QWidget>
#include <QCheckBox>
#include "../core/samplermodule.h"

/*!
 * \brief Classe des checkboxes en haut du séquenceur, elles doivent être cochées pour marquer les points de début et d’arrêt de la boucle.
 */

class LoopCheckBox : public QCheckBox
{
    Q_OBJECT;
public:
    LoopCheckBox(QWidget *parent);

public slots:
    //! Appelle setLoopStates de samplerModule avec l'id de la checkbox en paramètre.
    void btnStateChanged();
    //! Méthode appelée à chaque fois qu’une checkbox est cliquée, cette méthode met à jour l’état de la checkbox, la décoche ou la coche en fonction des différents cas.
    void updateState();

private:
    //! Etat actuel de la
    int m_state;
    //! Pointeur vers le SamplerModule
    SamplerModule *module;
};

#endif // LOOPCHECKBOX_H
