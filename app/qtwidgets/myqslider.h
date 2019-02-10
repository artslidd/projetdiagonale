#ifndef MYQSLIDER_H
#define MYQSLIDER_H

#include <QWidget>
#include <QSlider>

/*!
 * \brief Classe purement graphique permettant d'obtenir des Slider personnalisés
 */
class MyQSlider : public QSlider
{
public:
    MyQSlider(QWidget *parent);

    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;

private:
    QPixmap m_guide;
    QPixmap m_glow;
    QPixmap m_cursor;
};

#endif // MYQSLIDER_H
