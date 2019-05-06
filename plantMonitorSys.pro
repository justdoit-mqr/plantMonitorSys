#-------------------------------------------------
#
# Project created by QtCreator 2017-03-16T13:53:28
#
#-------------------------------------------------

QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = plantMonitorSys
TEMPLATE = app

#imx6ul的qt5.6.2环境下没有gl3的头文件和库,而qcustomplot.h会通过 <QtPrintSupport/QtPrintSupport>
#--<QtPrintSupport/QtPrintSupportDepends>--<QtGui/QtGui>最终包含"qopengl.h"，在该头文件默认会
#包含gl3.h，导致找不到头文件编译失败。所以这里定义宏声明不使用ES_3,这样默认会用ES_2就没问题了
DEFINES += QT_NO_OPENGL_ES_3#imx6ul qcustomplot
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
