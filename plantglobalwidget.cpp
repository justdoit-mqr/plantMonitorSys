/*
 *@file:   plantglobalwidget.h
 *@author: 缪庆瑞
 *@date:   2017.3.17
 *@brief:  车间概况总汇部件
 */
#define BTNWIDTH 88
#define BTNHEIGHT 65
#define DATABASE_NAME "./database/plantManager.db"
#include "plantglobalwidget.h"
#include <QGridLayout>
#include <QStyleFactory>

PlantGlobalWidget::PlantGlobalWidget(QWidget *parent) : QWidget(parent)
{
    plantAreaA = new QGroupBox(tr("A区:"),this);
    //plantAreaA->setAutoFillBackground(true);
    //plantAreaA->setStyle(QStyleFactory::create("Windows"));//使用windows风格的样式，在arm上可以不用
    plantAreaB = new QGroupBox(tr("B区:"),this);
    //plantAreaB->setAutoFillBackground(true);
    //plantAreaB->setStyle(QStyleFactory::create("Windows"));
    hBoxLayoutA = new QHBoxLayout(plantAreaA);
    hBoxLayoutA->setContentsMargins(4,1,1,1);
    hBoxLayoutB = new QHBoxLayout(plantAreaB);
    hBoxLayoutB->setContentsMargins(4,1,1,1);

    initStatusGroupAreaC();//初始化设备状态展示组
    initEquipmentNoBtn();//初始化设备号按钮
    //初始化添加按钮的对话框，在信号响应槽中只显示即可
    addEquipmentDialog = new AddEquipment(this);
    addEquipmentDialog->setWindowTitle(tr("添加设备"));
    connect(addEquipmentDialog,SIGNAL(addSuccessSignal()),this,SLOT(updateTable()));

    QGridLayout *gridLayout = new QGridLayout(this);
    gridLayout->setContentsMargins(1,1,1,1);
    gridLayout->setSpacing(0);
    gridLayout->addWidget(plantAreaA,0,0,1,1);
    gridLayout->addWidget(plantAreaB,0,1,1,1);
    gridLayout->addWidget(statusGroupAreaC,1,0,1,2);
    gridLayout->setRowStretch(0,10);//设置布局的比例关系
    gridLayout->setRowStretch(1,1);

    qDebug()<<"main:"<<this->thread();//主线程
    //开启设备信息采集线程
    equipmentInfothread = new EquipmentInfoThread();//移到子线程的对象不可以指定父对象
    threadA = new QThread();
    equipmentInfothread->moveToThread(threadA);
    /*这里信号的发送者在次线程，接收者在主线程，不在同一线程，connect()第5个自动参数默认Qt::AutoConnection会使用
    Qt::QueuedConnection排队方式进入事件队列，异步处理。这时就要求信号槽队列中的数据类型必须时系统能识别的元类型
     否则就需要使用qRegisterMetaType()注册，或者第五个参数使用Qt::DirectConnection，但不安全*/
    //qRegisterMetaType<QList<QStringList> >("QList<QStringList>");//不太明白qt的元对象，但加上这句话线程间的信号槽数据确实就可以传递了
    connect(equipmentInfothread,SIGNAL(equipmentDataSignal(QStringList)),this,SLOT(handleEquipmentInfoSlot(QStringList)));
    connect(equipmentInfothread,SIGNAL(disconnectSignal(QString)),this,SLOT(disconnectSlot(QString)));//客户端断开连接信号槽
    connect(this,SIGNAL(replyDataToClient(QStringList)),equipmentInfothread,SLOT(sendMessageData(QStringList)));
    threadA->start();

}

PlantGlobalWidget::~PlantGlobalWidget()
{
    threadA->quit();//程序关闭后保证线程退出，quit为退出事件循环，自然也就退出了run()
    threadA->deleteLater();
    equipmentInfothread->deleteLater();
}
/*
 *@brief:   初始化设备号按钮，从数据库中读，将按钮初始化到表格中,
 *@author:  缪庆瑞
 *@date:    2017.3.17
 *@update:  2017.3.28 界面重构
 */
void PlantGlobalWidget::initEquipmentNoBtn()
{
    //连接数据库
    if(createConnection(DATABASE_NAME,"connection1"))
    {
        QStringList list;//数据库中所有设备号的链表
        //数据库连接成功，执行查询命令，根据数据库中的信息，初始化界面的设备按钮
        QSqlQuery query(db);
        //通过数据库构建A区部件
        if(!query.exec("select No,Name from equipmentInfo where Area = 'A'"))
        {
            qDebug()<<query.lastError().text();
        }
        while(query.next())
        {
           QPushButton *equipmentNoA = new QPushButton(query.value(1).toString(),this);
           list.append(query.value(0).toString());
           equipmentNoA->setObjectName(query.value(0).toString());//根据设备独一无二的号设置objName
           equipmentNoA->setStyleSheet(statusColor.at(0));//初始化设备离线状态
           equipmentNoA->setFixedHeight(BTNHEIGHT-16);
           itemsA.append(new BaseTableItem(BaseTableItem::WIDGET,"",NULL,equipmentNoA,NULL));
           connect(equipmentNoA,SIGNAL(clicked()),this,SLOT(getEquipmentInfoSlot()));
        }
         //通过数据库构建B区部件
        if(!query.exec("select No,Name from equipmentInfo where Area='B'"))
        {
            qDebug()<<query.lastError().text();
        }
        while(query.next())
        {
           QPushButton *equipmentNoB = new QPushButton(query.value(1).toString(),this);
           list.append(query.value(0).toString());
           equipmentNoB->setObjectName(query.value(0).toString());
           equipmentNoB->setStyleSheet(statusColor.at(0));//初始化设备离线状态
           equipmentNoB->setFixedHeight(BTNHEIGHT-16);
           itemsB.append(new BaseTableItem(BaseTableItem::WIDGET,"",NULL,equipmentNoB,NULL));
           connect(equipmentNoB,SIGNAL(clicked()),this,SLOT(getEquipmentInfoSlot()));
        }
        /*该部分内容如果在构造函数中执行，此时还没有连接信号与槽，故信号得不到响应
         但是当添加设备时再次执行该函数，该信号是有效的*/
        emit sendEquipmentNoList(list);//通过信号发送所有的设备号到信息展示界面和信息汇总界面
    }
    addBtnA = new QPushButton(this);
    addBtnA->setStyleSheet("border-image: url(:/images/add.png);outline:none;");//目前arm板加载jpg格式的图片有问题，段错误
    //addBtnA->setStyleSheet(statusColor.at(0));
    addBtnA->setFixedHeight(BTNHEIGHT-16);
    connect(addBtnA,SIGNAL(clicked()),this,SLOT(addEquipmentASlot()));
    addBtnB = new QPushButton(this);
    addBtnB->setStyleSheet("border-image: url(:/images/add.png);outline:none;");
    //addBtnB->setStyleSheet(statusColor.at(0));
    addBtnB->setFixedHeight(BTNHEIGHT-16);
    connect(addBtnB,SIGNAL(clicked()),this,SLOT(addEquipmentBSlot()));
    itemsA.append(new BaseTableItem(BaseTableItem::WIDGET,"",NULL,addBtnA,NULL));
    itemsB.append(new BaseTableItem(BaseTableItem::WIDGET,"",NULL,addBtnB,NULL));
    //初始化表格部件
    tableWidgetA = new BaseTableWidget(5,4,itemsA,true,this);
    tableWidgetA->setFrameShape(QFrame::NoFrame);//不显示表格边框
    //tableWidgetA->setAutoFillBackground(true);
    tableWidgetA->setStyleSheet("selection-background-color: "
                                   "rgba(255, 255, 255, 0);");//单元格选中有默认样式，这里设置白色透明
    tableWidgetA->setColWidthRowHeight(BTNWIDTH,BTNHEIGHT);
    tableWidgetA->horizontalHeader()->setVisible(false);//表头不可显示
    tableWidgetA->verticalHeader()->setVisible(false);
    tableWidgetA->setEditTriggers(QAbstractItemView::NoEditTriggers);//设置表格不可编辑
    tableWidgetA->setGridStyle(Qt::NoPen);//不显示网格线
    //设置垂直滚动条的样式，主要是宽些
    tableWidgetA->verticalScrollBar()->setStyleSheet(":vertical {"
                                                     "border: 4px groove grey;"
                                                     "background: #f3f3f3;"
                                                     "width: 28px;}"
                                                     );
    tableWidgetB = new BaseTableWidget(5,4,itemsB,true,this);
    tableWidgetB->setFrameShape(QFrame::NoFrame);
    //tableWidgetB->setAutoFillBackground(true);
    tableWidgetB->setStyleSheet("selection-background-color: "
                                   "rgba(255, 255, 255, 0);");//单元格选中有默认样式，这里设置白色透明
    tableWidgetB->setColWidthRowHeight(BTNWIDTH,BTNHEIGHT);
    tableWidgetB->horizontalHeader()->setVisible(false);//表头不可显示
    tableWidgetB->verticalHeader()->setVisible(false);
    tableWidgetB->setEditTriggers(QAbstractItemView::NoEditTriggers);//设置表格不可编辑
    tableWidgetB->setGridStyle(Qt::NoPen);//不显示网格线
    tableWidgetB->verticalScrollBar()->setStyleSheet(":vertical {"
                                                     "border: 4px solid grey;"
                                                     "background: #f3f3f3;"
                                                     "width: 28px;}"
                                                     );
    hBoxLayoutA->addWidget(tableWidgetA);
    hBoxLayoutB->addWidget(tableWidgetB);
}

/*
 *@brief:   初始化设备状态展示组，用颜色标示状态
 *@author:  缪庆瑞
 *@date:    2017.3.17
 */
void PlantGlobalWidget::initStatusGroupAreaC()
{
    statusGroupAreaC = new QWidget(this);
    statusGroupAreaC->setAutoFillBackground(true);
    //按钮  不同状态的样式
    statusColor<<"QPushButton{background-color: lightGray;border-radius: 4px;font: 20pt \"msyh\";outline:none;}"//离线
                 "QPushButton:pressed{background-color: orange;}"
              <<"QPushButton{background-color: gray;border-radius: 4px;font: 20pt \"msyh\";outline:none;}"//待机
                "QPushButton:pressed{background-color: orange;}"
              <<"QPushButton{background-color: #00aa00;border-radius: 4px;font: 20pt \"msyh\";outline:none;}"//运行
                "QPushButton:pressed{background-color: orange;}"
              <<"QPushButton{background-color: red;border-radius: 4px;font: 20pt \"msyh\";outline:none;}"//报警
                "QPushButton:pressed{background-color: orange;}";
    //编辑框 不同状态的样式
    editStatusColor<<"color: lightGray;font: 18pt \"mysh\";"//离线
                  <<"color: gray;font: 18pt \"msyh\";"//待机
                  <<"color: blue;font: 18pt \"msyh\";"//运行 电压
                  <<"color: darkgreen;font: 18pt \"msyh\";"//运行 电流
                  <<"color: red;font: 18pt \"mysh\";";//警报
    QStringList statusText;
    statusText<<tr("离线")<<tr("待机")<<tr("运行")<<tr("警报");
    QFont statusTextFont("msyh",20);
    for(int i=0;i<4;i++)
    {
        statusBtn[i] = new QPushButton(this);
        statusBtn[i]->setEnabled(false);
        statusBtn[i]->setStyleSheet(statusColor.at(i));
        statusLabel[i] = new QLabel(this);
        statusLabel[i]->setText(statusText.at(i));
        statusLabel[i]->setFont(statusTextFont);
    }
    QHBoxLayout *statusGroupLayout = new QHBoxLayout(statusGroupAreaC);
    statusGroupLayout->setContentsMargins(2,1,1,1);
    statusGroupLayout->addWidget(statusBtn[0]);
    statusGroupLayout->addWidget(statusLabel[0]);
    statusGroupLayout->addWidget(statusBtn[1]);
    statusGroupLayout->addWidget(statusLabel[1]);
    statusGroupLayout->addWidget(statusBtn[2]);
    statusGroupLayout->addWidget(statusLabel[2]);
    statusGroupLayout->addWidget(statusBtn[3]);
    statusGroupLayout->addWidget(statusLabel[3]);
}
/*
 *@brief:   创建数据库连接
 *@author:  缪庆瑞
 *@date:    2017.3.29
 *@param:   databaseName:数据库文件名
 *@param:   connectionName:指定连接名字，同时使用多个数据库名时需要
 *@return:  bool:true表示连接成功  false:连接失败
 */
bool PlantGlobalWidget::createConnection(QString databaseName, QString connectionName)
{
    //如果连接已存在就不需要在创建连接，相当于连接创建成功
    if(QSqlDatabase::contains(connectionName))
    {
        return true;
    }
    db = QSqlDatabase::addDatabase("QSQLITE",connectionName);//添加数据库驱动
    db.setDatabaseName(databaseName);//数据库连接命名
    if(!db.open())//打开数据库
    {
        qDebug()<<db.lastError();
        MyMessageBox::myCritical(0,tr("无法打开数据库"),tr("不能够建立数据库连接"),20);
        return false;
    }
    else
    {
        /*QSqlQuery query(db);//在创建数据库时修改默认字符编码为utf8来支持中文
        if(!query.exec("ALTER DATABASE `"+databaseName+"` DEFAULT CHARACTER SET utf8 COLLATE utf8_bin"))
        {
            qDebug()<<query.lastError();
            return false;
        }*/
        return true;
    }
}
/*
 *@brief:   保存实时数据到数据库，以供查看历史数据记录
 *@author:  缪庆瑞
 *@date:    2017.4.18
 *@param:   tableNameSuffix:数据库表名后缀(表名形如equipment_A01)，这里是设备号，为每一个设备建一张表保存数据
 *@param:   date:日期 yyyy-MM-dd
 *@param:   time:时间 hh:mm:ss
 *@param:   voltage:电压
 *@param:   current:电流
 */
void PlantGlobalWidget::saveDataToDb(QString tableNameSuffix, QString date, QString time, double voltage, int current)
{
    QString tableName("equipment_"+tableNameSuffix);
    QSqlQuery query(db);//此时数据库已经被连接可以直接用
    //判断对应设备的表是否存在，不存在则新建表
    if(!query.exec(QString("create table if not exists '%1'(Edate date,Etime time,Evoltage float,Ecurrent int)").arg(tableName)))
    {
        qDebug()<<"create table failed..."<<query.lastError().text();
        return;
    }
    //表存在，插入数据
    if(!query.exec(QString("insert into '%1' values('%2','%3',%4,%5)").arg(tableName).arg(date).arg(time).arg(voltage).arg(current)))
    {
        qDebug()<<"insert data failed..."<<query.lastError().text();
        return;
    }
}
/*
 *@brief:   获取各个状态的设备数量，并发给信息汇总界面
 *@author:  缪庆瑞
 *@date:    2017.4.24
 */
void PlantGlobalWidget::getEqmStatusCount()
{
    //qDebug()<<"getStatus1:"<<QTime::currentTime().toString("hh:mm:ss:zzz");
    int eqmCountA = itemsA.size()-1;//A区设备数    减去添加按钮
    int eqmCountB = itemsB.size()-1;//B区设备数
    int allEqmCount = eqmCountA+eqmCountB;//共有设备数量
    int offlineEqmCount = 0;//离线设备数
    int standbyEqmCount = 0;//待机设备数
    int runEqmCount = 0;//运行设备数
    int warningEqmCount = 0;//报警设备数
    for(int i=0;i<eqmCountA;i++)//通过按钮的样式判断设备状态
    {
        if(itemsA.at(i)->widget1->styleSheet()==statusColor.at(0))//离线
        {
            offlineEqmCount++;
        }
        else if(itemsA.at(i)->widget1->styleSheet()==statusColor.at(1))//待机
        {
            standbyEqmCount++;
        }
        else if(itemsA.at(i)->widget1->styleSheet()==statusColor.at(2))//运行
        {
            runEqmCount++;
        }
        else if(itemsA.at(i)->widget1->styleSheet()==statusColor.at(3))//报警
        {
            warningEqmCount++;
        }
    }
    for(int i=0;i<eqmCountB;i++)//通过按钮的样式判断设备状态
    {
        if(itemsB.at(i)->widget1->styleSheet()==statusColor.at(0))//离线
        {
            offlineEqmCount++;
        }
        else if(itemsB.at(i)->widget1->styleSheet()==statusColor.at(1))//待机
        {
            standbyEqmCount++;
        }
        else if(itemsB.at(i)->widget1->styleSheet()==statusColor.at(2))//运行
        {
            runEqmCount++;
        }
        else if(itemsB.at(i)->widget1->styleSheet()==statusColor.at(3))//报警
        {
            warningEqmCount++;
        }
    }
    emit sendEqmCount(allEqmCount,offlineEqmCount,standbyEqmCount,runEqmCount,warningEqmCount);
    //qDebug()<<"getStatus2:"<<QTime::currentTime().toString("hh:mm:ss:zzz");
}
/*
 *@brief:   获取被点击的设备信息
 *@author:  缪庆瑞
 *@date:    2017.3.30
 */
void PlantGlobalWidget::getEquipmentInfoSlot()
{
    QPushButton *tempBtn = qobject_cast<QPushButton *>(sender());
    emit currentTabChange(tempBtn->objectName());
    //equipmentNoLabel->setText(tr("A区")+id);
    //根据按钮显示的文本确定当前展示的设备号,该设备号与客户端发来组号的保持对应
    //currentEquipmentNo = id.left(1).toInt();
    //以下为通过设备号获取WiFi传来的对应数据处理显示
}
/*
 *@brief:   响应A区添加设备的按钮信号，弹出添加对话框
 *@author:  缪庆瑞
 *@date:    2017.3.31
 */
void PlantGlobalWidget::addEquipmentASlot()
{
    addEquipmentDialog->setArea("A");//通过该方法确定添加的设备在哪个区域
    addEquipmentDialog->exec();//模态运行
}
/*
 *@brief:   响应B区添加设备的按钮信号，弹出添加对话框
 *@author:  缪庆瑞
 *@date:    2017.3.31
 */
void PlantGlobalWidget::addEquipmentBSlot()
{
    addEquipmentDialog->setArea("B");
    addEquipmentDialog->exec();
}
/*
 *@brief:   响应添加设备成功的信号，重新读取数据库更新界面设备按钮
 *@author:  缪庆瑞
 *@date:    2017.3.25
 */
void PlantGlobalWidget::updateTable()
{
    //deletelater删除表格,表格中的各cellwidget都会被删除，所以需要重新初始化表格
    tableWidgetA->deleteLater();
    tableWidgetB->deleteLater();
    itemsA.clear();
    itemsB.clear();

    initEquipmentNoBtn();
    getEqmStatusCount();
}
/*
 *@brief:   处理线程发来的设备信息，实时显示或根据规则判断设备状态
 *@author:  缪庆瑞
 *@date:    2017.4.1
 *@param:   infoList:已被转换成可显示数据的设备信息链表
 */
void PlantGlobalWidget::handleEquipmentInfoSlot(QStringList infoList)
{
    QStringList hashValueList;//存储对应设备的实时状态及数据

    QString ipAddr = infoList.at(0);//客户端设备的地址
    QString voltage = infoList.at(1);//电压
    QString current = infoList.at(2);//电流
    QString date = infoList.at(3);//日期 yyyy-MM-dd
    QString time = infoList.at(4);//时间 hh:mm:ss
    double voltageD = voltage.toDouble();
    int currentI = current.toInt();
    //qDebug()<<ipAddr;
    QSqlQuery query(db);
    if(!query.exec(QString("select No from equipmentInfo where IpAddr='%1'").arg(ipAddr)))
    {
        qDebug()<<query.lastError().text();
    }
    if(query.next())//如果数据库有该地址的设备则处理
    {
        QString equipmentNo = query.value(0).toString();//根据唯一的ip获取唯一的设备号
        QPushButton *equipmentNoBtn = this->findChild<QPushButton*>(equipmentNo);
        if(equipmentNoBtn==NULL)
        {
            qDebug()<<tr("1:未找到ip对应的设备号部件！！！");
            return;
        }
        if(currentI<=10)//待机  电流小于10A
        {
            equipmentNoBtn->setStyleSheet(statusColor.at(1));
            hashValueList.append(tr("待机"));
            hashValueList.append(editStatusColor.at(1));
            hashValueList.append(voltage);
            hashValueList.append(editStatusColor.at(1));
            hashValueList.append(current);
            hashValueList.append(editStatusColor.at(1));
        }
        else if(currentI>400||voltageD>40)//警报
        {
            equipmentNoBtn->setStyleSheet(statusColor.at(3));
            hashValueList.append(tr("警报 参数超限"));
            hashValueList.append(editStatusColor.at(3));
            hashValueList.append(voltage);
            if(voltageD>40)
            {
                hashValueList.append(editStatusColor.at(4));
            }
            else
            {
                hashValueList.append(editStatusColor.at(2));
            }
            hashValueList.append(current);
            if(currentI>400)
            {
                hashValueList.append(editStatusColor.at(4));
            }
            else
            {
                hashValueList.append(editStatusColor.at(3));
            }
        }
        else//运行
        {
            equipmentNoBtn->setStyleSheet(statusColor.at(2));
            hashValueList.append(tr("运行"));
            hashValueList.append(editStatusColor.at(3));
            hashValueList.append(voltage);
            hashValueList.append(editStatusColor.at(2));
            hashValueList.append(current);
            hashValueList.append(editStatusColor.at(3));
        }
        hashValueList.append(time);//存储数据对应的实时时间
        //qDebug()<<"write db:0"<<QDateTime::currentDateTime().toString("hh:mm:ss:zzz");
        emit sendEquipmentData(equipmentNo,hashValueList);
        getEqmStatusCount();//获取各状态设备数量，并发送
        //将数据保存到数据库 因为写数据库一条数据相对耗时，所以写数据放在实时展示后
        //qDebug()<<"write db:1"<<QDateTime::currentDateTime().toString("hh:mm:ss:zzz");
        saveDataToDb(equipmentNo,date,time,voltageD,currentI);
        //qDebug()<<"write db:2"<<QDateTime::currentDateTime().toString("hh:mm:ss:zzz");
    }
    else //数据库没有该地址的设备则处理
    {
        qDebug()<<tr("2:未找到对应Ip地址的设备");
    }
}

/*
 *@brief:   线程发来的客户端断开连接信号处理，设置离线状态，清空显示数据
 *@author:  缪庆瑞
 *@date:    2017.3.25
 *@param:  ipAddr:断开连接客户端的模拟ip地址
 */
void PlantGlobalWidget::disconnectSlot(QString ipAddr)
{
    //qDebug()<<ipAddr;
    QSqlQuery query(db);
    if(!query.exec(QString("select No from equipmentInfo where IpAddr='%1'").arg(ipAddr)))
    {
        qDebug()<<query.lastError().text();
    }
    if(query.next())
    {
        QString equipmentNo = query.value(0).toString();//根据唯一的ip获取唯一的设备号
        QPushButton *equipmentNoBtn = this->findChild<QPushButton*>(equipmentNo);
        if(equipmentNoBtn==NULL)
        {
            qDebug()<<tr("3:未找到ip对应的设备号部件！！！");
            return;
        }
        else
        {
            equipmentNoBtn->setStyleSheet(statusColor.at(0));//设置成离线状态
            QStringList hashValueList;
            hashValueList.append(tr("离线"));
            hashValueList.append(editStatusColor.at(0));
            hashValueList.append("N/A");
            hashValueList.append(editStatusColor.at(0));
            hashValueList.append("N/A");
            hashValueList.append(editStatusColor.at(0));
            //断开连接时向信息展示页发送信号，提供断开后的状态显示
            emit sendEquipmentData(equipmentNo,hashValueList);
            getEqmStatusCount();//获取各状态设备数量，并发送
        }
    }
    else //数据库没有该地址的设备则处理
    {
        qDebug()<<tr("4:未找到对应Ip地址的设备");
    }
}
/*
 *@brief:   更新表格显示的名字，在信息展示界面成功修改设备参数保存到数据库后，发出信号
 *                重新根据数据库保存的数据设置设备名称，并捎带向客户端发送预置电流电压的值，模拟控制
 *@author:  缪庆瑞
 *@date:    2017.3.25
 *@param:  equipmentNo:设备按钮部件的objectName
 */
void PlantGlobalWidget::updateTableBtnName(QString equipmentNo)
{
    QStringList list;//服务器给客户端发送预置电流电压值，模拟控制
    QSqlQuery query(db);
    if(!query.exec(QString("select Name,IpAddr,PresetA,PresetV from equipmentInfo where No='%1'").arg(equipmentNo)))
    {
        qDebug()<<query.lastError().text();
    }
    if(query.next())
    {
        QPushButton *equipmentNoBtn = this->findChild<QPushButton*>(equipmentNo);
        if(equipmentNoBtn==NULL)
        {
            qDebug()<<tr("未找到ip对应的设备号部件！！！");
            return;
        }
        else
        {
            equipmentNoBtn->setText(query.value(0).toString());
            list.append(query.value(1).toString());
            list.append(query.value(2).toString());
            list.append(query.value(3).toString());
        }
    }
    //模拟控制操作，服务器向客户端发送预置电流电压数据
    emit replyDataToClient(list);
}
