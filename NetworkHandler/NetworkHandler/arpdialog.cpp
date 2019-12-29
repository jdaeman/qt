#include "arpdialog.h"
#include "ui_arpdialog.h"

#include <QDebug>
#include <QMessageBox>

#include "util.h"

void aaa(unsigned int ip, unsigned char * mac)
{
    qDebug() << QString((char *)inet_ntoa_e(ip)) << " " << (char *)ether_ntoa_e(mac, 6);
}

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

void ArpDialog::on_pushButton_clicked()
{
    ui->pushButton->setEnabled(false);
    ui->progressBar->reset();
    scanning(0, 255, aaa);

    ui->pushButton->setEnabled(true);
    //qthread, timer -> progressbar updated,
    //arp.h -> int needs_time(int s, int e);
}
