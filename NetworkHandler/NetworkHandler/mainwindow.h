#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "sniffdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_list_nic_clicked(const QModelIndex &index);

    void on_push_nic_reset_clicked();

    void on_push_public_clicked();

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    int which;
};
#endif // MAINWINDOW_H
