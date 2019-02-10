#include "myqgroupbox.h"
#include "qstyle.h"
#include "qstyleoption.h"
#include "qstylepainter.h"
#include "qlayout.h"

MyQGroupBox::MyQGroupBox(QWidget * parent) : QGroupBox(parent), m_background(":/groupbox_background.png"), m_title(":/groupbox_title.png")
{
}

void MyQGroupBox::paintEvent(QPaintEvent *)
{
    if (contentsMargins().top() != 64)
        setContentsMargins(10,64,10,10);

    QStyleOptionGroupBox option;
    initStyleOption(&option);

    QPainter painter(this);
    painter.drawPixmap(0,15,size().width(),size().height()-15, m_background);
    painter.drawPixmap(size().width()/2 - 118, 0, 236, 54, m_title);

    painter.setPen(QPen(Qt::white));
    painter.setFont(QFont("Gabriola", 25));
    painter.drawText(size().width()/2 - 118 + 20,8,236 - 40,37, Qt::AlignCenter, option.text);
}
