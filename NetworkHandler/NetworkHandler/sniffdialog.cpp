#include "sniffdialog.h"
#include "ui_sniffdialog.h"

#include <QDebug>

SniffDialog::SniffDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SniffDialog)
{
    ui->setupUi(this);

    ui->push_start->setEnabled(true);
    ui->push_stop->setEnabled(false);

    connect(&sniffer, SIGNAL(capture(unsigned char *, int, void *)),
            this, SLOT(capture(unsigned char *, int, void *)));
}

SniffDialog::~SniffDialog()
{
    delete ui;
}

void SniffDialog::on_push_start_clicked()
{
    ui->push_start->setEnabled(false);
    ui->push_stop->setEnabled(true);

    sniffer.start();
}

void SniffDialog::on_push_stop_clicked()
{
    ui->push_start->setEnabled(true);
    ui->push_stop->setEnabled(false);

    sniffer.terminate();
    sniffer.wait();
}

void SniffDialog::capture(unsigned char *buf, int len, void *arg)
{
    QString tmp;
    tmp.sprintf("%d", len);
    ui->listWidget->addItem(tmp);
}

void SniffDialog::on_edit_filter_editingFinished()
{
    //protocl filter
    qDebug() << ui->edit_filter->text();
}
