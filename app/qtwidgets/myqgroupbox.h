#ifndef MYQGROUPBOX_H
#define MYQGROUPBOX_H

#include <QWidget>
#include <QGroupBox>

/*!
 * \brief Classe purement graphique permettant d'obtenir des GroupBox personnalisées
 */
class MyQGroupBox : public QGroupBox
{
public:
    MyQGroupBox(QWidget *parent);

    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;

private:
    QPixmap m_background;
    QPixmap m_title;
};

#endif // MYQGROUPBOX_H
