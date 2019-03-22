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
#include <QDir>
#include "mymessagebox.h"

#ifdef VIDEO_CAPTURE
#include "videocapture.h"//视频采集处理类
#endif

class VideoMonitorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VideoMonitorWidget(QWidget *parent = 0);
    ~VideoMonitorWidget();

protected:
    virtual void paintEvent(QPaintEvent *event);

signals:
    /*开始与结束录制的信号　用来关联子线程videoCapture的槽，这样视频编码保存的处理均在
     * 子线程中完成。如果直接在主线程调用videoCapture编码保存视频的方法,那么即便videoCapture
     * 被move到子线程，编码保存处理还是在主线程，引起界面卡顿。
     */
    void startRecordSignal();
    void stopRecordSignal();

public slots:
    void startRecordSlot();//开始录制　会进行视频编码操作保存成文件
    void stopRecordSlot();//停止录制
#ifdef VIDEO_CAPTURE
    void getFrameSlot();//获取采集的视频帧
#endif

private:
#ifdef VIDEO_CAPTURE
    QThread *threadB;//视频采集放在线程中进行
    VideoCapture *videoCapture;//视频采集对象
    bool initStatus;//视频设备初始化状态
    uchar *rgbFrame;//RGB格式图像的起始地址
    QImage *image;//QImage图像
    QTimer *timer;//定时获取视频帧，同时也用来定时检测usb摄像设备的插拔状态
#endif

    QLabel *videoOutput;//展示视频画面
    QPushButton *startRecordVideoBtn;//开始录制视频的按钮
    QPushButton *stopRecordVideoBtn;//停止录制

};

#endif // VIDEOMONITORWIDGET_H
