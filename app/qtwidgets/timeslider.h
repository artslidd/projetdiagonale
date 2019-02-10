#ifndef TIMESLIDER_H
#define TIMESLIDER_H

#include <QObject>
#include <QSlider>

#include "../core/samplermodule.h"

/*!
 * \brief Classe du slider sous la grille de séquenceur, crée pour pouvoir déplacer le slider en cliquant sur la barre (en plus de pouvoir le déplacer en glissant le curseur)
 */

class TimeSlider : public QSlider
{
    Q_OBJECT

public:
    TimeSlider(QWidget *parent);
    void mousePressEvent( QMouseEvent * event );

};

#endif // TIMESLIDER_H
