#include "tristateselector.h"
#include <QMouseEvent>

TriStateSelector::TriStateSelector(QWidget *parent) : QLabel(parent), m_off(":/tristate_off.png"), m_one(":/tristate_one.png"), m_two(":/tristate_two.png"), m_three(":/tristate_three.png"), m_state(-1)
{
    setState(0);
}

void TriStateSelector::setState(int state) {
    if (m_state == state)
        return;

    if (state == -1)
        state = (m_state + 1) % 4;

    switch(state) {
    case 1:
        this->setPixmap(m_one);
    break;
    case 2:
        this->setPixmap(m_two);
    break;
    case 3:
        this->setPixmap(m_three);
    break;
    default:
        this->setPixmap(m_off);
    break;
    }

    m_state = state;

    emit stateChanged(state);
}

void TriStateSelector::reset() {
    setState(0);
}

void TriStateSelector::mousePressEvent(QMouseEvent *e) {
    const int w = size().width();
    const int x = e->localPos().x();

    if (x < 0 || x >= w)
        return;

    setState(1 + x * 3 / w);
}
