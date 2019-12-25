#include "sniffdialog.h"
#include "ui_sniffdialog.h"

#include "sniff.h"

#include <QStringList>
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

int SniffDialog::sniff_setup(struct nic * nic)
{
    int err = sniff_init(nic->index, 1);
    if (err < 0)
        return err;

    return 0;
}

void SniffDialog::on_push_start_clicked()
{
    ui->push_start->setEnabled(false);
    ui->push_stop->setEnabled(true);

    clear_pkts();

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
    char summ[1024];

    if (packet_parse(buf, (unsigned char *)summ) < 0)
        return;

    unsigned char * pkt = new unsigned char [len];
    memcpy(pkt, buf, len);
    pkts.push_back(pkt);

    tmp.sprintf("%5d Bytes\t%s", len, summ);

    ui->list_packet->addItem(tmp);
    ui->list_packet->scrollToBottom();
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
    clear_pkts();
}

void SniffDialog::on_list_packet_clicked(const QModelIndex &index)
{
    int idx = index.row();
    int last = packet_parse(pkts[idx], NULL);
    if (last < 0)
        return;

    ui->text_detail->clear();
    for (int l = 2; l <= last; l++)
    {
        QString detail;
        if (get_parsed(l) == NULL)
            break;

        detail.sprintf("%s\n", (const char *)get_parsed(l));
        ui->text_detail->append(detail);
    }
}

void SniffDialog::clear_pkts()
{
    for (unsigned int i = 0; i < pkts.size(); i++)
        delete[]pkts[i];
    pkts.clear();
}
