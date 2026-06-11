#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setApplicationName(QString("DataMiner"));
    a.setApplicationVersion(QString("1.0.0"));

    MainWindow w;
    w.show();
    return a.exec();
}
