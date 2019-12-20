#ifndef SNIFF_DIALOG_H
#define SNIFF_DIALOG_H

#include <QDialog>
#include <nic.h>
#include <sniffer.h>

#define MAX_PACKETS 16384

namespace Ui {
class sniff_dialog;
}

class sniff_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit sniff_dialog(QWidget *parent = 0);
    ~sniff_dialog();

    void set_nic(struct nic * sel)
    {
        this->sel = sel;
    }

private slots:
    void on_sniff_dialog_finished(int result);

    void on_start_btn_clicked();

    void on_stop_btn_clicked();

    void sig_from_sniffer(int type, char * buf, int len);

    void on_all_btn_clicked();

    void on_arp_btn_clicked();

    void on_ip_btn_clicked();

    void on_icmp_btn_clicked();

    void on_igmp_btn_clicked();

    void on_tcp_btn_clicked();

    void on_udp_btn_clicked();

    void on_packet_list_currentRowChanged(int currentRow);

private:
    Ui::sniff_dialog *ui;

    struct nic * sel;
    sniffer * sniff_thread;

    char * packets[MAX_PACKETS];
    int last_pkt_idx;

    unsigned short current_protocol;

    void memory_init();
    void terminate_sniffer();
};

#endif // SNIFF_DIALOG_H
