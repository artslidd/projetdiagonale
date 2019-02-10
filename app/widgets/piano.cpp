#include "piano.h"
#include "../tools/frequenciesmanager.h"
#include <QMouseEvent>
#include <QPainter>
#include <QDebug>

Piano::Piano(QWidget * parent) : QWidget(parent), m_piano(":/piano.png"), m_freq(":/frequences.png"), m_pianoMode(false), m_keyPressed(-1), m_keyHovered(-1)
{

}

void Piano::switchMode() {
    m_pianoMode = !m_pianoMode;

    if (!m_pianoMode) {
        if (m_keyPressed >= 0)
            emit pianoKeyReleased();

        m_keyPressed = -1;
    }

    update();
}

void Piano::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    const int h = size().height();
    const int w = size().width();

    if (m_pianoMode) {
        painter.drawPixmap(0,0,w,h, m_piano);

        if (m_keyPressed > -1) {
            QPointF key = FrequenciesManager::getPositionInPianoFromIndex(m_keyPressed);
            painter.setPen(QColor::fromRgb(255,255,255,255));
            painter.drawEllipse(key.x() * w, key.y() * h, 10, 10);
        } /*else if (m_keyHovered > -1) {
            QPointF key = FrequenciesManager::getPositionInPianoFromIndex(m_keyHovered);
            painter.setPen(QColor::fromRgb(255,255,255,50));
            painter.drawEllipse(key.x() * w, key.y() * h, 10, 10);
        }*/
    } else {
        painter.drawPixmap(0,0,w,h, m_freq);
    }
}

void Piano::mousePressEvent(QMouseEvent *e) {
    if (!m_pianoMode)
        return;

    int index = FrequenciesManager::getIndexInPianoFrequenciesFromPosition((double) e->localPos().x() / (double) size().width(), (double) e->localPos().y() / (double) size().height());
    m_keyPressed = index;

    static const QVector<float> frequencies = FrequenciesManager::getPianoFrequencies();
    emit pianoKeyPressed(frequencies[index]);

    update();
}

void Piano::mouseMoveEvent(QMouseEvent *e) {
    if (!m_pianoMode || m_keyPressed < 0)
        return;

    /*if  (m_keyPressed < 0) {
        if (e->localPos().x() < 0 || e->localPos().x() > size().width() || e->localPos().y() < 0 || e->localPos().y() > size().height()) {
            m_keyHovered = -1;
            return;
        }

        m_keyHovered = FrequenciesManager::getIndexInPianoFrequenciesFromPosition((double) e->localPos().x() / (double) size().width(), (double) e->localPos().y() / (double) size().height());
        return;
    }*/

    int index = FrequenciesManager::getIndexInPianoFrequenciesFromPosition((double) e->localPos().x() / (double) size().width(), (double) e->localPos().y() / (double) size().height());

    if (m_keyPressed == index)
        return;

    m_keyPressed = index;

    static const QVector<float> frequencies = FrequenciesManager::getPianoFrequencies();
    emit pianoKeyPressed(frequencies[index]);

    update();
}

void Piano::mouseReleaseEvent(QMouseEvent *e) {
    Q_UNUSED(e);

    if (m_keyPressed < 0)
        return;

    m_keyPressed = -1;
    emit pianoKeyReleased();

    update();
}
