#ifndef CHORDSELECTOR_H
#define CHORDSELECTOR_H
#include "../core/samplermodule.h"

#include <QObject>
#include <QPushButton>

/*!
 * \brief Classe des boutons d’accords. Elle hérite de QPushButton.
 */

class ChordSelector : public QPushButton
{
    Q_OBJECT
public:
    ChordSelector(QWidget *parent);

public slots:
    /*!
     * \brief Recharge l'accord (utile surtout pour le premier click)
     */
    void updateChor();

private slots:
    //! Remet à zéro le bouton
    void reset();

signals:
    /*!
     * \brief Signal émis apèrs un changement d'accord, peu importe l'origine du changement
     * \param chor L'état suite au changement (0 = aucune sélection)
     */
    void chorChanged();

private:
    //! Nom de l'accord
    QString m_chord;
    //! Pointeur vers le SamplerModule
    SamplerModule *module;
    //! Index du bouton
    int m_btn_index;

};

#endif // CHORDSELECTOR_H
