#include "mainwindow.h"

#include <QApplication>
#include <QMutex>

QMutex sniff_mutex;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    w.setWindowTitle("NetworkHandler");
    w.show();

    return a.exec();
}
