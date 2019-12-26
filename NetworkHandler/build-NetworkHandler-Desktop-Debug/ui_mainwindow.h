/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
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
    QLineEdit *lineEdit;
    QPushButton *pushButton_2;
    QCheckBox *checkBox;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(659, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        list_nic = new QListWidget(centralwidget);
        list_nic->setObjectName(QStringLiteral("list_nic"));
        list_nic->setGeometry(QRect(0, 0, 191, 91));
        text_nic_info = new QTextBrowser(centralwidget);
        text_nic_info->setObjectName(QStringLiteral("text_nic_info"));
        text_nic_info->setGeometry(QRect(190, 0, 261, 111));
        push_nic_reset = new QPushButton(centralwidget);
        push_nic_reset->setObjectName(QStringLiteral("push_nic_reset"));
        push_nic_reset->setGeometry(QRect(0, 90, 191, 20));
        push_public = new QPushButton(centralwidget);
        push_public->setObjectName(QStringLiteral("push_public"));
        push_public->setGeometry(QRect(450, 0, 81, 111));
        push_sniff = new QPushButton(centralwidget);
        push_sniff->setObjectName(QStringLiteral("push_sniff"));
        push_sniff->setGeometry(QRect(0, 140, 531, 27));
        pushButton = new QPushButton(centralwidget);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(140, 360, 99, 27));
        lineEdit = new QLineEdit(centralwidget);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));
        lineEdit->setGeometry(QRect(540, 10, 113, 27));
        pushButton_2 = new QPushButton(centralwidget);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));
        pushButton_2->setGeometry(QRect(540, 70, 111, 41));
        checkBox = new QCheckBox(centralwidget);
        checkBox->setObjectName(QStringLiteral("checkBox"));
        checkBox->setGeometry(QRect(580, 40, 21, 22));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 659, 25));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0));
        push_nic_reset->setText(QApplication::translate("MainWindow", "Reset", 0));
        push_public->setText(QApplication::translate("MainWindow", "\n"
"Check\n"
"Public IP\n"
"", 0));
        push_sniff->setText(QApplication::translate("MainWindow", "Packet Capture", 0));
        pushButton->setText(QApplication::translate("MainWindow", "IP Location", 0));
        pushButton_2->setText(QApplication::translate("MainWindow", "ARP Manager", 0));
        checkBox->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
