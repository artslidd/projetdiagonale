#include "myqdial.h"
#include "qstyle.h"
#include "qstyleoption.h"
#include "qstylepainter.h"
#include <qmath.h>

MyQDial::MyQDial(QWidget *parent) : QDial(parent), m_back(":/dial_back.png"), m_front(":/dial_front.png")
{

}

void MyQDial::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    QStyleOptionSlider opt;
    initStyleOption(&opt);

    const int h = size().height();
    const int w = size().width();

    if (h > w) {
        painter.drawPixmap(0,(h-w)/2,w,w, m_back);

        const float angle = - 5.f * M_PI / 4.f + (6.0f * M_PI / 4.0f) * (opt.sliderValue - opt.minimum) / opt.maximum ;

        const int  r = 21 * w / 100;
        const int dx = r * qCos(angle);
        const int dy = r * qSin(angle);

        painter.drawPixmap((w/2)+dx-7,(h/2)+dy-7,14,14,m_front);
    } else {
        painter.drawPixmap((w-h)/2,0,h,h, m_back);

        const float angle = - 5.f * M_PI / 4.f + (6.0f * M_PI / 4.0f) * (opt.sliderValue - opt.minimum) / opt.maximum ;

        const int  r = 21 * h / 100;
        const int dx = r * qCos(angle);
        const int dy = r * qSin(angle);

        painter.drawPixmap((w/2)+dx-7,(h/2)+dy-7,14,14,m_front);
    }
}
