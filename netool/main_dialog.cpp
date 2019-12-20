#include "main_dialog.h"
#include "ui_main_dialog.h"

#include <QDebug>
#include <arpa/inet.h>
#include <QString>
#include <netinet/ether.h>
#include <QMessageBox>
#include <consts.h>
#include <err_msgbox.h>

#include <unistd.h>
#include <pthread.h>

main_dialog::main_dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::main_dialog)
{
    ui->setupUi(this);
    this->sel = NULL;
    this->ui->spoof_stop_btn->setEnabled(false);
    //qDebug() << getpid() << " " << pthread_self() << " " << QThread::currentThreadId();
}

main_dialog::~main_dialog()
{
    delete ui;
}

void main_dialog::on_nic_reset_btn_clicked()
{
    int cnt = nic_init(this->list, LIST_LEN);
    if(cnt < 0)
        return;

    this->ui->nic_list->clear();
    this->ui->nic_info_browser->clear();
    this->sel = NULL;

    for(int idx = 1; idx <= cnt; idx++)
        this->ui->nic_list->addItem(this->list[idx].if_name);
}

void main_dialog::on_nic_list_clicked(const QModelIndex &index)
{
    int ch = index.row() + 1;
    this->sel = &this->list[ch];

    QString temp;
    QTextBrowser * info = this->ui->nic_info_browser;

    info->clear();
    info->append(sel->if_name);
    temp.sprintf("IP :\t%s", inet_ntoa(*(struct in_addr *)&sel->if_laddr));
    info->append(temp);
    temp.sprintf("Netmask :\t%s", inet_ntoa(*(struct in_addr *)&sel->if_netmask));
    info->append(temp);
    temp.sprintf("Gateway :\t%s", inet_ntoa(*(struct in_addr *)&sel->gateway));
    info->append(temp);
    temp.sprintf("MAC :\t%s", ether_ntoa((struct ether_addr *)sel->if_paddr));
    info->append(temp);
}

void main_dialog::on_sniff_btn_clicked()
{
    if(!this->sel)
    {
        err_msgbox("Sniff button error", "You have to select nic");
        return;
    }
    else if(!sd.isHidden())
    {
        err_msgbox("Sniff button error", "You already executed sniffer");
        return;
    }

    sd.setGeometry(0, 0, 550, 600);
    sd.setWindowTitle("Sniffer");
    sd.set_nic(this->sel);
    sd.show();
}

void main_dialog::on_spoof_start_btn_clicked()
{
    this->ui->spoof_stop_btn->setEnabled(true);
    this->ui->spoof_start_btn->setEnabled(false);

    char * victim = this->ui->gateway_edit->text().toLocal8Bit().data();
    unsigned int n_victim = inet_addr(victim);

    for(int i = 0; i < LIST_LEN; i++)
    {
        this->spoof[i] = new spoofer(this->sel, n_victim);
        this->spoof[i]->start();
    }
}

void main_dialog::on_spoof_stop_btn_clicked()
{
    this->ui->spoof_stop_btn->setEnabled(false);
    this->ui->spoof_start_btn->setEnabled(true);

    for(int i = 0; i < LIST_LEN; i++)
    {
        this->spoof[i]->terminate();
        this->spoof[i]->wait();
        delete this->spoof[i];
    }
}
