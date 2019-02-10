#include "../widgets/waveformwidget.h"
#include <QResizeEvent>

#include <QtCore>
#include <QDebug>

WaveformWidget::WaveformWidget(QWidget *parent) : QWidget(parent), m_pixmap(WAVEFORM_WIDTH), m_data(NULL)
{    // Taille
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    setBaseSize(WAVEFORM_WIDTH, 300);
    setMinimumHeight(256);
    setMinimumWidth (250);
    setMaximumWidth (WAVEFORM_WIDTH);

    // La pixmap, initialisée à une seconde
    m_pixmap.setDisplayedFrames(FREQUENCE_ECHANTILLONNAGE);

    // Actualisation
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(update()));
    m_timer.start(50);
}

void WaveformWidget::paintEvent(QPaintEvent * e)
{
    Q_UNUSED(e);

    const int w = size().width();
    const int h = size().height();

    // Fond
    QPainter painter(this);
    //painter.fillRect(0, 0, w, h, Qt::white);

    // Dégradés en haut et en bas
    QRect r1(0,0,w,(h-256)/2);
    QLinearGradient gradient1(r1.topLeft(), r1.bottomLeft());
    gradient1.setColorAt(0, Qt::transparent);
    gradient1.setColorAt(1, Qt::black);
    painter.fillRect(r1, gradient1);

    QRect r2(0,h-((h-256)/2)-1,w,(h-256)/2);
    QLinearGradient gradient2(r2.topLeft(), r2.bottomLeft());
    gradient2.setColorAt(0, Qt::black);
    gradient2.setColorAt(1, Qt::transparent);
    painter.fillRect(r2, gradient2);

    // Pixmap
    m_pixmap.update(m_data->rawData(), m_data->getSize(), m_data->getTime());
    m_pixmap.draw(painter, w, h);

    // Bordure
    /*QPen pen(Qt::lightGray);
    painter.setPen(pen);
    painter.drawRect(0, 0, w-1, h-1);*/
}

void WaveformWidget::setBuffer(RingBuffer *buffer) {
    m_data  = buffer;
}

void WaveformWidget::setDisplayedInterval(int interval) {
    const qreal period = WAVEFORM_MIN_PERIOD * qExp(((float)interval / 100.0f) * qLn(WAVEFORM_MAX_PERIOD/WAVEFORM_MIN_PERIOD));

    m_pixmap.setDisplayedFrames(period * FREQUENCE_ECHANTILLONNAGE);

    emit displayedStringChanged((period > 1.0) ? QString::number(period,'f',1) + " s" : QString::number(period*1000.0,'f',1) + " ms");
}

void WaveformWidget::enableTrigger(bool enable) {
    m_pixmap.enableSynchronization(enable);
}

void WaveformWidget::automaticAdjust() {
    // Calcul
    const int k1 = findFirstPositiveZeroCrossing(m_data->getSize() - 1);
    const int k2 = findFirstPositiveZeroCrossing(k1 - 1);

    // Envoi
    const int periodInFrames = m_pixmap.setDisplayedFrames((k1 - k2 + 1) * AUTO_ADJUST_NUMBER_OF_PERIODS);

    // Affichage
    const float period       = (float) periodInFrames / (float) FREQUENCE_ECHANTILLONNAGE;
    emit displayedIntervalChanged((int) (qLn(period / WAVEFORM_MIN_PERIOD) / qLn(WAVEFORM_MAX_PERIOD/WAVEFORM_MIN_PERIOD) * 100.f));
    emit displayedStringChanged((period > 1.0) ? QString::number(period,'f',1) + " s" : QString::number(period*1000.0,'f',1) + " ms");

}

int WaveformWidget::findFirstPositiveZeroCrossing(int start) {
    const float * data = m_data->rawData();

    bool high = false;
    for (int frame = start ; frame > 0 ; --frame) {
        if (high) {
            if (data[frame] <= 0)
                return frame;
        } else if (data[frame] > 0) {
            high = true;
        }
    }

    return 0;
}

void WaveformWidget::hide(){
    parentWidget()->hide();
    parentWidget()->setMaximumWidth(0);
}

void WaveformWidget::show(){
    parentWidget()->show();
    parentWidget()->setMaximumWidth(655);
}
