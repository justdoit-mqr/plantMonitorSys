/*
 *@file:   equipmentinfothread.h
 *@author: 缪庆瑞
 *@date:   2017.3.16
 *@brief:  获取设备信息参数的线程，实际该类是一个继承QObject实现TCPServer
 *的子类，使用movetothread()使其运行在子线程中。关于线程的使用方法可参考
 *网络上“QT开发者关于QThread的咆哮”文章，讲述了QThread实际应该怎么用。
 */
#include "equipmentinfothread.h"
#include <QDebug>

EquipmentInfoThread::EquipmentInfoThread(QObject *parent) : QObject(parent)
{
    nextBlockSize=0;
    timer = new QTimer(this);
    tcpServer = new QTcpServer(this);
    if(!tcpServer->listen(QHostAddress::Any,6666))//监听所有地址,6666端口的客户端连接
    {
        qDebug()<<tcpServer->errorString();
        return ;
    }
    initRequestData();
    //一旦客户端发起连接，服务器会发射newconnection()信号
    connect(tcpServer,SIGNAL(newConnection()),this,SLOT(getClientSocket()));
    connect(timer,SIGNAL(timeout()),this,SLOT(sendMessage()));//连接定时器的槽
}

EquipmentInfoThread::~EquipmentInfoThread()
{

}
/*
 *@brief:   初始化服务器定时发送的请求数据的指令包，因为请求指令一直相同
 *  所以，数据只初始化一次就行。
 *@author:  缪庆瑞
 *@date:    2017.3.22
 */
void EquipmentInfoThread::initRequestData()
{
    QByteArray rawData;
    //添加原始数据
    //服务器请求数据的指令包，按照公司HDLC协议封装，其中数据字段针对公司项目具体参数，对该项目没用
    {
        rawData.append(0x7e);//包头
        rawData.append(0x1f);
        rawData.append(0x01);//控制字段，0x01表示请求数据，0x02表示回复数据
        rawData.append(0x01);//数据字段
        rawData.append((char)0x00);
        rawData.append(0x01);
        rawData.append(0xfe);
        int checkSum=0x1f+0x01+0x01+0x00+0x01+0xfe;//计算校验和
        rawData.append(checkSum&0xff);//校验和，除去包头包尾所有数据相加取低8位
        rawData.append(0x7d);//包尾
    }
    requestData=dataToHDLC(rawData);
    requestData.prepend(requestData.size());//存放数据的个数
}
/*
 *@brief:   将发送的原始数据经过HDLC协议封装
 *@author:  缪庆瑞
 *@date:    2017.3.22
 *@return:  QByteArray：经HDLC封装的字节数组
 */
QByteArray EquipmentInfoThread::dataToHDLC(QByteArray rawData)
{
    QByteArray encipheredData;
    encipheredData.append(0x7E);//添加HDLC开始标志
    for(int i=1; i<rawData.size()-1; i++)
    {
        //数据字节中出现标志字段则需要做处理，具体根据公司HDLC协议标准
        switch (rawData[i])
        {
        case 0x7E:
        case 0x00:
        case 0x7C:
        case 0x7D:
            encipheredData.append(0x7C);
            encipheredData.append(rawData[i]^0x20);
            break;
        default: {encipheredData.append(rawData[i]); break;}
        }
    }
    encipheredData.append(0x7D);//添加HDLC结束标志
    return encipheredData;
}
/*
 *@brief:   将接收的HDLC协议封装的数据解析为原始数据
 *@author:  缪庆瑞
 *@date:    2017.3.22
 */
void EquipmentInfoThread::HDLCToData(QByteArray &encipheredData)
{
    for(int i=1;i<encipheredData.size()-1;i++)
    {
        if(encipheredData[i]==(char)0x7C)
        {
            encipheredData.remove(i,1);//删除原数据中增加的数据
            encipheredData[i]=encipheredData[i]^0x20;
        }
    }
}
/*
 *@brief:   转换服务器接收到的字节信息，变为QString类型的直观数据
 *@author:  缪庆瑞
 *@date:    2017.3.23
 */
QStringList EquipmentInfoThread::convertReceiveMessage(QByteArray &array)
{
    //将服务器接收到的信息转换为QString类型的直观数据，主要包括以下五种
    QString ipAddr;
    QString voltage;
    QString current;
    QDate date;
    QTime time;

    QStringList list;

    quint8 ipSegment[4];//ip段
    for(int i=0;i<4;i++)
    {
        //qDebug()<<(int)array.at(i+3);
        //获取ip段 将char强制转换uint,最高位为1时表负值，此时转换值有问题
        //ipSegment[i] = (uint)array.at(i+3);
        ipSegment[i] = (quint8)array.at(i+3);//获取ip段
        ipAddr.append(QString::number(ipSegment[i]));
        if(i!=3)
        {
            ipAddr.append(".");
        }
    }
    //qDebug()<<ipAddr;
    quint8 voltageH = (quint8)array.at(7);
    quint8 voltageL = (quint8)array.at(8);
    voltage = QString::number(((voltageH<<8)+voltageL)/10.0,'f',1);
    quint8 currentH = (quint8)array.at(9);
    quint8 currentL = (quint8)array.at(10);
    current = QString::number((currentH<<8)+currentL);

    quint8 yearH = (quint8)array.at(11);
    quint8 yearL = (quint8)array.at(12);
    int year = (yearH<<8)+yearL;
    date.setDate(year,(quint8)array.at(13),(quint8)array.at(14));//设置日期
    time.setHMS((quint8)array.at(15),(quint8)array.at(16),array.at(17));//设置时间

    list.append(ipAddr);
    list.append(voltage);
    list.append(current);
    list.append(date.toString("yyyy-MM-dd"));
    list.append(time.toString("hh:mm:ss"));
    return list;
}
/*
 *@brief:   获取连接到服务器的QTcpSocket
 *@author:  缪庆瑞
 *@date:    2017.3.22
 */
void EquipmentInfoThread::getClientSocket()
{
    //qDebug()<<"getClient:"<<this->thread();
    //得到最近连接到的QTcpSocket 即客户端创建的套接字
    QTcpSocket *clientConnection = tcpServer->nextPendingConnection();
    clientConnectionList.append(clientConnection);
    connect(clientConnection,SIGNAL(disconnected()),this,SLOT(disconnectSlot()));
    connect(clientConnection,SIGNAL(readyRead()),this,SLOT(receiveMessage()));
    if(!timer->isActive())
    {
        timer->start(1000);
    }
}
/*
 *@brief:   服务器接收客户端的消息
 *@author:  缪庆瑞
 *@date:    2017.3.22
 */
void EquipmentInfoThread::receiveMessage()
{
    /*readyRead()信号的响应槽，该信号在QIODevice类中，QTcpSocket继承了该信号，每当有新的
    数据可以从设备(这里是Socket)中读时，就会发出该信号。对于socket而言，往socket中写数据
    的write()方法将数据写到了内存缓冲区，然后经由事件循环获得控制权后将缓冲区数据写到socket中，
    这时才发送readyread()信号,所以如果write()操作很频繁的话，并不是每次write都会触发一次readyread()
    信号,具体多久触发一次依赖于网络和机器性能。因此当客户端向服务器连续（时间间隔较小）发送
    数据包，可能发送多个数据包才会触发一次信号，这样服务器处理显示的就不是最新的数据了。

    经测试发现，以目前的机器1s发送(write())一次数据，readyread()信号正常1s触发一次，是不会出现
    上述问题的,但有一个现象是当鼠标在pc客户端程序标题栏按下时，此时服务器能正常向客户端写数据
    但客户端的readyread()信号的槽得不到响应，不知是没发射信号还是发射的信号进入循环事件队列
    等待响应？而当释放鼠标时，基本上都会连续发送两次readyread()信号(还不清楚为什么)，这样就
    造成了上面说的连续发送数据包，导致服务器端只触发一次readyread信号。*/
    //获取发送readyRead()信号的对象
    QTcpSocket *clientConnection = qobject_cast<QTcpSocket *>(sender());
    quint8 temp;//从套接字中每次读一个字节临时存放
    QByteArray encipheredData;//存储接收到的经过HDLC封装的数据
    QDataStream in(clientConnection);
    //这里数据流中都是普通类型的字节，没有存放Qt相关的类型，不需要设置版本号
    //in.setVersion(QDataStream::Qt_4_6);
    if(nextBlockSize == 0)
    {
        //传输的数据第一个字节存放数据块大小
        if(clientConnection->bytesAvailable()< 1)
        {
            return;
        }
        in >> nextBlockSize;
    }
    //qDebug()<<"available:"<<clientConnection->bytesAvailable()<<"current:"<<nextBlockSize;
    //如果socket中可读的字节小于数据块的大小，则表明数据包还没有完整，不处理继续接收
    if(clientConnection->bytesAvailable() < nextBlockSize)
    {
        return;
    }
    //从socket中取出一个数据包,
    for(int i=0;i<nextBlockSize;i++)
    {
        in >> temp;
        encipheredData.append(temp);
    }
    nextBlockSize = 0;//接收到完整的块后，将下一个未知块大小设置为0
    /*update 2017.4.7
    这里的while()循环就是为了解决上述readyread()信号的问题，因为是实时显示，
    所以每次只处理最新的一个完整数据包，旧的数据包丢弃，如果旧数据包需要保存的话
    则在下面注释的位置加上部分代码即可*/
    while(true)
    {
        //socket中仅仅有一个已被接收的完整数据包，退出循环去处理接收的完整数据包
        if(clientConnection->bytesAvailable()< 1)
        {
            break;
        }
        else //除了被接收的数据包外，还有可用数据
        {
            in >> nextBlockSize;//获取下一个数据包的大小
            //如果下一个数据包不完整，则退出循环去处理接收的完整数据包，
            if(clientConnection->bytesAvailable() < nextBlockSize)
            {
                break;
            }
            //如果下一个数据包完整
            else
            {
                /*如果每一个到来的正确数据包都要存数据库，可以在这里加上对HDLC包头包尾
                 的判断,正确则存入数据库*/
                encipheredData.clear();//清空上一个接收的数据包
                //从socket中再次取出一个数据包,
                for(int i=0;i<nextBlockSize;i++)
                {
                    in >> temp;
                    encipheredData.append(temp);
                }
                nextBlockSize = 0;//接收到完整的块后，将下一个未知块大小设置为0
            }
        }
    }
    //qDebug()<<"available2:"<<clientConnection->bytesAvailable()<<"current2:"<<nextBlockSize;
    //判断HDLC包头包尾是否出错
    if(encipheredData.at(0)==(char)0x7E&&encipheredData.at(encipheredData.size()-1)==(char)0x7D)
    {
        HDLCToData(encipheredData);
        int checkSum=0;//校验和
        int num=encipheredData.size();//处理后数据的长度
        /* 注意QByteArray的at()方法返回的是有符号char类型,最好在执行加法运算之前
         * 转换成无符号char。不然如果发送方采用无符号类型计算校验和，而接收方采用
         * 有符号类型计算校验和，两者最终求的checkSum的原码值(int 类型)会不一样。但
         * 由于计算机采用补码进行加减运算，两者求的checkSum的补码的低8位是相同的。
         * 所以这里不转成quint8也不会出错，只是转换成无符号计算更容易理解
         */
        for(int j=1;j<num-2;j++)
        {
            checkSum+=(quint8)encipheredData.at(j);//对接收的数据计算校验和
        }
        /* 如果校验和正确，控制命令为回复数据命令，则将数据处理后显示
         * 因为at()方法默认返回有符号char，最高位为1时则为负值，而int型的校验和
         * 取低8位为正值，故需要转换成unsigned char
         */
        if((quint8)(checkSum&0xff)==(quint8)encipheredData.at(num-2)&&encipheredData.at(2)==0x02)
        {
            /*客户端发送的数据经过HDLC协议的封装，具体格式如下：
             包头+地址字段+控制字段+客户端ip字段+电压+电流+日期(年+月+日+时+分+秒)+包尾
             包头：0x7e；地址地段：表示客户端地址，暂时用不到用一个字节表示；
             控制字段：0x02表示客户端回复的数据；客户端ip字段：四个字节表示，模拟设备地址
             电压：客户端设备电压，高低两字节；电流：客户端设备电压，高低两字节
             日期：对应数据的产生时间，年为高低两字节，其他1字节
             包尾：0x7d
             */
            //qDebug()<<tr("服务器接收到客户端回复的实时数据：");
            //qDebug()<<encipheredData.toHex();
            QStringList list = convertReceiveMessage(encipheredData);
            //update 2017.4.1
            //给客户端设备的套接字objectname设置为模拟ip地址，用来判断哪个客户端断开了连接
            clientConnection->setObjectName(list.at(0));
            //qDebug()<<list;
            emit equipmentDataSignal(list);//将设备信息以信号的形式发出去
        }
        //update 2017.6.1 时间校准
        else if((quint8)(checkSum&0xff)==(quint8)encipheredData.at(num-2)&&encipheredData.at(2)==0x03)
        {
            /*客户端发送的数据经过HDLC协议的封装，具体格式如下：
             包头+地址字段+控制字段+日期(年+月+日+时+分+秒)+包尾
             包头：0x7e；地址地段：表示客户端地址，暂时用不到用一个字节表示；
             控制字段：0x03表示客户端回复校准时间数据；
             日期：对应数据的产生时间，年为高低两字节，其他1字节
             包尾：0x7d
             */
            //qDebug()<<tr("服务器接收到客户端回复的实时数据：");
            //qDebug()<<encipheredData.toHex();
            QDate date;
            QString dateStr;
            QTime time;
            QString timeStr;
            quint8 yearH = (quint8)encipheredData.at(3);
            quint8 yearL = (quint8)encipheredData.at(4);
            int year = (yearH<<8)+yearL;
            date.setDate(year,(quint8)encipheredData.at(5),(quint8)encipheredData.at(6));//设置日期
            dateStr = date.toString("yyyy-MM-dd");
            time.setHMS((quint8)encipheredData.at(7),(quint8)encipheredData.at(8),encipheredData.at(9));//设置时间
            timeStr = time.toString("hh:mm:ss");
            QString setTimeStr("date -s \""+dateStr+" "+timeStr+"\"");
            qDebug()<<"setTimeStr:";//<<setTimeStr<<dateStr<<timeStr;
            system(setTimeStr.toUtf8().data());//设置时间
        }
        else
        {
            qDebug()<<"checkSum is error Or control cmd is error!!"
                   <<tr("校验和不正确或控制命令非回复数据命令");
        }
    }
    else
    {
        qDebug()<<"HDLC Baghead Or Bagtrail is error!!"
                <<tr("接收的HDLC包头包尾数据错误");
    }
    //nextBlockSize = 0;//接收到完整的块后，将下一个未知块大小设置为0
}
/*
 *@brief:   服务器向客户端发送消息  请求数据的命令
 *@author:  缪庆瑞
 *@date:    2017.3.22
 */
void EquipmentInfoThread::sendMessage()
{
    //遍历所有连接的客户端socket，定时发送请求数据命令
    for(int i=0;i<clientConnectionList.size();i++)
    {
        if(clientConnectionList.at(i)!= NULL)//已被清理的无效socket就不再发送数据了
        {
            //这里要注意被清理内存的对象指针一定要置NULL，否则野指针执行下面这句话就会引起异常
            clientConnectionList.at(i)->write(requestData);
            //clientConnectionList.at(i)->flush();
        }
    }
}
/*
 *@brief:   服务器向客户端发送消息  预置电流电压值，模拟控制
 *@author:  缪庆瑞
 *@date:    2017.4.5
 *@param:   list:包含 IpAddr，presetA，presetV
 */
void EquipmentInfoThread::sendMessageData(QStringList list)
{
    //使用系统临时double类型直接转换成int会丢失精度，需要定义double变量转换
    double presetV = list.at(2).toDouble()*10.0;
    uint presetVoltage = (uint)presetV;//通过变量再强制转换类型
    //qDebug()<<(uint)(list.at(2).toDouble()*10)<<presetVoltage;
    quint8 presetVoltageH = ((presetVoltage>>8)&0xff);//预置电压高字节;
    quint8 presetVoltageL = presetVoltage&0xff;
    uint presetCurrent = list.at(1).toInt();
    quint8 presetCurrentH =  ((presetCurrent>>8)&0xff);//预置电流高字节;;
    quint8 presetCurrentL = presetCurrent&0xff;
    QByteArray rawData;
    //添加原始数据
    //服务器请求数据的指令包，按照公司HDLC协议封装，其中数据字段针对公司项目具体参数，对该项目没用
    {
        rawData.append(0x7e);//包头
        rawData.append(0x1f);
        rawData.append(0x02);//控制字段，0x01表示请求数据，0x02表示回复数据
        rawData.append(presetVoltageH);//预置电压H/L
        rawData.append(presetVoltageL);
        rawData.append(presetCurrentH);//预置电流H/L
        rawData.append(presetCurrentL);
        int checkSum=0x1f+0x02+presetVoltageH+presetVoltageL+presetCurrentH+presetCurrentL;//计算校验和
        rawData.append(checkSum&0xff);//校验和，除去包头包尾所有数据相加取低8位
        rawData.append(0x7d);//包尾
    }
    QByteArray replayData=dataToHDLC(rawData);
    replayData.prepend(replayData.size());//存放数据的个数
    //遍历所有连接的客户端socket，根据objectname的ip确定回复给哪个客户端
    for(int i=0;i<clientConnectionList.size();i++)
    {
        //已被清理的无效socket就不再发送数据了,根据objectname的ip确定回复给哪个客户端
        if(clientConnectionList.at(i)!= NULL&&clientConnectionList.at(i)->objectName()==list.at(0))
        {
            //这里要注意被清理内存的对象指针一定要置NULL，否则野指针执行下面这句话就会引起异常
            clientConnectionList.at(i)->write(replayData);
        }
    }
}
/*
 *@brief:   客户端断开连接的信号槽
 *@author:  缪庆瑞
 *@date:    2017.3.22
 */
void EquipmentInfoThread::disconnectSlot()
{
    //获取发送disconnect信号的对象
    QTcpSocket *clientConnection = qobject_cast<QTcpSocket *>(sender());
    emit disconnectSignal(clientConnection->objectName());//发送断开连接信号，让主线程知道

    for(int i=0;i<clientConnectionList.size();i++)
    {
        if(clientConnection == clientConnectionList.at(i))
        {
            /*因为后面要delete无效的对象，释放内存空间，但指向该对象的指针需要手动置NULL，否则
            指针还是指向之前的地址，不小心调用就会程序异常,at(i)返回const指针无法置NULL，故使用
            [i]来置null,这里除了置空外也可以将无效指针从链表移除*/
            clientConnectionList[i] = NULL;
        }
    }
    clientConnection->deleteLater();//删除无效的对象，清理内存占用
}
