#include "arpdialog.h"
#include "ui_arpdialog.h"

ArpDialog::ArpDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ArpDialog)
{
    ui->setupUi(this);
}

ArpDialog::~ArpDialog()
{
    delete ui;
}
