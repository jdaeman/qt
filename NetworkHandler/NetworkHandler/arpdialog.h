#ifndef ARPDIALOG_H
#define ARPDIALOG_H

#include <QDialog>

namespace Ui {
class ArpDialog;
}

class ArpDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ArpDialog(QWidget *parent = nullptr);
    ~ArpDialog();

private:
    Ui::ArpDialog *ui;
};

#endif // ARPDIALOG_H
