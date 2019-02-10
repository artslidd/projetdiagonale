#ifndef MYQRADIOBUTTON_H
#define MYQRADIOBUTTON_H

#include <QWidget>
#include <QRadioButton>

/*!
 * \brief Classe purement graphique permettant d'obtenir des RadioButton personnalisés
 */
class MyQRadioButton : public QRadioButton
{
public:
    MyQRadioButton(QWidget *parent);

    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;
    bool hitButton(const QPoint &pos) const Q_DECL_OVERRIDE;

private:
    QPixmap m_pixmap;
};

#endif // MYQRADIOBUTTON_H
