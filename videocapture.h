//videocapture.h
#ifndef VIDEOCAPTURE_H
#define VIDEOCAPTURE_H

#include <QObject>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>//v4l2驱动程序的头文件
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QTimer>
#include <QDateTime>
#include <QMutex>//用于线程的互斥锁
#include <QMutexLocker>
/*该头文件用于视频保存操作，只需要实时显示不保存文件的话就不需要，因为视频保存操作
 涉及帧的编码操作，需要一些库的支持*/
#include "videoencode.h"

#define BUFFERCOUNT (3)//缓冲区的数量，视频帧按队列顺序刷新，太多的话可能会造成视频有延迟
typedef unsigned int uint;
typedef unsigned char uchar;

class VideoCapture:public QObject
{
	Q_OBJECT
public:
    //视频采集相关函数
	VideoCapture();
    ~VideoCapture();
    bool initGlobal();//对视频采集的整体初始化
    bool openDevice();
	void initDevice();
	void initMmap();
	void start();
	void stop();
    void getFrame(void *rgbFrameAddr);
    void unGetFrame();
	void unInitMmap();
	void closeDevice();

    bool getVideoEqmExistStatus();

    //视频帧格式转换
    int convert_yuv_to_rgb_pixel(int y,int u,int v);
    void convert_yuv_to_rgb_buffer(uchar *yuv,uchar *rgb,uint width,uint height);

    //视频保存相关函数
//    void startRecord();
//    void stopRecord();

    bool unMmap;//表示是否需要主线程卸载内存映射

public slots:
    void getAndSaveFrame();
    //void getFrame(void *rgbFrameAddr);
    //视频保存相关函数
    void startRecord();
    void stopRecord();
    void hotplugSlot();

private:
	/*定义结构体来映射每一个缓冲帧*/
	struct buffer
	{
		void * addr;//缓冲帧映射到内存中的起始地址
		uint length;//缓冲帧映射到内存中的长度
	}*bufPtr;
	int fd;//设备文件描述符
	uint numBuf;//缓冲帧序号
    int index;//标记缓冲区序号

    void *yuvFrameAddr;

    QTimer *hotplugTimer;//定时获取采集设备的状态，实现热插拔
    bool videoEqmExist;//标记设备是否存在
    char videoEqmName[30];//视频设备文件名称及路径

    //视频保存
    VideoEncode *videoEncode;//视频编码操作保存文件的对象
    QTimer *timer;//定时编码保存视频帧
    void *yuv422Frame;
    uchar *yuv420;
    uchar *yuv420Temp;

    QMutex mutex;
};
#endif //VIDEOCAPTURE_H

