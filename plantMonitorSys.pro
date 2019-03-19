#-------------------------------------------------
#
# Project created by QtCreator 2017-03-16T13:53:28
#
#-------------------------------------------------

QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = plantMonitorSys
TEMPLATE = app
LIBS += -L./ffmpeg/lib -lavcodec
LIBS += -L./ffmpeg/lib -lavformat
LIBS += -L./ffmpeg/lib -lavutil
LIBS += -L./ffmpeg/lib -lx264

SOURCES += main.cpp\
        mainwidget.cpp \
    plantglobalwidget.cpp \
    equipmentinfothread.cpp \
    infoshowwidget.cpp \
    basetableitem.cpp \
    basetablewidget.cpp \
    addequipment.cpp \
    customplotwidget.cpp \
    qcustomplot/qcustomplot.cpp \
    infocollectwidget.cpp \
    mymessagebox.cpp \
    softkeyboard/softkeyboard.cpp \
    videomonitorwidget.cpp \
    videocapture.cpp \
    videoencode.cpp \
    networkingwidget.cpp \
    wificonndialog.cpp

HEADERS  += mainwidget.h \
    plantglobalwidget.h \
    equipmentinfothread.h \
    infoshowwidget.h \
    basetableitem.h \
    basetablewidget.h \
    addequipment.h \
    customplotwidget.h \
    qcustomplot/qcustomplot.h \
    infocollectwidget.h \
    mymessagebox.h \
    softkeyboard/softkeyboard.h \
    globalvar.h \
    videomonitorwidget.h \
    videocapture.h \
    videoencode.h \
    networkingwidget.h \
    wificonndialog.h

RESOURCES += \
    images.qrc
