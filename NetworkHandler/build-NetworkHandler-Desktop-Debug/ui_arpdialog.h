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
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_ArpDialog
{
public:
    QPushButton *pushButton;
    QProgressBar *progressBar;

    void setupUi(QDialog *ArpDialog)
    {
        if (ArpDialog->objectName().isEmpty())
            ArpDialog->setObjectName(QString::fromUtf8("ArpDialog"));
        ArpDialog->resize(662, 652);
        pushButton = new QPushButton(ArpDialog);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(190, 110, 331, 141));
        progressBar = new QProgressBar(ArpDialog);
        progressBar->setObjectName(QString::fromUtf8("progressBar"));
        progressBar->setGeometry(QRect(60, 20, 118, 23));
        progressBar->setValue(0);

        retranslateUi(ArpDialog);

        QMetaObject::connectSlotsByName(ArpDialog);
    } // setupUi

    void retranslateUi(QDialog *ArpDialog)
    {
        ArpDialog->setWindowTitle(QApplication::translate("ArpDialog", "Dialog", nullptr));
        pushButton->setText(QApplication::translate("ArpDialog", "start", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ArpDialog: public Ui_ArpDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ARPDIALOG_H
