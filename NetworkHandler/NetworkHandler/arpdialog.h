#ifndef ARPDIALOG_H
#define ARPDIALOG_H

#include <QDialog>
#include "arp.h"

namespace Ui {
class ArpDialog;
}

class ArpDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ArpDialog(QWidget *parent = nullptr);
    ~ArpDialog();

private slots:
    void on_pushButton_clicked();

private:
    Ui::ArpDialog *ui;

};

#endif // ARPDIALOG_H
