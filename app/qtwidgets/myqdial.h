#ifndef MYQDIAL_H
#define MYQDIAL_H

#include <QWidget>
#include <QDial>

/*!
 * \brief Classe purement graphique permettant d'obtenir des Dial personnalisés
 */
class MyQDial : public QDial
{
public:
    MyQDial(QWidget *parent);

    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;

private:
    QPixmap m_back;
    QPixmap m_front;
};

#endif // MYQDIAL_H
