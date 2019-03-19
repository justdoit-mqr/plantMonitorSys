/*
 *@file:   videomonitorwidget.h
 *@author: 缪庆瑞
 *@date:   2017.5.10
 *@brief:  显示实时监控视频的类，该类主要调用VideoCapture类的方法完成视频帧的采集，然后
 * 对采集到的帧格式处理成图片显示。
 */

#include "videomonitorwidget.h"
#include <QGridLayout>
#include <QTime>

#define IMAGEWIDTH (320)
#define IMAGEHEIGHT (240)
#define IMAGEFORMAT QImage::Format_RGB888

VideoMonitorWidget::VideoMonitorWidget(QWidget *parent) : QWidget(parent)
{
    videoCapture = new VideoCapture();//视频采集对象
    threadB = new QThread();//视频采集保存单开一个线程
    videoCapture->moveToThread(threadB);//通过该操作将videoCapture对象的操作放在了子线程中
    //qDebug()<<"sub thread:"<<videoCapture->thread();
    connect(this,SIGNAL(startRecordSignal()),videoCapture,SLOT(startRecord()));
    connect(this,SIGNAL(stopRecordSignal()),videoCapture,SLOT(stopRecord()));
    threadB->start();
    initStatus = false;//视频设备初始化状态

    rgbFrame = (uchar *)malloc(IMAGEWIDTH*IMAGEHEIGHT*3*sizeof(char));//为图像分配内存空间
    image = new QImage(rgbFrame,IMAGEWIDTH,IMAGEHEIGHT,IMAGEFORMAT);//根据内存空间创建图像

    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(getFrameSlot()));
    timer->start(100);

    videoOutput = new QLabel();//展示视频画面
    videoOutput->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    videoOutput->setFont(QFont("msyh",30));
    videoOutput->setAlignment(Qt::AlignCenter);
    videoOutput->setScaledContents(true);//按照label的大小放缩视频
    videoOutput->setStyleSheet("border:2px solid black;");
    QString styleSheetString("font: 20px \"msyh\";");
    startRecordVideoBtn = new QPushButton(tr("开始录制"),this);
    startRecordVideoBtn->setStyleSheet(styleSheetString);
    startRecordVideoBtn->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    connect(startRecordVideoBtn,SIGNAL(clicked()),this,SLOT(startRecordSlot()));
    stopRecordVideoBtn = new QPushButton(tr("结束录制"),this);
    stopRecordVideoBtn->setStyleSheet(styleSheetString);
    stopRecordVideoBtn->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    stopRecordVideoBtn->setEnabled(false);
    connect(stopRecordVideoBtn,SIGNAL(clicked()),this,SLOT(stopRecordSlot()));

    QGridLayout *gridLayout = new QGridLayout(this);
    gridLayout->setMargin(1);
    gridLayout->addWidget(videoOutput,0,0,5,3);
    gridLayout->addWidget(startRecordVideoBtn,1,3,1,1);
    gridLayout->addWidget(stopRecordVideoBtn,3,3,1,1);
}

VideoMonitorWidget::~VideoMonitorWidget()
{
    threadB->quit();
    threadB->deleteLater();
    videoCapture->deleteLater();
}
/*
 *@brief:   绘图事件处理函数，用来自动刷新视频帧
 *@author:  缪庆瑞
 *@date:    2017.5.10
 *@param:   event：绘图事件
 */
void VideoMonitorWidget::paintEvent(QPaintEvent *event)
{
    //qDebug()<<"paintevent"<<QTime::currentTime().toString("hh:mm:ss:zzz");
    //判断视频设备是否存在，每次设备被拔出后下次需要重新初始化设备
    if(!videoCapture->getVideoEqmExistStatus())
    {
        if(videoCapture->unMmap)//判断是否需要主线程卸载视频采集设备的内存映射
        {
            videoCapture->unInitMmap();
            videoCapture->unMmap = false;
        }
        initStatus = false;
    }
    if(!initStatus)//设备尚未初始化
    {
        if(!videoCapture->initGlobal())//设备初始化失败
        {
            videoOutput->setText(tr("未检测到\n视频采集设备!!"));
            startRecordVideoBtn->setEnabled(false);
            stopRecordVideoBtn->setEnabled(false);
            return;
        }
        else //设备初始化完成
        {
            initStatus = true;
            videoCapture->start();//开启视频采集
            startRecordVideoBtn->setEnabled(true);
            stopRecordVideoBtn->setEnabled(false);
        }
    }
//    videoCapture->getFrame((void *)rgbFrame);//获取一帧RGB格式图片流
//    image->loadFromData((uchar *)rgbFrame,IMAGEWIDTH*IMAGEHEIGHT*3*sizeof(char));//rgb格式图片流生成QImage图片
//    videoOutput->setPixmap(QPixmap::fromImage(*image));//屏幕显示
    //videoCapture->unGetFrame();//处理完一帧后，将一个空缓冲区放入队列(实现循环队列)，避免缓冲区被取完
    QWidget::paintEvent(event);
}
/*
 *
 *@brief:   开始视频录制
 *@author:  缪庆瑞
 *@date:    2017.5.12
 */
void VideoMonitorWidget::startRecordSlot()
{
    QDir dir("/mnt/mtd/video");//视频的保存路径，实际使用网络挂载
    //system("umount /mnt/mtd");//先卸载挂载
    if(system("ping 192.168.43.112 -w 2"))//测试网络是否通，-w 2表示两秒后自动退出，通返回０
    {
        MyMessageBox::myInformation(this,"Information",tr("请确定已成功挂载\n外部存储介质？"));
        return;
    }
    system("mount -t nfs -o nolock 192.168.43.112:/root /mnt/mtd");//执行成功返回０
    if(!dir.exists())//视频路径不存在，即没挂载上
    {
        MyMessageBox::myInformation(this,"Information",tr("请确定已成功挂载\n外部存储介质？"));
        return;
    }
    startRecordVideoBtn->setText(tr("正在录制..."));
    startRecordVideoBtn->setEnabled(false);
    stopRecordVideoBtn->setEnabled(true);
    /*此时videoCapture对象的操作在子线程中进行，但如果在主线程中通过对象直接调用方法(此处
    为startRecord()),那么该方法实际是在主线程中执行的，所以就会遇到一个问题：即videoCapture中
    有一个QTimer定时器，连接了信号与槽。因为videoCapture在子线程中，自然QTimer也在子线程中
    但如果通过此处直接调用startRecord(),会发生在主线程中启动定时器，这样在子线程中的槽函数就不会
    响应*/
    //videoCapture->startRecord();
    //通过发送信号的形式，激发子线程的槽函数，这样槽函数也是在子线程中执行的
    emit startRecordSignal();
}
/*
 *@brief:   停止视频录制
 *@author:  缪庆瑞
 *@date:    2017.5.12
 */
void VideoMonitorWidget::stopRecordSlot()
{
    stopRecordVideoBtn->setEnabled(false);
    startRecordVideoBtn->setText(tr("开始录制"));
    startRecordVideoBtn->setEnabled(true);
    //videoCapture->stopRecord();
    emit stopRecordSignal();
}

void VideoMonitorWidget::getFrameSlot()
{
    //qDebug()<<"getFrame:"<<QTime::currentTime().toString("hh:mm:ss:zzz");
    if(initStatus&&this->isVisible())//仅当设备完成初始化且当前界面被展示才获取界面
    {
        videoCapture->getFrame((void *)rgbFrame);//获取一帧RGB格式图片流
        image->loadFromData((uchar *)rgbFrame,IMAGEWIDTH*IMAGEHEIGHT*3*sizeof(char));//rgb格式图片流生成QImage图片
        videoOutput->setPixmap(QPixmap::fromImage(*image));//屏幕显示
    }
    update();
    //repaint();//立即刷新界面
}
