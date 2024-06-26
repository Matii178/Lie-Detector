#include "mainwindow.h"

#include <QApplication>

#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    QFile file("C:/Users/Mateusz/Documents/GitHub/LieDetector/Irrorater.qss");
    file.open(QFile::ReadOnly);

    QString styleSheet { QLatin1String(file.readAll())};

    a.setStyleSheet(styleSheet);

    w.show();
    return a.exec();
}
