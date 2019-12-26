#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "nic.h"
#include "util.h"
#include "sniff.h"

#include <QDebug>
#include <QMessageBox>
#include <string.h>
#include <curl/curl.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    which = -1;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_list_nic_clicked(const QModelIndex &index)
{
    QString qstr;
    int idx = index.row();

    struct nic * nic = get_nic(idx);
    which = idx;

    ui->text_nic_info->clear();
    qstr.sprintf("IP\t%s", inet_ntoa_e(nic->ip));
    ui->text_nic_info->append(qstr);
    qstr.sprintf("Subnet\t%s", inet_ntoa_e(nic->subnet));
    ui->text_nic_info->append(qstr);
    qstr.sprintf("MAC\t%s", ether_ntoa_e(nic->mac, 6));
    ui->text_nic_info->append(qstr);
    //qstr.sprintf("MTU\t%d", nic->mtu);
    //ui->text_nic_info->append(qstr);
    qstr.sprintf("Gateway\t%s", inet_ntoa_e(nic->route));
    ui->text_nic_info->append(qstr);
    qstr.sprintf("Neigh\t%s", ether_ntoa_e(nic->neigh, 6));
    ui->text_nic_info->append(qstr);
}

void MainWindow::on_push_nic_reset_clicked()
{
    ui->list_nic->clear();
    which = -1;

    int cnt = init_nic();
    if (cnt < 0)
        return;

    for (int idx = 0; idx < cnt; idx++)
    {
        const struct nic * nic = get_nic(idx);
        QString name;

        name.sprintf("%s", nic->name);
        ui->list_nic->addItem(name);
    }
}

void MainWindow::on_push_public_clicked()
{
    QMessageBox mbox;
    char * ret;

    mbox.setWindowTitle("Your Public IP");
    ret = get_public_ip();
    if (ret)
        mbox.setText(ret);
    else
        mbox.setText("OOPS, some error");

    mbox.exec();
}

void MainWindow::on_push_sniff_clicked()
{
    int ret;
    QMessageBox mbox;

    mbox.setWindowTitle("OOPS");
    if (which < 0)
    {
        mbox.setText("Check interface");
        mbox.exec();
        return;
    }

    struct nic * nic = get_nic(which);
    ret = sd.sniff_setup(nic);

    if (ret < 0)
    {
        mbox.setText(strerror(-ret));
        mbox.exec();
        return;
    }

    sd.show();
}

size_t write_callback(char * ptr, size_t size, size_t nmemb, void * userdata)
{
    //printf("%s\n", ptr);
    qDebug() << size * nmemb << endl << endl;

    return size * nmemb;
}

void MainWindow::on_pushButton_clicked()
{
   testtest();
}
