/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.12.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QListWidget *list_nic;
    QTextBrowser *text_nic_info;
    QPushButton *push_nic_reset;
    QPushButton *push_public;
    QPushButton *push_sniff;
    QPushButton *pushButton;
    QPushButton *push_arp;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(659, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        list_nic = new QListWidget(centralwidget);
        list_nic->setObjectName(QString::fromUtf8("list_nic"));
        list_nic->setGeometry(QRect(0, 0, 191, 91));
        text_nic_info = new QTextBrowser(centralwidget);
        text_nic_info->setObjectName(QString::fromUtf8("text_nic_info"));
        text_nic_info->setGeometry(QRect(190, 0, 261, 111));
        push_nic_reset = new QPushButton(centralwidget);
        push_nic_reset->setObjectName(QString::fromUtf8("push_nic_reset"));
        push_nic_reset->setGeometry(QRect(0, 90, 191, 20));
        push_public = new QPushButton(centralwidget);
        push_public->setObjectName(QString::fromUtf8("push_public"));
        push_public->setGeometry(QRect(450, 0, 81, 111));
        push_sniff = new QPushButton(centralwidget);
        push_sniff->setObjectName(QString::fromUtf8("push_sniff"));
        push_sniff->setGeometry(QRect(0, 140, 531, 27));
        pushButton = new QPushButton(centralwidget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(140, 360, 99, 27));
        push_arp = new QPushButton(centralwidget);
        push_arp->setObjectName(QString::fromUtf8("push_arp"));
        push_arp->setGeometry(QRect(0, 180, 111, 41));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 659, 25));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", nullptr));
        push_nic_reset->setText(QApplication::translate("MainWindow", "Reset", nullptr));
        push_public->setText(QApplication::translate("MainWindow", "\n"
"Check\n"
"Public IP\n"
"", nullptr));
        push_sniff->setText(QApplication::translate("MainWindow", "Packet Capture", nullptr));
        pushButton->setText(QApplication::translate("MainWindow", "IP Location", nullptr));
        push_arp->setText(QApplication::translate("MainWindow", "ARP Manager", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
