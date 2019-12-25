#ifndef SNIFFDIALOG_H
#define SNIFFDIALOG_H

#include "sniffer.h"
#include "nic.h"
#include <vector>

#include <QDialog>

namespace Ui {
class SniffDialog;
}

class SniffDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SniffDialog(QWidget *parent = 0);
    ~SniffDialog();

    int sniff_setup(struct nic * nic);

private slots:
    void on_push_start_clicked();

    void on_push_stop_clicked();

    void capture(unsigned char * buf, int len, void * arg);

    void on_edit_filter_editingFinished();

    void on_SniffDialog_finished(int result);

    void on_list_packet_clicked(const QModelIndex &index);

private:
    Ui::SniffDialog *ui;
    Sniffer sniffer;
    int which;
    std::vector<unsigned char *> pkts;

    void clear_pkts();
};

#endif // SNIFFDIALOG_H
