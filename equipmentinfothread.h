/*
 *@file:   equipmentinfothread.h
 *@author: 缪庆瑞
 *@date:   2017.3.16
 *@brief:  获取设备信息参数的线程，实际该类是一个继承QObject实现TCPServer
 *的子类，使用movetothread()使其运行在子线程中。关于线程的使用方法可参考
 *网络上“QT开发者关于QThread的咆哮”文章，讲述了QThread实际应该怎么用。
 */
#ifndef EQUIPMENTINFOTHREAD_H
#define EQUIPMENTINFOTHREAD_H

#include <QObject>
#include <QtNetwork>
#include <QTimer>
#include <QList>

class EquipmentInfoThread : public QObject
{
    Q_OBJECT
public:
    explicit EquipmentInfoThread(QObject *parent = 0);
    ~EquipmentInfoThread();
    //初始化服务器定时向连接的客户端发送请求数据的指令包,指令数据相同
    void initRequestData();
    //数据在网络传输中，通过公司内部的HDLC协议对其处理
    QByteArray dataToHDLC(QByteArray rawData);
    void HDLCToData(QByteArray &encipheredData);
    QStringList convertReceiveMessage(QByteArray &array);

signals:
    void equipmentDataSignal(QStringList);
    void disconnectSignal(QString ipAddr);//客户端断开连接信号

public slots:
    void getClientSocket();//获取连接到服务器的QTcpSocket 即客户端创建的套接字
    void receiveMessage();//服务器接收客户端的消息
    void sendMessage();//服务器发送请求数据的消息
    void sendMessageData(QStringList list);//服务器回复数据
    void disconnectSlot();//对客户端断开连接的信号处理

private:
    QTcpServer *tcpServer;
    QList<QTcpSocket *> clientConnectionList;//客户端创建的套接字链表，因为可能多个客户端同时连接
    QTimer *timer;//定时器，用来让服务器定时向客户端请求数据
    quint8 nextBlockSize;
    QByteArray requestData;
};

#endif // EQUIPMENTINFOTHREAD_H
