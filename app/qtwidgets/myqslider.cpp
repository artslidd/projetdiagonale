#include "myqslider.h"
#include "qstyle.h"
#include "qstyleoption.h"
#include "qstylepainter.h"

MyQSlider::MyQSlider(QWidget *parent) : QSlider(parent), m_guide(":/slider_guide.png"), m_glow(":/slider_glow.png"), m_cursor(":/slider_cursor.png")
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
}

void MyQSlider::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    QStyleOptionSlider opt;
    initStyleOption(&opt);

    const int h = size().height();
    const int w = size().width();

    const float p = (float) (opt.sliderValue - opt.minimum) / (float) opt.maximum;

    painter.drawPixmap(0,h/4,w,h/2, m_guide);
    painter.drawPixmap(0,h/4,8 + (97*w/100) * p + 1,h/2, m_glow, 0, 0, (8 + (97*w/100) * p + 1) * 256 / w, 16);
    painter.drawPixmap((w - 10) * p - 3,0,16,h, m_cursor);
}
