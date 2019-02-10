#ifndef MYQPUSHBUTTON_H
#define MYQPUSHBUTTON_H

#include <QWidget>
#include <QPushButton>

/*!
 * \brief Classe purement graphique permettant d'obtenir des PushButton personnalisés
 */
class MyQPushButton : public QPushButton
{
public:

    MyQPushButton(QWidget *parent);

    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;

private:

    QPixmap m_normal;
    QPixmap m_hover;
    QPixmap m_pressed;
};

#endif // MYQPUSHBUTTON_H
