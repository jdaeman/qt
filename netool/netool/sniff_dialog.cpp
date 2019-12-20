#include "sniff_dialog.h"
#include "ui_sniff_dialog.h"

#include <QDebug>

#include <consts.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <err_msgbox.h>
#include <packet_types.h>
#include <lock.h>


#include <color.h>
#include <time.h>
#include <unistd.h>

sniff_dialog::sniff_dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::sniff_dialog)
{
    ui->setupUi(this);
    this->ui->stop_btn->setEnabled(false);
    this->sniff_thread = NULL;
    this->current_protocol = 0;
    this->last_pkt_idx = -1;

    this->ui->pkt_num->setText("0/16384");

    //qDebug() << getpid() << " " << pthread_self() << " " << QThread::currentThreadId();
}

sniff_dialog::~sniff_dialog()
{
    this->terminate_sniffer();
    this->memory_init();

    delete ui;
}

void sniff_dialog::on_sniff_dialog_finished(int result)
{
    this->terminate_sniffer();
    this->memory_init();
}

void sniff_dialog::memory_init()
{
    for(int i = 0; i < this->last_pkt_idx; i++)
        delete [] this->packets[i];
    this->last_pkt_idx = -1;

    this->ui->packet_list->clear();
    this->ui->sniff_browser->clear();
}

void sniff_dialog::on_start_btn_clicked()
{
    if(!this->current_protocol)
    {
        err_msgbox("sniff start error", "You have to select protocol for packet filter");
        return;
    }

    this->ui->filter_box->setEnabled(false);
    this->ui->start_btn->setEnabled(false);
    this->ui->stop_btn->setEnabled(true);

    this->memory_init();

    this->sniff_thread = new sniffer(this->sel, this->current_protocol);
    connect(this->sniff_thread, SIGNAL(sig_to_sniff_dialog(int, char*, int)),
            this, SLOT(sig_from_sniffer(int, char*, int)));

    this->sniff_thread->start();
    this->ui->sniff_browser->setText("Start packet sniffing");
}

void sniff_dialog::on_stop_btn_clicked()
{
    this->ui->stop_btn->setEnabled(false);

    this->terminate_sniffer();

    this->ui->filter_box->setEnabled(true);
    this->ui->start_btn->setEnabled(true);
    this->ui->sniff_browser->setText("Finish packet sniffing");
}

void sniff_dialog::sig_from_sniffer(int type, char *buf, int len)
{ 
    switch(type)
    {
    case to_sniff_dialog::ERR:
        this->ui->sniff_browser->setTextColor(RED);
        this->ui->sniff_browser->append(buf);
        delete this->sniff_thread;
        this->sniff_thread = NULL;
        break;
    case to_sniff_dialog::PKT_CAP:
        QString infos;

        if(this->last_pkt_idx + 1 >= MAX_PACKETS)
            break;
        this->packets[++this->last_pkt_idx] = new char[len];
        memset(this->packets[this->last_pkt_idx], 0, len);

        pthread_mutex_lock(&mutex);
        memcpy(this->packets[this->last_pkt_idx], buf, len);
        pthread_mutex_unlock(&mutex);

        packet_title(this->packets[this->last_pkt_idx], infos);
        this->ui->packet_list->addItem(infos);
        this->ui->packet_list->scrollToBottom();
        infos.sprintf("%d/%d", this->last_pkt_idx + 1, MAX_PACKETS);
        this->ui->pkt_num->setText(infos);
        break;
    }
}

void sniff_dialog::terminate_sniffer()
{
    if(this->sniff_thread)
    {
        this->sniff_thread->terminate();
        this->sniff_thread->wait();
        delete this->sniff_thread;
        this->sniff_thread = NULL;
    }
}

void sniff_dialog::on_all_btn_clicked()
{
    this->current_protocol = ETH_P_ALL;
}

void sniff_dialog::on_arp_btn_clicked()
{
    this->current_protocol = ETH_P_ARP;
}

void sniff_dialog::on_ip_btn_clicked()
{
    this->current_protocol = ETH_P_IP;
}

void sniff_dialog::on_icmp_btn_clicked()
{
    this->current_protocol = IPPROTO_ICMP;
}

void sniff_dialog::on_igmp_btn_clicked()
{
    this->current_protocol = IPPROTO_IGMP;
}

void sniff_dialog::on_tcp_btn_clicked()
{
    this->current_protocol = IPPROTO_TCP;
}

void sniff_dialog::on_udp_btn_clicked()
{
    this->current_protocol = IPPROTO_UDP;
}

void sniff_dialog::on_packet_list_currentRowChanged(int currentRow)
{
    if(currentRow < 0 || currentRow >= MAX_PACKETS)
        return;

    struct ether_header * eth = (struct ether_header *)this->packets[currentRow];
    QString infos;

    detail_handle(eth, infos);
    this->ui->sniff_browser->setText(infos);
}
