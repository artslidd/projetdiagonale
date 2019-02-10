#include "progresswidget.h"

#include <QtGlobal>
#include <QDebug>
#include <QMouseEvent>

ProgressWidget::ProgressWidget() : m_label(NULL), m_totalSeconds(130), m_selectionInProgress(false)
{
    this->setTextVisible(false);
    this->setMinimum(0);
    this->setMaximum(1000);
    this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    this->setBaseSize(300,20);
}

void ProgressWidget::link(QLabel *label) {
    m_label = label;

    updateLabel();
}

void ProgressWidget::setTotalLength(int seconds) {
    m_totalSeconds = seconds;
    this->setValue(0);

    updateLabel();
}

bool ProgressWidget::setProgressInSeconds(int seconds) {
    if (m_totalSeconds <= 0 || m_selectionInProgress)
        return false;


    setValue(seconds * 1000 / m_totalSeconds);

    updateLabel();
    update();

    return (seconds == m_totalSeconds);
}

void ProgressWidget::reset() {
    m_totalSeconds = 0;
    this->setValue(0);

    updateLabel();

    emit userRequest(0.0f);
}

void ProgressWidget::updateLabel() {
    if (m_label == NULL)
        return;

    // Current
    const int currSeconds = this->value() * m_totalSeconds / 1000;
    const int currM  = currSeconds / 60;
    const int currSS = currSeconds % 60;

    const QString current = QString::number(currM) + ":" + (currSS < 10 ? QString("0") : QString()) + QString::number(currSS);

    // Total
    const int totalM  = m_totalSeconds / 60;
    const int totalSS = m_totalSeconds % 60;

    const QString total = QString::number(totalM) + ":" + (totalSS < 10 ? QString("0") : QString()) + QString::number(totalSS);

    // Display
    m_label->setText(current + " / " + total);
}

void ProgressWidget::mouseMoveEvent(QMouseEvent *e) {
    if (m_totalSeconds <= 0)
        return;

    if (e->buttons() == Qt::LeftButton) {
        const double value = qBound(0.0, (e->localPos().x() + 2) / this->size().width(), 1.0);
        setValue(value * 1000);

        updateLabel();
        update();
    }
}

void ProgressWidget::setProgress(int value) {
    setValue(value);

    emit userRequest(value * m_totalSeconds / 1000);

    updateLabel();
    update();

}

void ProgressWidget::mousePressEvent(QMouseEvent *e) {
    m_selectionInProgress = true;

    this->mouseMoveEvent(e);
}

void ProgressWidget::mouseReleaseEvent(QMouseEvent *e) {
    m_selectionInProgress = false;

    this->mouseMoveEvent(e);

    const float value = qBound(0.0, (e->localPos().x() + 2) / this->size().width(), 1.0);

    emit userRequest(value * m_totalSeconds);
}
