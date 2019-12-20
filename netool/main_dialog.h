#ifndef MAIN_DIALOG_H
#define MAIN_DIALOG_H

#include <QDialog>
#include <nic.h>
#include <sniff_dialog.h>
#include <spoofer.h>

#define LIST_LEN 5

namespace Ui {
class main_dialog;
}

class main_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit main_dialog(QWidget *parent = 0);
    ~main_dialog();

private slots:
    void on_nic_reset_btn_clicked();

    void on_nic_list_clicked(const QModelIndex &index);

    void on_sniff_btn_clicked();

    void on_spoof_start_btn_clicked();

    void on_spoof_stop_btn_clicked();

private:
    Ui::main_dialog *ui;

    struct nic list[LIST_LEN];
    struct nic * sel;

    sniff_dialog sd;

    spoofer * spoof[LIST_LEN];
};

#endif // MAIN_DIALOG_H
