#include <QMessageBox>

void err_msgbox(const char * title, const char * text)
{
    QMessageBox qbox;

    qbox.setWindowTitle(title);
    qbox.setText(text);
    qbox.exec();
}
