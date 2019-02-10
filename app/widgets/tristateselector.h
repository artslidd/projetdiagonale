#ifndef TRISTATESELECTOR_H
#define TRISTATESELECTOR_H

#include <QObject>
#include <QWidget>
#include <QLabel>

/*!
 * \brief Le TriStateSelector est un widget très simple affichant trois petits ronds cliquable.
 * Chaque rond correspond à un état et est actualisé soit via le slot \a setState(), soit après un \a reset(), soit suite à un clic. Si l'utilisateur clique sur un
 * rond, l'état change et un signal est émis.
 */
class TriStateSelector : public QLabel
{
    Q_OBJECT
public:
    explicit TriStateSelector(QWidget *parent = 0);

    void mousePressEvent(QMouseEvent * e) Q_DECL_OVERRIDE;

public slots:
    /*!
     * \brief Fonction permettant de changer d'état (0 = aucune sélection)
     * \param state Le nouvel état. Si le paramètre est omis, on passe à l'état suivant
     */
    void setState(int state = -1);
    //! Passe à l'état vide (aucune sélection)
    void reset();

signals:
    /*!
     * \brief Signal émis apèrs un changement de sélection, peu importe l'origine du changement
     * \param state L'état suite au changement (0 = aucune sélection)
     */
    void stateChanged(int state);

private:
    //! Pixmap de l'état OFF
    QPixmap m_off;
    //! Pixmap de l'état 1
    QPixmap m_one;
    //! Pixmap de l'état 2
    QPixmap m_two;
    //! Pixmap de l'état 3
    QPixmap m_three;

    //! L'état du sélecteur
    int m_state;
};

#endif // TRISTATESELECTOR_H
