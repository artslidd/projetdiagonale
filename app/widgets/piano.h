#ifndef PIANO_H
#define PIANO_H

#include <QWidget>

/*!
 * \brief La classe affichant le piano et gérant les clics sur ses touches.
 * Cette classe peut selon son mode afficher soit un piano, soit une image générique (tortue/lièvre).
 * En mode piano, si l'utilisateur clique sur une touche, la classe envoie des signaux en conséquence.
 */
class Piano : public QWidget
{
    Q_OBJECT
public:
    explicit Piano(QWidget *parent = 0);

    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;

    void mousePressEvent(QMouseEvent * e) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent * e) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent * e) Q_DECL_OVERRIDE;

public slots:
    //! Toggle permettant de passer du mode piano au mode image simple et inversement
    void switchMode();

signals:
    /*!
     * \brief Signal indiquant qu'une touche de piano vient de recevoir un MousePress
     * \param frequency La fréquence en Hz correspondant à la touche appuyée
     */
    void pianoKeyPressed(int frequency);
    //! Signal indiquant que la touche de piano qui était appuyée n'est plus sous la souris (la touche est relâchée mais cela ne correspond pas nécessairement à un MouseRelease)
    void pianoKeyReleased();

private:
    //! La pixmap en mode piano
    QPixmap m_piano;
    //! La pixmap en mode image simple (tortue/lièvre)
    QPixmap m_freq;

    //! Booléen valant true en mode piano
    bool m_pianoMode;
    //! Booléen permettant le bon fonctionnement des interactions utilisateur
    int m_keyPressed;
    //! Booléen permettant le bon fonctionnement des interactions utilisateur
    int m_keyHovered;
};

#endif // PIANO_H
