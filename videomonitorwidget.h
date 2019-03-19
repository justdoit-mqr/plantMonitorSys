/*
 *@file:   videomonitorwidget.h
 *@author: 缪庆瑞
 *@date:   2017.5.10
 *@brief:  显示实时监控视频的类，该类主要调用VideoCapture类的方法完成视频帧的采集，然后
 * 对采集到的帧格式处理成图片显示。
 */
#ifndef VIDEOMONITORWIDGET_H
#define VIDEOMONITORWIDGET_H

#include <QWidget>
#include <QImage>
#include <QLabel>
#include <QPushButton>
#include <QEvent>
#include <QThread>
#include <QTimer>
#include "mymessagebox.h"
#include "videocapture.h"

class VideoMonitorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VideoMonitorWidget(QWidget *parent = 0);
    ~VideoMonitorWidget();

protected:
    virtual void paintEvent(QPaintEvent *event);

signals:
    void startRecordSignal();
    void stopRecordSignal();

public slots:
    void startRecordSlot();
    void stopRecordSlot();
    void getFrameSlot();

private:
    QThread *threadB;//视频采集放在线程中进行
    VideoCapture *videoCapture;//视频采集对象
    bool initStatus;//视频设备初始化状态
    uchar *rgbFrame;//RGB格式图像的起始地址
    QImage *image;//QImage图像
    QLabel *videoOutput;//展示视频画面
    QPushButton *startRecordVideoBtn;//开始录制视频的按钮
    QPushButton *stopRecordVideoBtn;//停止录制

    QTimer *timer;//定时获取视频帧
};

#endif // VIDEOMONITORWIDGET_H
