/********************************************************************************
** Form generated from reading UI file 'sniffdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.12.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SNIFFDIALOG_H
#define UI_SNIFFDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextBrowser>

QT_BEGIN_NAMESPACE

class Ui_SniffDialog
{
public:
    QLineEdit *edit_filter;
    QListWidget *list_packet;
    QPushButton *push_start;
    QPushButton *push_stop;
    QTextBrowser *text_detail;
    QPushButton *pushButton;

    void setupUi(QDialog *SniffDialog)
    {
        if (SniffDialog->objectName().isEmpty())
            SniffDialog->setObjectName(QString::fromUtf8("SniffDialog"));
        SniffDialog->resize(694, 664);
        edit_filter = new QLineEdit(SniffDialog);
        edit_filter->setObjectName(QString::fromUtf8("edit_filter"));
        edit_filter->setGeometry(QRect(10, 40, 610, 27));
        list_packet = new QListWidget(SniffDialog);
        list_packet->setObjectName(QString::fromUtf8("list_packet"));
        list_packet->setGeometry(QRect(10, 70, 610, 201));
        push_start = new QPushButton(SniffDialog);
        push_start->setObjectName(QString::fromUtf8("push_start"));
        push_start->setGeometry(QRect(10, 10, 99, 27));
        push_start->setAutoDefault(false);
        push_stop = new QPushButton(SniffDialog);
        push_stop->setObjectName(QString::fromUtf8("push_stop"));
        push_stop->setGeometry(QRect(140, 10, 99, 27));
        push_stop->setAutoDefault(false);
        text_detail = new QTextBrowser(SniffDialog);
        text_detail->setObjectName(QString::fromUtf8("text_detail"));
        text_detail->setGeometry(QRect(10, 280, 610, 371));
        pushButton = new QPushButton(SniffDialog);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(630, 70, 51, 31));

        retranslateUi(SniffDialog);

        QMetaObject::connectSlotsByName(SniffDialog);
    } // setupUi

    void retranslateUi(QDialog *SniffDialog)
    {
        SniffDialog->setWindowTitle(QApplication::translate("SniffDialog", "Dialog", nullptr));
        push_start->setText(QApplication::translate("SniffDialog", "Start", nullptr));
        push_stop->setText(QApplication::translate("SniffDialog", "Stop", nullptr));
        pushButton->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class SniffDialog: public Ui_SniffDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SNIFFDIALOG_H
