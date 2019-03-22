#-------------------------------------------------
#
# Project created by QtCreator 2017-03-16T13:53:28
#
#-------------------------------------------------

QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = plantMonitorSys
TEMPLATE = app

#DEFINES += VIDEO_CAPTURE #视频采集功能开启,使用linux的v4l2采集
#DEFINES += VIDEO_ENCODE_SAVE #视频编码保存功能开启,需要特定平台的库,并开启视频采集功能

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
    networkingwidget.h \
    wificonndialog.h

RESOURCES += \
    images.qrc

#视频采集功能相关文件
if(contains(DEFINES,VIDEO_CAPTURE)){
HEADERS  += videocapture.h
SOURCES += videocapture.cpp
}

#视频编码保存功能相关文件
if(contains(DEFINES,VIDEO_ENCODE_SAVE)){
HEADERS  += videoencode.h
SOURCES += videoencode.cpp

LIBS += -L./ffmpeg/lib -lavcodec
LIBS += -L./ffmpeg/lib -lavformat
LIBS += -L./ffmpeg/lib -lavutil
LIBS += -L./ffmpeg/lib -lx264
}
