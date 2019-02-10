#ifndef MYQCHECKBOX_H
#define MYQCHECKBOX_H

#include <QWidget>
#include <QCheckBox>

/*!
 * \brief Classe purement graphique permettant d'obtenir des CheckBox personnalisées
 */
class MyQCheckBox : public QCheckBox
{
Q_OBJECT

public:

    MyQCheckBox(QWidget *parent);

    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;
    bool hitButton(const QPoint &pos) const Q_DECL_OVERRIDE;

public slots:

    void check();

private:

    QPixmap m_on;
    QPixmap m_off;
};

#endif // MYQCHECKBOX_H
