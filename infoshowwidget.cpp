/*
 *@file:   infoshowwidget.cpp
 *@author: 缪庆瑞
 *@date:   2017.3.20
 *@brief:  信息展示界面，包括设备参数，视频监控等
 */
#include "infoshowwidget.h"
#include <QGridLayout>
#include <QDebug>
#include <QStyleFactory>

InfoShowWidget::InfoShowWidget(QWidget *parent) : QWidget(parent)
{
    initEquipmentParamArea();//初始化设备参数信息展示区域   
    initTabWidget();//初始化曲线与视频监控tab页
    initEquipmentNoBox();

    QGridLayout *gridLayout = new QGridLayout(this);
    gridLayout->setContentsMargins(1,1,1,1);
    gridLayout->setSpacing(0);
    gridLayout->addWidget(equipmentParamBox,0,0,1,1);
    gridLayout->addWidget(tabWidget,0,1,1,1);

    gridLayout->setColumnStretch(0,1);
    gridLayout->setColumnStretch(1,2);
    //连接软键盘信号
    connect(softKeyboard,SIGNAL(sendText(QString)),this,SLOT(keyboardSlot(QString)));
}

InfoShowWidget::~InfoShowWidget()
{

}
/*
 *@brief:   初始化设备参数信息展示区
 *@author:  缪庆瑞
 *@date:    2017.3.28
 */
void InfoShowWidget::initEquipmentParamArea()
{
    equipmentParamBox = new QGroupBox(this);//无标题的groupbox
    //equipmentParamBox->setStyle(QStyleFactory::create("Windows"));
    //equipmentParamBox->setAutoFillBackground(true);
    //初始化设备信息参数的相关部件
    titleLabel = new QLabel(tr("参数信息 "),this);
    titleLabel->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    titleLabel->setStyleSheet("color:darkred;font: 22px \"msyh\";");//字体使用bold加粗可能会影响字号
    alterParamBtn = new QPushButton(tr("修改"),this);
    alterParamBtn->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
    alterParamBtn->setStyleSheet("font: 20px;outline: none;");
    updateParamBtn = new QPushButton(tr("保存"),this);
    updateParamBtn->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
    updateParamBtn->setStyleSheet("font: 20px;outline: none;");
    updateParamBtn->setEnabled(false);
    connect(alterParamBtn,SIGNAL(clicked()),this,SLOT(alterParamSlot()));
    connect(updateParamBtn,SIGNAL(clicked()),this,SLOT(updateParamSlot()));

    QString styleSheetString("color: black;border-radius: 2px;font: 18px \"msyh\";");//定义按钮标签的样式
    //定义lineEdit编辑框使能和非使能的样式，如果要同时定义不同状态的不同样式，需要加上对应的类名，不然语法不符合规则
    QString styleSheetString2("QLineEdit:disabled{color: gray;font: 18px \"msyh\";}QLineEdit{font: 18px \"msyh\";}");
    equipmentNoBtn = new QPushButton(tr("设备号:"),this);
    equipmentNoBtn->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    equipmentNoBtn->setEnabled(false);//暂时充当一个label，不可点击
    equipmentNoBtn->setStyleSheet(styleSheetString);
    equipmentNoBox = new QComboBox(this);
    equipmentNoBox->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    connect(equipmentNoBox,SIGNAL(currentIndexChanged(QString)),this,SLOT(refreshDisplay(QString)));
    //设置下拉框滚动条的样式,主要是加宽滚动条以至于在触摸屏上便于滑动
    equipmentNoBox->view()->verticalScrollBar()->setStyleSheet(":vertical {"
                                                               "border: 4px groove grey;"
                                                               "background: #f3f3f3;"
                                                               "width: 28px;}"
                                                               );
    equipmentNameBtn = new QPushButton(tr("设备名称:"),this);
    equipmentNameBtn->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    equipmentNameBtn->setEnabled(false);
    equipmentNameBtn->setStyleSheet(styleSheetString);
    equipmentNameEdit= new QLineEdit(this);
    equipmentNameEdit->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    equipmentNameEdit->setEnabled(false);
    equipmentNameEdit->setStyleSheet(styleSheetString2);
    equipmentNameEdit->installEventFilter(this);//安装事件过滤器
    equipmentAddrBtn = new QPushButton(tr("设备地址:"),this);
    equipmentAddrBtn->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    equipmentAddrBtn->setEnabled(false);
    equipmentAddrBtn->setStyleSheet(styleSheetString);
    equipmentAddrEdit = new QLineEdit(this);
    equipmentAddrEdit->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    equipmentAddrEdit->setEnabled(false);
    equipmentAddrEdit->setStyleSheet(styleSheetString2);
    equipmentAddrEdit->installEventFilter(this);
    equipmentStatusBtn = new QPushButton(tr("运行状态:"),this);
    equipmentStatusBtn->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    equipmentStatusBtn->setEnabled(false);
    equipmentStatusBtn->setStyleSheet(styleSheetString);
    equipmentStatusEdit = new QLineEdit(this);
    equipmentStatusEdit->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    equipmentStatusEdit->setText(tr("离线"));
    equipmentStatusEdit->setEnabled(false);
    equipmentStatusEdit->installEventFilter(this);
    equipmentPresetVBtn = new QPushButton(tr("给定电压:"),this);
    equipmentPresetVBtn->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    equipmentPresetVBtn->setEnabled(false);
    equipmentPresetVBtn->setStyleSheet(styleSheetString);
    equipmentPresetVEdit = new QLineEdit(this);
    equipmentPresetVEdit->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    equipmentPresetVEdit->setEnabled(false);
    equipmentPresetVEdit->setStyleSheet(styleSheetString2);
    equipmentPresetVEdit->installEventFilter(this);
    equipmentVoltageBtn = new QPushButton(tr("实际电压:"),this);
    equipmentVoltageBtn->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    equipmentVoltageBtn->setEnabled(false);
    equipmentVoltageBtn->setStyleSheet(styleSheetString);
    equipmentVoltageEdit = new QLineEdit(this);
    equipmentVoltageEdit->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    equipmentVoltageEdit->setEnabled(false);
    equipmentVoltageEdit->installEventFilter(this);
    equipmentPresetABtn = new QPushButton(tr("给定电流:"),this);
    equipmentPresetABtn->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    equipmentPresetABtn->setEnabled(false);
    equipmentPresetABtn->setStyleSheet(styleSheetString);
    equipmentPresetAEdit = new QLineEdit(this);
    equipmentPresetAEdit->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    equipmentPresetAEdit->setEnabled(false);
    equipmentPresetAEdit->setStyleSheet(styleSheetString2);
    equipmentPresetAEdit->installEventFilter(this);
    equipmentCurrentBtn = new QPushButton(tr("实际电流:"),this);
    equipmentCurrentBtn->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    equipmentCurrentBtn->setEnabled(false);
    equipmentCurrentBtn->setStyleSheet(styleSheetString);
    equipmentCurrentEdit = new QLineEdit(this);
    equipmentCurrentEdit->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    equipmentCurrentEdit->setEnabled(false);
    equipmentCurrentEdit->installEventFilter(this);

    //将参数相关部件添加到布局中
    QGridLayout *gridLayout = new QGridLayout(equipmentParamBox);
    gridLayout->setVerticalSpacing(7);//设置垂直间隔
    gridLayout->setContentsMargins(1,1,1,1);
    gridLayout->addWidget(titleLabel,0,0,1,1);
    gridLayout->addWidget(alterParamBtn,0,1,1,1);
    gridLayout->addWidget(updateParamBtn,0,2,1,1);

    gridLayout->addWidget(equipmentNoBtn,1,0,1,1);
    gridLayout->addWidget(equipmentNoBox,1,1,1,2);
    gridLayout->addWidget(equipmentNameBtn,2,0,1,1);
    gridLayout->addWidget(equipmentNameEdit,2,1,1,2);
    gridLayout->addWidget(equipmentAddrBtn,3,0,1,1);
    gridLayout->addWidget(equipmentAddrEdit,3,1,1,2);
    gridLayout->addWidget(equipmentStatusBtn,4,0,1,1);
    gridLayout->addWidget(equipmentStatusEdit,4,1,1,2);
    gridLayout->addWidget(equipmentPresetVBtn,5,0,1,1);
    gridLayout->addWidget(equipmentPresetVEdit,5,1,1,2);
    gridLayout->addWidget(equipmentVoltageBtn,6,0,1,1);
    gridLayout->addWidget(equipmentVoltageEdit,6,1,1,2);
    gridLayout->addWidget(equipmentPresetABtn,7,0,1,1);
    gridLayout->addWidget(equipmentPresetAEdit,7,1,1,2);
    gridLayout->addWidget(equipmentCurrentBtn,8,0,1,1);
    gridLayout->addWidget(equipmentCurrentEdit,8,1,1,2);
}
/*
 *@brief:   初始化曲线绘制与视频监控区域
 *@author:  缪庆瑞
 *@date:    2017.4.6
 */
void InfoShowWidget::initTabWidget()
{
    tabWidget = new QTabWidget(this);
    tabWidget->setAutoFillBackground(true);
    tabWidget->setFont(QFont("msyh",20));
    customPlotWidget = new CustomPlotWidget(this);
    tabWidget->addTab(customPlotWidget,tr("曲线绘制"));
    videoMonitorWidget = new VideoMonitorWidget(this);
    tabWidget->addTab(videoMonitorWidget,tr("视频监控"));
    //设置tab选项卡按钮的样式
    tabWidget->setStyleSheet("::tab{"
                             "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                             "stop: 0 #E1E1E1, stop: 0.4 #DDDDDD,stop: 0.5 #D8D8D8, stop: 1.0 #D3D3D3); "
                             "font: 18px \"mysh\";"
                             "width: 90px;"//选项卡按钮宽度
                             "border-style: solid;"//边框样式
                             "border-color: #a3a3a3;"//边框颜色
                             "border-width: 3px 2px 2px 1px;"//边框宽度 上右下左
                             "border-bottom-color: #C2C7CB; "
                             "padding: 6px;}"
                             "::tab:selected{"//选项卡被选中时的状态
                             "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                             "stop: 0 #fafafa, stop: 0.4 #f4f4f4,stop: 0.5 #e7e7e7, stop: 1.0 #fafafa);"
                             "font: 20px \"mysh\";"
                             "border-bottom-width: 0px;"//不显示边框下边框
                             "border-top-color: red;"
                             "color:red;}");//红色前景
}
/*
 *@brief:   初始化设备号组,通过读取数据库获得设备号
 *@author:  缪庆瑞
 *@date:    2017.3.28
 */
void InfoShowWidget::initEquipmentNoBox()
{
    QStringList list;//哈希表设备号(键)对应的值(初始化为离线状态)
    setEquipmentStatus(list);
    //通过连接名获取在车间总况界面连接的数据库,即共用一个数据库连接
    db = QSqlDatabase::database("connection1");
    QSqlQuery query(db);
    if(!query.exec("select No from equipmentInfo where Area = 'A'"))
    {
        qDebug()<<query.lastError().text();
    }
    while(query.next())
    {
        equipmentNoList.append(query.value(0).toString());
        equipmentInfoHash.insert(query.value(0).toString(),list);//插入hash表
    }
    if(!query.exec("select No from equipmentInfo where Area = 'B'"))
    {
        qDebug()<<query.lastError().text();
    }
    while(query.next())
    {
        equipmentNoList.append(query.value(0).toString());
        equipmentInfoHash.insert(query.value(0).toString(),list);//插入hash表
    }
    equipmentNoBox->addItems(equipmentNoList);
}
/*
 *@brief:   根据设备号改变ComboBox的index
 *@author:  缪庆瑞
 *@date:    2017.3.30
 *@param:   equipmentNo:表示对应的设备
 */
void InfoShowWidget::changeComboBoxIndex(QString equipmentNo)
{
    //如果当前的设备号改变，会发送信号currentIndexChanged(QString)，然后执行刷新界面的槽
    //equipmentNoBox->setCurrentText(equipmentNo);
    //qt4.8.2 没有setCurrentText()方法，所以使用下面的方法代替
    int index = equipmentNoBox->findText(equipmentNo);//通过该方法找到text对应index值
    if(index != -1)//comboBox中有对应的text文本
    {
        equipmentNoBox->setCurrentIndex(index);
    }
}
/*
 *@brief:   该方法是设置哈希表默认的值（离线状态下的电流/电压显示）
 *@author:  缪庆瑞
 *@date:    2017.4.1
 */
void InfoShowWidget::setEquipmentStatus(QStringList &list)
{
    QString styleSheetString("color: lightGray;font: 18px \"mysh\";");
    list.append(tr("离线"));//运行状态
    list.append(styleSheetString);
    list.append(tr("N/A"));//电压
    list.append(styleSheetString);
    list.append(tr("N/A"));//电流
    list.append(styleSheetString);
}
/*
 *@brief:   事件过滤器处理函数
 *@author:  缪庆瑞
 *@date:    2017.5.5
 *@param:   obj:产生事件的对象
 *@param:   event:产生的事件
 */
bool InfoShowWidget::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type()==QEvent::MouseButtonRelease)//只处理鼠标释放事件
    {
        tempEdit = qobject_cast<QLineEdit *>(obj);
        if(tempEdit->isEnabled())//可编辑的edit才可以弹出软键盘
        {
            /*因为将softKeyboard改成了全局对象，可能会导致同一个对象的信号引起多个槽响应，出现段错误
            所以每一次先断开全局对象的所有连接，再连接一次*/
            disconnect(softKeyboard,0,0,0);//断开软键盘对象的所有信号与槽的连接
            //连接软键盘信号
            connect(softKeyboard,SIGNAL(sendText(QString)),this,SLOT(keyboardSlot(QString)));
            softKeyboard->setInputText(tempEdit->text());
            softKeyboard->showFullScreen();
        }
    }
    return QWidget::eventFilter(obj,event);
}
/*
 *@brief:   更新设备号组
 *@author:  缪庆瑞
 *@date:    2017.3.28
 *@param:   equipmentNoList:新的设备号的列表
 */
void InfoShowWidget::updateEquipmentNoBox(QStringList equipmentNoList)
{
    /*因为combobox的clear操作完成后会发送currentIndexChanged(QString)信号，参数为空串""，在槽函数中没有
    判断，引起链表超出范围异常，所以在clear操作之前，断开信号与槽，之后在连接*/
    //qDebug()<<"test11111111"<<equipmentNoList;
    disconnect(equipmentNoBox,SIGNAL(currentIndexChanged(QString)),this,SLOT(refreshDisplay(QString)));
    equipmentNoBox->clear();
    connect(equipmentNoBox,SIGNAL(currentIndexChanged(QString)),this,SLOT(refreshDisplay(QString)));
    //qDebug()<<"test11111112";
    equipmentNoBox->addItems(equipmentNoList);
    equipmentInfoHash.clear();//改变设备号的个数后，需要重新更新hash表
    QStringList list;//哈希表设备号(键)对应的值(初始化为离线状态)
    setEquipmentStatus(list);
    for(int i=0;i<equipmentNoList.size();i++)
    {
        equipmentInfoHash.insert(equipmentNoList.at(i),list);
    }
}
/*
 *@brief:   根据信号设置hash表键值对，
 *@author:  缪庆瑞
 *@date:    2017.4.1
 *@param:   equipmentNo:表示对应的设备(键)
 *@param:   equipmentDataList:表示设备对应的参数及状态信息(值)
 */
void InfoShowWidget::setHashData(QString equipmentNo, QStringList equipmentDataList)
{
    //qDebug()<<"ddddddddddddddddddddddddddd"<<equipmentDataList.size();
    //hash默认一键一值，如果重新给键设值，之前的会自动擦除
    equipmentInfoHash.insert(equipmentNo,equipmentDataList);
    /*判断链表的长度，由于断开连接时要更新状态，也会执行该槽，但发送的信号附带的数据
     不带时间，长度为6，避免产生异常加以判断*/
    if(equipmentDataList.size()>6)
    {
        //将电压/电流/时间设置到图表类的hash表中
        QStringList graphDataList;
        graphDataList.append(equipmentDataList.at(2));//电压
        graphDataList.append(equipmentDataList.at(4));//电流
        graphDataList.append(equipmentDataList.at(6));//时间
        customPlotWidget->setHashData(equipmentNo,graphDataList);
    }
    if(equipmentNoBox->currentText()==equipmentNo)//当前显示的设备为发送数据的设备
    {
        //refreshDisplay(equipmentNo);//刷新界面
        //刷新界面，仅仅刷新实时数据和状态，从数据库读的这里就不刷新了
        QStringList list = equipmentInfoHash.value(equipmentNo);
        equipmentStatusEdit->setText(list.at(0));//文本
        equipmentStatusEdit->setStyleSheet(list.at(1));//样式
        if(list.at(2)!="N/A")//判断当离线时不追加v
        {
            equipmentVoltageEdit->setText(list.at(2)+"V");//电压
        }
        else
        {
            equipmentVoltageEdit->setText(list.at(2));//电压
        }
        equipmentVoltageEdit->setStyleSheet(list.at(3));
        if(list.at(4)!="N/A")
        {
            equipmentCurrentEdit->setText(list.at(4)+"A");//电流
        }
        else
        {
            equipmentCurrentEdit->setText(list.at(4));//电流
        }
        equipmentCurrentEdit->setStyleSheet(list.at(5));
        //绘制曲线
        customPlotWidget->graphing(equipmentNo);
    }
}
/*
 *@brief:   根据设备号刷新界面显示
 *@author:  缪庆瑞
 *@date:    2017.3.28
 *@param:   equipmentNo:表示对应的设备
 */
void InfoShowWidget::refreshDisplay(QString equipmentNo)
{
    //当combobox执行clear操作完成后会发送currentIndexChanged(QString)信号，这时参数为""空串
    //qDebug()<<"equipmentNo:"<<equipmentNo;
    customPlotWidget->setEquipmentNo(equipmentNo);//将当前设备号传给绘图界面
    QSqlQuery query(db);
    //根据独一无二的设备号获取数据库的设备信息
    if(!query.exec("select Name,IpAddr,PresetA,PresetV from equipmentInfo where No='"+equipmentNo+"'"))
    {
        qDebug()<<query.lastError().text();
    }
    while(query.next())
    {
        equipmentNameEdit->setText(query.value(0).toString());
        equipmentAddrEdit->setText(query.value(1).toString());
        equipmentPresetAEdit->setText(query.value(2).toString());
        equipmentPresetVEdit->setText(query.value(3).toString());
    }
    QStringList list = equipmentInfoHash.value(equipmentNo);
    //qDebug()<<list<<equipmentNo;
    equipmentStatusEdit->setText(list.at(0));//文本
    equipmentStatusEdit->setStyleSheet(list.at(1));//样式
    equipmentVoltageEdit->setText(list.at(2));
    equipmentVoltageEdit->setStyleSheet(list.at(3));
    equipmentCurrentEdit->setText(list.at(4));
    equipmentCurrentEdit->setStyleSheet(list.at(5));
}
/*
 *@brief:   修改参数，将参数编辑框设置使能
 *@author:  缪庆瑞
 *@date:    2017.4.5
 */
void InfoShowWidget::alterParamSlot()
{
    equipmentNameEdit->setEnabled(true);
    equipmentAddrEdit->setEnabled(true);
    equipmentPresetVEdit->setEnabled(true);
    equipmentPresetAEdit->setEnabled(true);
    alterParamBtn->setEnabled(false);
    updateParamBtn->setEnabled(true);
    this->setFocus();
}
/*
 *@brief:   更新参数到数据库
 *@author:  缪庆瑞
 *@date:    2017.4.5
 */
void InfoShowWidget::updateParamSlot()
{
    QString equipmentNo = equipmentNoBox->currentText();
    QString equipmentName = equipmentNameEdit->text();
    QString equipmentIpAddr = equipmentAddrEdit->text();
    double presetV;
    int presetA;

    bool ok=true;
    QStringList ipAddrSegment = equipmentIpAddr.split(".");//按点分割ip地址段
    if(ipAddrSegment.size()!=4)
    {
        MyMessageBox::myInformation(this,"infomation",tr("ip地址格式不符合规范"),20);
        //QMessageBox::information(this,"infomation",tr("ip地址格式不符合规范"));
        return;
    }
    else
    {
        for(int i=0;i<4;i++)
        {
            int ip = ipAddrSegment.at(i).toInt(&ok);
            if(!ok||ip<0||ip>255)
            {
                MyMessageBox::myInformation(this,"infomation",tr("ip地址格式不符合规范"),20);
                //QMessageBox::information(this,"infomation",tr("ip地址格式不符合规范"));
                return;
            }
        }
    }
    presetV = equipmentPresetVEdit->text().toDouble(&ok);
    if(!ok||presetV>60||presetV<=0)
    {
        MyMessageBox::myInformation(this,"infomation",tr("给定电压应在\n0-60.0V范围内"),20);
        //QMessageBox::information(this,"infomation",tr("给定电压应在\n0-60.0V范围内"));
        return;
    }
    presetA = equipmentPresetAEdit->text().toInt(&ok);
    if(!ok||presetA>600||presetA<=0)
    {
        MyMessageBox::myInformation(this,"infomation",tr("给定电流应在\n0-600A范围内"),20);
        //QMessageBox::information(this,"information",tr("给定电流应在\n0-600A范围内"));
        return;
    }
    //通过连接名获取在车间总况界面连接的数据库,即共用一个数据库连接
    db = QSqlDatabase::database("connection1");
    QSqlQuery query(db);
    if(!query.exec(QString("select * from equipmentInfo where No !='%1' and IpAddr='%2'")
                   .arg(equipmentNo).arg(equipmentIpAddr)))
    {
        qDebug()<<query.lastError().text();
    }
    if(query.next())
    {
        MyMessageBox::myInformation(this,"infomation",tr("ip地址冲突,请修改设备地址"),20);
        //QMessageBox::information(this,"information",tr("ip地址冲突,请修改设备地址"));
        return;
    }
    //更新数据库
    if(!query.exec(QString("update equipmentInfo set Name='%1',IpAddr='%2',PresetA=%3,PresetV=%4 where No='%5'")
                  .arg(equipmentName).arg(equipmentIpAddr).arg(presetA).arg(presetV).arg(equipmentNo)))
    {
        qDebug()<<query.lastError().text();
    }
    //更新成功后设置使能状态
    equipmentNameEdit->setEnabled(false);
    equipmentAddrEdit->setEnabled(false);
    equipmentPresetVEdit->setEnabled(false);
    equipmentPresetAEdit->setEnabled(false);
    alterParamBtn->setEnabled(true);
    updateParamBtn->setEnabled(false);
    this->setFocus();
    emit updateTableBtnName(equipmentNo);//发送信号，通知主页某个设备号的名字可能改变了
}
/*
 *@brief:   软键盘信号处理槽
 *@author:  缪庆瑞
 *@date:    2017.5.5
 *@param:   text:软键盘输入的文本
 */
void InfoShowWidget::keyboardSlot(QString text)
{
    tempEdit->setText(text);//将软键盘输入的内容填入点击的edit上
}
