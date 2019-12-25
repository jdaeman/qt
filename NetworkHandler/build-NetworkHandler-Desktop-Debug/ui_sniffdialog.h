/********************************************************************************
** Form generated from reading UI file 'sniffdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SNIFFDIALOG_H
#define UI_SNIFFDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
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

    void setupUi(QDialog *SniffDialog)
    {
        if (SniffDialog->objectName().isEmpty())
            SniffDialog->setObjectName(QStringLiteral("SniffDialog"));
        SniffDialog->resize(630, 520);
        edit_filter = new QLineEdit(SniffDialog);
        edit_filter->setObjectName(QStringLiteral("edit_filter"));
        edit_filter->setGeometry(QRect(10, 40, 610, 27));
        list_packet = new QListWidget(SniffDialog);
        list_packet->setObjectName(QStringLiteral("list_packet"));
        list_packet->setGeometry(QRect(10, 70, 610, 201));
        push_start = new QPushButton(SniffDialog);
        push_start->setObjectName(QStringLiteral("push_start"));
        push_start->setGeometry(QRect(10, 10, 99, 27));
        push_start->setAutoDefault(false);
        push_stop = new QPushButton(SniffDialog);
        push_stop->setObjectName(QStringLiteral("push_stop"));
        push_stop->setGeometry(QRect(140, 10, 99, 27));
        push_stop->setAutoDefault(false);
        text_detail = new QTextBrowser(SniffDialog);
        text_detail->setObjectName(QStringLiteral("text_detail"));
        text_detail->setGeometry(QRect(10, 280, 610, 230));

        retranslateUi(SniffDialog);

        QMetaObject::connectSlotsByName(SniffDialog);
    } // setupUi

    void retranslateUi(QDialog *SniffDialog)
    {
        SniffDialog->setWindowTitle(QApplication::translate("SniffDialog", "Dialog", 0));
        push_start->setText(QApplication::translate("SniffDialog", "Start", 0));
        push_stop->setText(QApplication::translate("SniffDialog", "Stop", 0));
    } // retranslateUi

};

namespace Ui {
    class SniffDialog: public Ui_SniffDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SNIFFDIALOG_H
