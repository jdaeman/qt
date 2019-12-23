#include "sniffdialog.h"
#include "ui_sniffdialog.h"

#include <QStringList>
#include <QDebug>

#include <linux/if_ether.h>
#include <linux/ip.h>
#include <arpa/inet.h>

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

int SniffDialog::sniff_setup(struct nic * nic)
{
    int err = sniff_init(nic->index, 1);
    if (err < 0)
        return err;

    //int ar[1] = {ALL};
    //sniff_set_filter(ar, 1);

    return 0;
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

    if (len < 1500)
        return;

    struct ethhdr * eth = (struct ethhdr *)buf;
    struct iphdr * ip = (struct iphdr *)(eth + 1);

    tmp.sprintf("\t%d", ntohs(ip->tot_len));
    ui->listWidget->addItem(tmp);
    ui->listWidget->scrollToBottom();
}

void SniffDialog::on_edit_filter_editingFinished()
{
    /*
    //protocl filter
    char msg[512];
    char * ptr;
    int list[64], cnt = 0;

    QString filter = ui->edit_filter->text().toLower();
    if (filter.length() > 512)
        return;

    strcpy(msg, filter.toLocal8Bit().data());

    */

}

void SniffDialog::on_SniffDialog_finished(int result)
{
    sniff_exit();
    qDebug() << result;
}

