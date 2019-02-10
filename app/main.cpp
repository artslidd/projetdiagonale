#include <QApplication>

#include <QtCore>
#include <QDebug>

#include "widgets/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Projet MISS");

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}

