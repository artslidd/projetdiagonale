#include "filterwidget.h"

#include <QtCharts/QtCharts>
#include <QMouseEvent>
#include <QPainter>
#include <QColor>
#include <QtMath>
#include <QtCore>
#include "../constants.h"

using namespace QtCharts;

FilterWidget::FilterWidget(QWidget * parent) : QWidget(parent), m_background(":/filter_background.png"), m_foreground(":/filter_foreground.png"), m_border(":/filter_border.png")
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    setBaseSize(300,200);
    setMinimumHeight(80);
    setMinimumWidth(50);

    m_filter.setType(Filter::DESACTIVE);
}

void FilterWidget::paintEvent(QPaintEvent * e)
{
    Q_UNUSED(e);

    const int totalW = size().width();
    const int totalH = size().height();

    const int paddingW = totalW * 12 / 768;
    const int paddingH = totalH * 12 / 450;

    const int w = totalW - 2 * paddingW;
    const int h = totalH - 2 * paddingH;

    // Painter
    QPainter painter(this);

    // Pen
    QPen pen(QColor(0,0,0,80));
    pen.setWidth(2);
    painter.setPen(pen);

    // Brush
    QLinearGradient gradient1(0, totalH, 0, totalH/2);
    gradient1.setColorAt(0, QColor::fromRgb(205, 199, 224, 210));
    gradient1.setColorAt(1, QColor::fromRgb(107,  99, 131, 210));

    QLinearGradient gradient2(0, totalH/2, 0, 0);
    gradient2.setColorAt(0, QColor::fromRgb(107,  99, 131, 210));
    gradient2.setColorAt(1, QColor::fromRgb( 40,  35,  56, 210));

    // Diagram
    painter.drawPixmap(0, 0, totalW, totalH, m_border);
    painter.drawPixmap(paddingW, paddingH, w, h, m_background);

    float x = 5.0f;
    for (int i = 0 ; i < m_filter.getNumberOfBars() ; ++i) {
        const float barW = m_filter.getBarLength(i);
        const int   barH = m_filter.getBarValue(i) * 85 / FILTER_MAX_GAIN_PERCENTAGE + 10;

        if (barH > 50) {
            painter.fillRect(paddingW + x, paddingH + h - barH * h / 100, barW * (w - 10) / 100 + 1, (barH - 50) * h / 100, gradient2);
            painter.fillRect(paddingW + x, paddingH + h - 50   * h / 100, barW * (w - 10) / 100 + 1, 50 * h / 100, gradient1);
        } else {
            painter.fillRect(paddingW + x, paddingH + h - barH * h / 100, barW * (w - 10) / 100 + 1, barH * h / 100, gradient1);
        }

        if (i == 0)
            painter.drawLine(paddingW + x, paddingH + h - barH * h / 100 + 2, paddingW + x, paddingH + h - 2);
        else {
            const int prevBarH = (m_filter.getBarValue(i-1) * 85 / FILTER_MAX_GAIN_PERCENTAGE + 10);
            painter.drawLine(paddingW + x, paddingH + h - barH * h / 100, paddingW + x, paddingH + h - prevBarH * h / 100);
        }

        painter.drawLine(paddingW + x, paddingH + h - barH * h / 100, paddingW + x + barW * (w - 10) / 100 + 1, paddingH + h - barH * h / 100);

        if (i == m_filter.getNumberOfBars() - 1)
            painter.drawLine(paddingW + x + barW * (w - 10) / 100 + 1, paddingH + h - barH * h / 100 + 2, paddingW + x + barW * (w - 10) / 100 + 1, paddingH + h - 2);

        x += barW * (w - 10) / 100;
    }

    painter.drawPixmap(paddingW, paddingH, w, h, m_foreground);
}

void FilterWidget::reset() {
    // Reset
    m_filter.reset();

    // Update
    update();
}

void FilterWidget::amplitudeChanged(int value) {
    m_filter.setAmplitude(value);

    update();
}

void FilterWidget::frequencyChangedSingle(int value) { // De 0 à 100

    m_filter.setFrequencySingle(89.f * qPow(9300.f / 89.f, value / 100.0f));

    update();
}

void FilterWidget::frequencyChangedDouble(int lowerValue, int upperValue) {
    m_filter.setFrequencyDouble(89.f * qPow(9300.f / 89.f, lowerValue / 100.0f), 89.f * qPow(9300.f / 89.f, upperValue / 100.0f));

    //m_filter.setFrequencyDouble(1.25f * NOTES_START_FREQUENCY * qPow(128.f / 1.25f / 1.25f, lowerValue / 100.0f), 1.25f * NOTES_START_FREQUENCY * qPow(128.f / 1.25f / 1.25f, upperValue / 100.0f));

    update();
}

void FilterWidget::setNone(bool checked) {

    if(checked) {
        m_filter.setType(Filter::DESACTIVE);
        emit singleMode();
        update();
    }
}

void FilterWidget::setLowPass(bool checked) {

    if(checked) {
        m_filter.setType(Filter::PASSE_BAS);
        emit singleMode();
        update();
    }
}

void FilterWidget::setHighPass(bool checked) {
    if(checked) {
        m_filter.setType(Filter::PASSE_HAUT);
        emit singleMode();
        update();
    }
}

void FilterWidget::setBandPass(bool checked) {
    if(checked) {
        m_filter.setType(Filter::PASSE_BANDE);
        emit doubleMode();
        update();
    }
}

void FilterWidget::setBandCut(bool checked) {
    if(checked) {
        m_filter.setType(Filter::COUPE_BANDE);
        emit doubleMode();
        update();
    }
}

void FilterWidget::attenuationSet(bool attenuation){
    m_filter.setAttenuation(attenuation);
}
