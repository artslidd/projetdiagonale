#include "doubleslider.h"
#include "qpainter.h"
#include <QMouseEvent>

#include <QDebug>

DoubleSlider::DoubleSlider(QWidget *parent) : QWidget(parent), m_single(true), m_left(25), m_right(75), m_slidingLeft(false), m_slidingRight(false), m_deltaSliding(0), m_guide(":/slider_guide.png"), m_glow(":/slider_glow.png"), m_cursor(":/slider_cursor.png")
{
    setBaseSize(300,30);
}


void DoubleSlider::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    const int h = size().height();
    const int w = size().width();

    painter.drawPixmap(0,h/4,w,h/2, m_guide,0,0,w,16);

    if (m_single) {
        painter.drawPixmap(0,h/4,m_left * w / 100,h/2, m_glow, 0, 0, m_left * w / 100, 16);
        painter.drawPixmap(m_left * (w - 8) / 100,0,16,h, m_cursor);
    } else {
        painter.drawPixmap(m_left * w / 100,h/4,(m_right - m_left) * w / 100,h/2, m_glow, 0, 0, (m_right - m_left) * w / 100, 16);
        painter.drawPixmap(m_left * (w-8) / 100,0,16,h, m_cursor);
        painter.drawPixmap(m_right * (w-8) / 100,0,16,h, m_cursor);
    }
}

void DoubleSlider::mousePressEvent(QMouseEvent *e) {
    const int w = size().width();
    const int x = e->x();

    if (x > m_left * (w-16) / 100 - 3 && x < m_left * (w-16) / 100 + 16) {
        m_slidingLeft = true;
        m_deltaSliding = x - m_left * (w-16) / 100;

        return;
    }

    if (x > m_right * (w-16) / 100 - 3 && x < m_right * (w-16) / 100 + 16 + 3) {
        m_slidingRight = true;
        m_deltaSliding = x - m_right * (w-16) / 100;

        return;
    }

    if (x > 0 && x < w) {
        const int xValue = x * 100 / (w - 16);

        if (m_single) {
            m_left = qBound(0, xValue < m_left ? m_left - 10 : m_left + 10, 100);
        } else {
            if (xValue < m_left) {
                m_left = qBound(0, m_left - 10, 100);
            } else if (xValue > m_right) {
                m_right = qBound(0, m_right + 10, 100);
            } else {
                if (m_right - xValue > xValue - m_left)
                    m_left  = qBound(0, m_left + 10, m_right - 5);
                else
                    m_right = qBound(m_left + 5, m_right - 10, 100);
            }
        }

        update();
    }

}

void DoubleSlider::mouseReleaseEvent(QMouseEvent *e) {
    Q_UNUSED(e);

    m_slidingLeft = false;
    m_slidingRight = false;
}

void DoubleSlider::mouseMoveEvent(QMouseEvent *e) {
    if (!m_slidingLeft && !m_slidingRight)
        return;

    const int w = size().width();
    const int x = e->x();

    if (m_slidingLeft)
        m_left  = qBound(0, (x - m_deltaSliding) * 100 / (w - 16), m_single ? 100 : m_right - 5);
    else
        m_right = qBound(m_left + 5, (x - m_deltaSliding) * 100 / (w - 16), 100);

    if (m_single)
        emit singleChange(m_left);
    else
        emit doubleChange(m_left, m_right);

    update();
}

void DoubleSlider::switchToSingle() {
    if (m_single)
        return;

    m_single = true;
    m_left = (m_left + m_right) / 2;

    emit singleChange(m_left);

    update();
}

void DoubleSlider::switchToDouble() {
    if (!m_single)
        return;

    m_single = false;

    if (m_left > 85) {
        m_right = m_left;
        m_left -= 25;
    } else if (m_left < 15) {
        m_right = m_left + 25;
    } else {
        m_left -= 12;
        m_right = m_left + 25;
    }

    emit doubleChange(m_left, m_right);

    update();
}

void DoubleSlider::setLeftCursor(int value) {
    if (value >= 0 && (m_single || value < m_right - 5))
        m_left = value;

    if (!m_single && value > m_right - 5)
        m_left = qMax(0, m_right - 5);

    if (m_single)
        emit singleChange(m_left);
    else
        emit doubleChange(m_left, m_right);

    update();
}

void DoubleSlider::setRightCursor(int value) {
    if (value <= 100 && value > m_left + 5)
        m_right = value;

    if (!m_single && value < m_left + 5)
        m_right = qMin(100, m_left + 5);

    if (m_single)
        emit singleChange(m_left);
    else
        emit doubleChange(m_left, m_right);

    update();
}
