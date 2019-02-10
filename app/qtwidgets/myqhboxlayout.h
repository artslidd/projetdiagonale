#ifndef MYQHBOXLAYOUT_H
#define MYQHBOXLAYOUT_H

#include <QWidget>
#include <QHBoxLayout>

class MyQHBoxLayout : public QHBoxLayout
{
public:
    MyQHBoxLayout(QWidget *parent);

    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;

private:
    QPixmap m_background;
};

#endif // MYQHBOXLAYOUT_H
