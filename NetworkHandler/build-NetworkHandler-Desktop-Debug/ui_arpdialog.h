/********************************************************************************
** Form generated from reading UI file 'arpdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.12.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ARPDIALOG_H
#define UI_ARPDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QListWidget>

QT_BEGIN_NAMESPACE

class Ui_ArpDialog
{
public:
    QListWidget *list_host;

    void setupUi(QDialog *ArpDialog)
    {
        if (ArpDialog->objectName().isEmpty())
            ArpDialog->setObjectName(QString::fromUtf8("ArpDialog"));
        ArpDialog->resize(662, 652);
        list_host = new QListWidget(ArpDialog);
        list_host->setObjectName(QString::fromUtf8("list_host"));
        list_host->setGeometry(QRect(0, 10, 611, 201));

        retranslateUi(ArpDialog);

        QMetaObject::connectSlotsByName(ArpDialog);
    } // setupUi

    void retranslateUi(QDialog *ArpDialog)
    {
        ArpDialog->setWindowTitle(QApplication::translate("ArpDialog", "Dialog", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ArpDialog: public Ui_ArpDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ARPDIALOG_H
