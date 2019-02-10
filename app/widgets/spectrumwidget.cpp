#include "spectrumwidget.h"
#include "../constants.h"
#include "../tools/frequenciesmanager.h"
#include <QPainter>
#include <QMouseEvent>

#include <QDebug>

SpectrumWidget::SpectrumWidget(QWidget *parent) : QWidget(parent),
    m_spectrum(SPECTRUM_NUMBER_OF_BARS, 0),
    //m_amplitudeMultiplier(0.005f)
    m_amplitude(1000.0),
    m_userAmplitude(1000.0),
    m_smoothAmplitude(1000.0),
    m_fixedScale(false),
    m_pianoMode(false),
    m_buttonPlus(false),
    m_buttonMinus(false),
    m_maximumTimer(0)
{
    m_gradient.setColorAt(0, "#0000ff");
    m_gradient.setColorAt(1, "#4e346a");
    m_gradient.setSpread(QGradient::ReflectSpread);

    m_maxima.resize(4);
    m_maxima.fill(0.0f);
    m_currentMax = 0;

    m_clock.start();
}


void SpectrumWidget::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);

    const int w = size().width();
    const int h = size().height();

    // Fond
    QPainter painter(this);
    //painter.fillRect(0, 0, w, h, Qt::black);

    // Lignes d'arrière-plan

    float step = 75000.0/m_smoothAmplitude < 10 ? 10 :  75000.0/m_smoothAmplitude;

    if (step > h / 2) {
        float subStep = step / 5.0f;


        if (subStep > h / 2) {
            QPen pen(QColor(255, 255, 255, 15));
            painter.setPen(pen);

            float subSubStep = subStep / 5.0f;

            for(float i = h-1 ; i >= 0 ; i -= subSubStep)
                painter.drawLine(0, i, w-1, i);
        }

        QPen pen(QColor(255, 255, 255, 28));
        painter.setPen(pen);
        for(float i = h-1 ; i >= 0 ; i -= subStep)
            painter.drawLine(0, i, w-1, i);
    }

    QPen pen(QColor(255, 255, 255, 50));
    painter.setPen(pen);
    for(float i = h-1 ; i >= 0 ; i -= step)
        painter.drawLine(0, i, w-1, i);


    // Barres
    if (m_pianoMode) {
        const QVector<double> widths = FrequenciesManager::getPianoWidths();

        const int W = size().width();
        double x = 0;

        for(int i = 1 ; i-1 < widths.size() && i < m_spectrum.size() ; i++)
        {
            drawBar(W*x, W*widths[i-1] - 1, (h - SPECTRUM_MARGIN) * (m_spectrum[i] / (m_smoothAmplitude * 1.20f)), &painter);
            x +=  widths[i-1];

        }
    } else {
        const int W = size().width();
        double x = 0;

        for(int i = 0 ; i < m_spectrum.size() ; i++)
        {
            drawBar(W*x, W/m_spectrum.size() - 1, (h - SPECTRUM_MARGIN) * (m_spectrum[i] / (m_smoothAmplitude * 1.20f)), &painter);
            x += 1.0f / (double) m_spectrum.size();

        }
    }

    // Plus et moins
    if (m_fixedScale) {
        if (m_buttonPlus)
            painter.setPen(QPen(QBrush(QColor(210,210,210)), 2, Qt::SolidLine, Qt::RoundCap));
        else
            painter.setPen(QPen(QBrush(QColor(95,95,95)), 2, Qt::SolidLine, Qt::RoundCap));

        painter.drawLine(w - SPECTRUM_PM_MARGIN - SPECTRUM_PM_BOXSIZE, SPECTRUM_PM_BOXSIZE / 2, w - SPECTRUM_PM_MARGIN, SPECTRUM_PM_BOXSIZE / 2);
        painter.drawLine(w - SPECTRUM_PM_MARGIN - SPECTRUM_PM_BOXSIZE / 2, 0, w - SPECTRUM_PM_MARGIN - SPECTRUM_PM_BOXSIZE / 2, SPECTRUM_PM_BOXSIZE);

        if (m_buttonMinus)
            painter.setPen(QPen(QBrush(QColor(210,210,210)), 2, Qt::SolidLine, Qt::RoundCap));
        else if (m_buttonPlus)
            painter.setPen(QPen(QBrush(QColor(95,95,95)), 2, Qt::SolidLine, Qt::RoundCap));

        painter.drawLine(w - SPECTRUM_PM_MARGIN - SPECTRUM_PM_BOXSIZE, SPECTRUM_PM_BOXSIZE + SPECTRUM_PM_SPACE, w - SPECTRUM_PM_MARGIN, SPECTRUM_PM_BOXSIZE + SPECTRUM_PM_SPACE);
    }


    // Bordure
    /*pen.setColor(Qt::lightGray);
    painter.setPen(pen);
    painter.drawRect(0, 0, w-1, h-1);*/
}

void SpectrumWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
}

void SpectrumWidget::mouseReleaseEvent(QMouseEvent *e)
{
    Q_UNUSED(e);

   if (m_buttonMinus) {
       zoomOut();
   } else if (m_buttonPlus) {
       zoomIn();
   }
}

void SpectrumWidget::mouseMoveEvent(QMouseEvent *e)
{
    m_buttonMinus = false;
    m_buttonPlus  = false;

    if (e->x() > size().width() - SPECTRUM_PM_MARGIN - SPECTRUM_PM_BOXSIZE && e->x() < size().width() - SPECTRUM_PM_MARGIN && e->y() < 2 * SPECTRUM_PM_BOXSIZE + SPECTRUM_PM_SPACE && e->y() > 0) {
        if (e->y() < SPECTRUM_PM_BOXSIZE + SPECTRUM_PM_SPACE / 2)
            m_buttonPlus = true;
        else
            m_buttonMinus = true;
    }
}

void SpectrumWidget::zoomIn() {
    if (!m_fixedScale)
        return;

    m_userAmplitude /= 1.15f;
}

void SpectrumWidget::zoomOut() {
    if (!m_fixedScale)
        return;

    m_userAmplitude *= 1.15f;
}

void SpectrumWidget::switchMode() {
    m_pianoMode = !m_pianoMode;

    update();
}

void SpectrumWidget::drawBar(int x, int width, int height, QPainter *painter)
{
    // Les coordonnees correspondent au coin BAS gauche de la barre.
    const int h = size().height();
    height = qBound(1, height-SPECTRUM_IGNORE_PIXELS, size().height() - SPECTRUM_MARGIN);

    m_gradient.setStart(x + width / 2, 0);
    m_gradient.setFinalStop(x + width, 0);
    if (height == size().height() - SPECTRUM_MARGIN)
        m_gradient.setColorAt(0, QColor(80 + 90 + 30, 74 + 90 + 30, 101 + 90 + 30));
    else
        m_gradient.setColorAt(0, QColor(80 + 90 * height / h, 74 + 90 * height / h, 101 + 90 * height / h));
    painter->fillRect(QRectF(x, h - SPECTRUM_MARGIN - height, width, height), m_gradient);
}

void SpectrumWidget::spectrumChanged(QVector<float> newSpectrum)
{
    const int elapsedTime = m_clock.restart();

    const float alpha = (elapsedTime > SPECTRUM_TIME_CONSTANT) ? 1.0f : (float) elapsedTime / (float) SPECTRUM_TIME_CONSTANT;

    float maximum = 0.0f;
    for(int i = 0 ; i < m_spectrum.size(); i++) {
        m_spectrum[i] = alpha * newSpectrum[i] + (1.0f - alpha) * m_spectrum[i];

        if (maximum < m_spectrum[i])
            maximum = m_spectrum[i];
    }

    // Amplitude automatique
    m_maximumTimer += elapsedTime;

    if (m_maximumTimer > 1000) {
        m_currentMax = (m_currentMax + 1) % m_maxima.size();
        m_maxima[m_currentMax] = 0.0f;
        m_maximumTimer = 0;
    }

    if (m_maxima[m_currentMax] < maximum)
        m_maxima[m_currentMax] = maximum;

    float max = 0.0f;
    for (int i = 0 ; i < m_maxima.size() ; ++i)
        if (m_maxima[i] > max)
            max = m_maxima[i];

    if (max < 0.30f * m_amplitude)
        m_amplitude = max;
    else if (max > m_amplitude)
        m_amplitude = max;

    if (m_amplitude < 20.0f)
        m_amplitude = 20.0f;

    const float beta = (elapsedTime > 450) ? 1.0f : (float) elapsedTime / (float) 450;

    if(!m_fixedScale){
        m_smoothAmplitude = beta * m_amplitude + (1.0f - beta) * m_smoothAmplitude;
    } else {
        m_smoothAmplitude = beta * m_userAmplitude + (1.0f - beta) * m_smoothAmplitude;
    }
    update();
}

void SpectrumWidget::setAutoscale(int autoScale){
    m_fixedScale = (autoScale == 0);
    setMouseTracking(m_fixedScale);

    if (m_fixedScale)
        m_userAmplitude = m_amplitude;
}
