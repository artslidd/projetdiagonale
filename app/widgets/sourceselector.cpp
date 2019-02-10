#include "sourceselector.h"
#include "qstyle.h"
#include "qstyleoption.h"
#include "qstylepainter.h"
#include "qlayout.h"
#include <QMouseEvent>
#include <QtMath>


SourceSelector::SourceSelector(QWidget * parent) : QGroupBox(parent), m_background(":/source_selector_background.png"), m_background_large(":/source_selector_background_large.png"), m_background_left(":/source_selector_background_left.png"), m_title(":/groupbox_title.png"), m_buttons(":/source_selector_buttons.png"), m_glow(":/glow.png"), m_index(0)
{
    setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    setMinimumWidth(200);
}

void SourceSelector::paintEvent(QPaintEvent *)
{
    const int w = size().width();
    const int h = size().height();

    if (contentsMargins().left() != 12*w/100 and m_index !=3)
        setContentsMargins(12*w/100,64,10,10);
    else if(contentsMargins().left() != 12*w/100 and m_index ==3)
        setContentsMargins(6*w/100,10,10,10);

    QStyleOptionGroupBox option;
    initStyleOption(&option);

    QPainter painter(this);
    painter.drawPixmap(114 , 15, w-116, h-15, w>700 ? m_background_large : m_background);
    painter.drawPixmap(2,15, 112, h-15, m_background_left);

    painter.drawPixmap(w/2 - 118, 0, 236, 54, m_title);
    painter.drawPixmap(5, h/2 - 150, 80, 300, m_buttons);
    painter.drawPixmap(5, h/2 - 150 + 75*m_index, 80, 75, m_glow); 

    painter.setPen(QPen(Qt::white));
    painter.setFont(QFont("Gabriola", 25));
    painter.drawText(w/2 - 118 + 20,8,236 - 40,37, Qt::AlignCenter, option.text);

    painter.setFont(QFont("Gabriola", 15));
    painter.drawText(9, h - 25, 50, 15, Qt::AlignCenter, "MIDI");
}

void SourceSelector::mousePressEvent(QMouseEvent *e)
{
    const int w = size().width();
    const int h = size().height();

    const int x = e->x();
    const int y = e->y();

    if (x > 9 - 12 && x < 9 + 50 + 12 && y > h - 25 - 12 && y < h - 25 + 15 + 12) {
        //if (e->button() == Qt::LeftButton)
            emit midiConfigPanelRequested();
    }

    if (x > 5*w/200 + 80 || x < 5*w/200 || y <= h/2 - 150 || y > h/2+150)
        return;

    const int index = (y - h/2 + 150 - 1) / 75;
    if (index != m_index) {
        m_index = index;
        emit newTabSelected(index);
        if(index==3){
            emit hideWaveform();
        }else{
            emit showWaveform();
        }
        update();
    }

}

int SourceSelector::currentIndex() {
    return m_index;
}

void SourceSelector::nextSource() {
    m_index = (m_index + 1) % 4;
    emit newTabSelected(m_index);
    update();
}

void SourceSelector::previousSource() {
    m_index = (m_index + 3) % 4;
    emit newTabSelected(m_index);
    update();
}
