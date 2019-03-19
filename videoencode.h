/*
 *@file:   videoencode.h
 *@author: 缪庆瑞
 *@date:   2017.5.12
 *@brief:  通过ffmpeg相关视频编解码库实现对原始数据帧的编码，输出视频文件，使用了avcodec
 * 等相关的视频编解码库，需要包含相关的头文件，编译时也要在pro文件中添加相应的库，例、
 * LIBS += -L./ffmpeg/lib -lavcodec
 */
#ifndef VIDEOENCODE_H
#define VIDEOENCODE_H

#include <QObject>
#include <QDebug>
#include <QTime>
#include <stdio.h>

/*编解码库的头文件，以C语言引入，避免因为C与C++ 编译后的函数名不一致，无法
链接*/
extern "C"
{
#include "ffmpeg/include/avcodec.h"
#include "ffmpeg/include/avformat.h"
#include "ffmpeg/include/swscale.h"
}

class VideoEncode : public QObject
{
    Q_OBJECT
public:
    explicit VideoEncode(QObject *parent = 0);
    ~VideoEncode();

    //转换yuyv422格式帧为yuv420p
    void yuyv422_to_yuv420p(AVFrame *dst, const AVFrame *src,int width, int height);
    uchar *YUYV422ToYUV420P(uchar *YUV422, uchar *YUV420, int width, int height);
    void initVideoProcess(QByteArray filename);//初始化保存视频的相关处理
    //编码保存视频帧
    void videoProcess(uchar *YUV420);
    //结束视频保存
    void finishVideoProcess();

signals:

public slots:

private:
    //uchar *YUV420;
    AVCodecContext *codecContext;//描述编解码器上下文
    AVFormatContext *formatContext;//解封装
    AVStream *stream;//存储视频流信息
    AVFrame *rawFrame;//存储原始数据信息
    uchar *rawFrame_buf;//原始帧内存地址
    uchar *video_outbuf;//转换格式后的原始帧的输出缓冲区
    int video_outbuf_size;//申请的输出缓冲区空间大小
    int real_outbuf_size;//转换格式后实际原始帧大小
    int pixelSize;
    //double video_pts;//时间戳
    //YUYV422ToYUV420P()转换函数中的变量
    uint8_t *data_srcBuf;//原yuyv422数据帧内存地址
    uint8_t *data_dstBuf;//转换后的yuv420p数据帧内存地址
    AVFrame *srcBuf ;//视频帧，除了包含采集的数据帧(data_srcBuf)还有其他信息
    AVFrame *dstBuf;

};

#endif // VIDEOENCODE_H
