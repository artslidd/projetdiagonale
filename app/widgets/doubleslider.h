#ifndef DOUBLESLIDER_H
#define DOUBLESLIDER_H

#include <QWidget>

/*!
 * \brief Classe régissant un widget slider pouvant contenir un ou deux curseurs au choix.
 * Le double slider est un QWidget utilisé essentiellement pour le filtrage et
 * qui permet à l'utilisateur de sélectionner une ou deux valeur entre 0 et 100
 * selon si le DoubleSlider est en mode Single ou Double.
 * Les passages de Single à Double et inversement tentent de conserver la position des
 * curseurs lors de la transformation de façon intuitive.
 */
class DoubleSlider : public QWidget
{
    Q_OBJECT
public:
    explicit DoubleSlider(QWidget *parent = 0);

    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent * e) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent * e) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent * e) Q_DECL_OVERRIDE;

signals:

    /*!
     * \brief Signal émis lorsque le slider est en mode Single et que le cureseur est déplacé
     * \param value La nouvelle valeur du slider entre 0 et 100
     */
    void singleChange(int value);
    /*!
     * \brief Signal émis lorsque le slider est en mode Double et qu'un des curseurs est déplacé
     * \param left La valeur du slider le plus à gauche après l'événement
     * \param right La valeur du slider le plus à droite après l'événement
     */
    void doubleChange(int left, int right);

public slots:

    //! Permet de faire passer le DoubleSlider en mode Single
    void switchToSingle();
    //! Permet de faire passer le DoubleSlider en mode Double
    void switchToDouble();

    /*!
     * \brief Déplace le curseur de gauche (le restreint entre 0 et le curseur de doite)
     * \param value La nouvelle valeur souhaitée
     */
    void setLeftCursor(int value);
    /*!
     * \brief Déplace le curseur de droite (le restreint entre le curseur de gauche et 100)
     * \param value La nouvelle valeur souhaitée
     */
    void setRightCursor(int value);

private:
    //! Vaut vrai si le DoubleSlider est en mode Single
    bool m_single;

    //! La valeur du curseur de gauche
    int m_left;
    //! La valeur du curseur de droite
    int m_right;

    //! Vaut vrai si l'utilisateur est en train de déplacer le curseur de gauche
    bool m_slidingLeft;
    //! Vaut vrai si l'utilisateur est en train de déplacer le curseur de droite
    bool m_slidingRight;
    //! Nombre de pixels de décalage avec lesquels l'utilisateur a saisi un curseur, necéssaire à l'affichage
    int  m_deltaSliding;

    //! Pixmap du guide du slider
    QPixmap m_guide;
    //! Pixmap de la trace violette à gauche (ou au milieu) des sliders
    QPixmap m_glow;
    //! Pixmap des curseurs
    QPixmap m_cursor;
};

#endif // DOUBLESLIDER_H
