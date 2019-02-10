#include "myqradiobutton.h"
#include "qstyle.h"
#include "qstyleoption.h"
#include "qstylepainter.h"
#include "qlayout.h"

MyQRadioButton::MyQRadioButton(QWidget *parent) : QRadioButton(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    setBaseSize(QSize(80+10+125,70));
    setMinimumHeight(50);
    setMinimumWidth(150);
    setMaximumWidth(80+10+125);
}

void MyQRadioButton::paintEvent(QPaintEvent *)
{
    const int textWidth = 80;
    const int imageWidth = size().width() - 90;

    QStyleOptionButton option;
    initStyleOption(&option);

    QPainter painter(this);

    // Arrière-plan
    if (this->isChecked()) {
        painter.setPen(QPen()); //no pen
        painter.setBrush(QBrush(QColor(255,255,255,40)));
        painter.drawRoundedRect(0,5,textWidth,size().height()-10, 2.0, 2.0);
    }

    // Texte
    painter.setPen(QPen(Qt::white));
    painter.setFont(QFont("Gabriola", size().height() < 65 ? 19 - (65 - size().height()) / 5 : 19));

    QStringList txt = option.text.split("\n");
    painter.drawText(0,8,textWidth,(size().height()-16)/2, Qt::AlignCenter, txt.first());
    painter.drawText(0,(size().height()-16)/2+8,textWidth,(size().height()-16)/2, Qt::AlignCenter, txt.last());

    // Pixmap
    if (m_pixmap.isNull()) {
        const QString str = option.text;

        if (str.contains("Aucun"))
            m_pixmap.load(":/filter_miniature_0.png");
        else if (str.contains("bas"))
            m_pixmap.load(":/filter_miniature_1.png");
        else if (str.contains("haut"))
            m_pixmap.load(":/filter_miniature_2.png");
        else if (str.contains("Coupe"))
            m_pixmap.load(":/filter_miniature_4.png");
        else
            m_pixmap.load(":/filter_miniature_3.png");
    }

    painter.drawPixmap(textWidth + 10, 0, imageWidth, size().height(), m_pixmap);

    if (this->isChecked()) {
        painter.setPen(QPen(QColor::fromRgb(123,108,164)));
        painter.drawRoundedRect(textWidth + 10, 0, imageWidth - 1, size().height()-1, 2.0, 2.0);
    }
}

bool MyQRadioButton::hitButton(const QPoint &pos) const {
    return rect().contains(pos);
}

