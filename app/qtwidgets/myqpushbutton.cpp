#include "myqpushbutton.h"
#include "qstyle.h"
#include "qstyleoption.h"
#include "qpainter.h"
#include <QIcon>

MyQPushButton::MyQPushButton(QWidget *parent) : QPushButton(parent), m_normal(":/button_normal.png"), m_hover(":/button_hover.png"), m_pressed(":/button_pressed.png")
{

}

void MyQPushButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    QStyleOptionButton option;
    initStyleOption(&option);

    if (option.state & QStyle::State_Sunken)
        painter.drawPixmap(0,0,size().width(),size().height(), m_pressed);
    else if (option.state & QStyle::State_MouseOver)
        painter.drawPixmap(0,0,size().width(),size().height(), m_hover);
    else
        painter.drawPixmap(0,0,size().width(),size().height(), m_normal);

    if (!option.icon.isNull()) {
        const int w = option.iconSize.width();
        const int h = option.iconSize.height();
        option.icon.paint(&painter, size().width()/2 - w/2, size().height()/2 - h/2, w, h);
    }

    painter.setPen(QPen(Qt::white));
    painter.setFont(QFont("Gabriola", 14));
    painter.drawText(0,0,size().width(),size().height(), Qt::AlignCenter, option.text);
}
