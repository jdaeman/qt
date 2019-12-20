#-------------------------------------------------
#
# Project created by QtCreator 2019-03-06T08:52:58
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = netool
TEMPLATE = app


SOURCES += main.cpp\
        main_dialog.cpp \
    nic.cpp \
    sniff_dialog.cpp \
    sniffer.cpp \
    err_msgbox.cpp \
    packet_types.cpp \
    color.cpp \
    lock.cpp \
    spoofer.cpp

HEADERS  += main_dialog.h \
    nic.h \
    sniff_dialog.h \
    consts.h \
    sniffer.h \
    err_msgbox.h \
    packet_types.h \
    color.h \
    lock.h \
    spoofer.h

FORMS    += main_dialog.ui \
    sniff_dialog.ui
