#include "mainwindow.h"

#include <QApplication>
#include <QDebug>

#include "util.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    qDebug() << adding(4, 5) << endl;

    return a.exec();
}
