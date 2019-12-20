#include "main_dialog.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    main_dialog w;

    w.setWindowTitle("NeTool");
    w.show();

    return a.exec();
}
