#include "myqcheckbox.h"
#include "qpainter.h"

MyQCheckBox::MyQCheckBox(QWidget *parent) : QCheckBox(parent), m_on(":/checkbox_on.png"), m_off(":/checkbox_off.png")
{

}

void MyQCheckBox::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    if (isChecked())
        painter.drawPixmap(0,0,size().width(),size().height(), m_on);
    else
        painter.drawPixmap(0,0,size().width(),size().height(), m_off);
}

bool MyQCheckBox::hitButton(const QPoint &pos) const {
    return rect().contains(pos);
}

void MyQCheckBox::check() {
    setChecked(true);
}
