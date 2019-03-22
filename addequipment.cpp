/*
 *@file:   addequipment.cpp
 *@author: 缪庆瑞
 *@date:   2017.3.31
 *@brief:  添加设备按钮的对话框
 */
#include "addequipment.h"
#include <QGridLayout>
#include <QDebug>

AddEquipment::AddEquipment(QWidget *parent):
    QDialog(parent)
{
    this->resize(360,300);
    this->setModal(true);//将对话框设置为模态,默认为应用模态：阻塞所有的窗口输入
    //窗口模态，仅阻塞父窗口及以上的窗口，以免阻塞对话框的子窗口-软键盘无法输入
    this->setWindowModality(Qt::WindowModal);
    this->setFont(QFont("msyh",20));
    QLabel *equipmentNoLabel = new QLabel(tr("设 备 号:"));
    equipmentNoEdit = new QLineEdit(this);
    equipmentNoEdit->setFixedHeight(45);
    equipmentNoEdit->installEventFilter(this);//lineEdit部件在该对象上安装事件过滤器
    QLabel *equipmentNameLabel = new QLabel(tr("设备名称:"));
    equipmentNameEdit = new QLineEdit(this);
    equipmentNameEdit->setFixedHeight(45);
    equipmentNameEdit->installEventFilter(this);
    QLabel *equipmentAddrLabel = new QLabel(tr("设备地址:"));
    equipmentAddrEdit = new QLineEdit(this);
    equipmentAddrEdit->setFixedHeight(45);
    equipmentAddrEdit->installEventFilter(this);
    QLabel *equipmentPresetVLabel = new QLabel(tr("给定电压:"));
    equipmentPresetVEdit = new QLineEdit(this);
    equipmentPresetVEdit->setFixedHeight(45);
    equipmentPresetVEdit->installEventFilter(this);
    QLabel *equipmentPresetALabel = new QLabel(tr("给定电流:"));
    equipmentPresetAEdit = new QLineEdit(this);
    equipmentPresetAEdit->setFixedHeight(45);
    equipmentPresetAEdit->installEventFilter(this);

    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->setSpacing(5);
    gridLayout->addWidget(equipmentNoLabel,0,0,1,1);
    gridLayout->addWidget(equipmentNoEdit,0,1,1,2);
    gridLayout->addWidget(equipmentNameLabel,1,0,1,1);
    gridLayout->addWidget(equipmentNameEdit,1,1,1,2);
    gridLayout->addWidget(equipmentAddrLabel,2,0,1,1);
    gridLayout->addWidget(equipmentAddrEdit,2,1,1,2);
    gridLayout->addWidget(equipmentPresetVLabel,3,0,1,1);
    gridLayout->addWidget(equipmentPresetVEdit,3,1,1,2);
    gridLayout->addWidget(equipmentPresetALabel,4,0,1,1);
    gridLayout->addWidget(equipmentPresetAEdit,4,1,1,2);

    okBtn = new QPushButton(tr("确定"),this);
    okBtn->setFixedHeight(55);
    okBtn->setStyleSheet("QPushButton{outline:none;font: 20px \"msyh\";}"
                         "QPushButton:pressed{background-color: orange;}");
    cancelBtn = new QPushButton(tr("取消"),this);
    cancelBtn->setFixedHeight(55);
    cancelBtn->setStyleSheet("QPushButton{outline:none;font: 22px \"msyh\";}"
                             "QPushButton:pressed{background-color: orange;}");
    connect(okBtn,SIGNAL(clicked()),this,SLOT(okSlot()));
    connect(cancelBtn,SIGNAL(clicked()),this,SLOT(cancelSlot()));
    QGridLayout *gridLayout1 = new QGridLayout(this);
    gridLayout1->setContentsMargins(9,5,9,4);
    gridLayout1->addLayout(gridLayout,0,0,1,2);
    gridLayout1->addWidget(cancelBtn,1,0,1,1);
    gridLayout1->addWidget(okBtn,1,1,1,1);

}

AddEquipment::~AddEquipment()
{

}
/*
 *@brief:   设置设备所在区域
 *@author:  缪庆瑞
 *@date:    2017.3.31
 */
void AddEquipment::setArea(QString Area)
{
    this->Area = Area;
}
/*
 *@brief:   确定按钮响应槽,对输入的数据格式判断，并对比数据库新增设备是否冲突
 *@author:  缪庆瑞
 *@date:    2017.3.31
 */
void AddEquipment::okSlot()
{
    bool ok=true;
    QString equipmentNo = equipmentNoEdit->text();
    QString equipmentName = equipmentNameEdit->text();
    QString equipmentIpAddr = equipmentAddrEdit->text();
    double presetV;
    int presetA;
    if(equipmentNo.isEmpty())
    {
        MyMessageBox::myInformation(this,"infomation",tr("设备号不能为空"));
        //QMessageBox::information(this,"infomation",tr("设备号不能为空"));
        return;
    }
    QStringList ipAddrSegment = equipmentIpAddr.split(".");//按点分割ip地址段
    if(ipAddrSegment.size()!=4)
    {
        MyMessageBox::myInformation(this,"infomation",tr("ip地址格式不符合规范"));
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
                MyMessageBox::myInformation(this,"infomation",tr("ip地址格式不符合规范2"));
                //QMessageBox::information(this,"infomation",tr("ip地址格式不符合规范2"));
                return;
            }
        }
    }
    presetV = equipmentPresetVEdit->text().toDouble(&ok);
    if(!ok||presetV>60||presetV<=0)
    {
        MyMessageBox::myInformation(this,"infomation",tr("给定电压应在\n0-60.0V范围内"));
        //QMessageBox::information(this,"infomation",tr("给定电压应在\n0-60.0V范围内"));
        return;
    }
    presetA = equipmentPresetAEdit->text().toInt(&ok);
    if(!ok||presetA>600||presetA<=0)
    {
        MyMessageBox::myInformation(this,"information",tr("给定电流应在\n0-600A范围内"));
        //QMessageBox::information(this,"information",tr("给定电流应在\n0-600A范围内"));
        return;
    }
    //通过连接名获取在车间总况界面连接的数据库,即共用一个数据库连接
    db = QSqlDatabase::database("connection1");
    QSqlQuery query(db);
    if(!query.exec("select * from equipmentInfo where No='"+equipmentNoEdit->text()+"'"))
    {
        qDebug()<<query.lastError().text();
    }
    if(query.next())
    {
        MyMessageBox::myInformation(this,"information",tr("设备号冲突,请修改设备号"));
        //QMessageBox::information(this,"information",tr("设备号冲突,请修改设备号"));
        return;
    }
    if(!query.exec("select * from equipmentInfo where IpAddr='"+equipmentAddrEdit->text()+"'"))
    {
        qDebug()<<query.lastError().text();
    }
    if(query.next())
    {
        MyMessageBox::myInformation(this,"information",tr("ip地址冲突,请修改设备地址"));
        //QMessageBox::information(this,"information",tr("ip地址冲突,请修改设备地址"));
        return;
    }
    if(!query.exec(QString("insert into equipmentInfo values('%1','%2','%3','%4',%5,%6)")
                  .arg(equipmentNo).arg(Area).arg(equipmentName).arg(equipmentIpAddr).arg(presetA).arg(presetV)))
    {
        qDebug()<<query.lastError().text();
    }
    //如果插入数据库成功，发送信号
    emit addSuccessSignal();
    cancelSlot();
}
/*
 *@brief:   取消按钮响应槽
 *@author:  缪庆瑞
 *@date:    2017.3.31
 */
void AddEquipment::cancelSlot()
{
    equipmentNoEdit->clear();
    equipmentNameEdit->clear();
    equipmentAddrEdit->clear();
    equipmentPresetVEdit->clear();
    equipmentPresetAEdit->clear();
    this->close();//默认只是隐藏界面，并不delete对象
}
/*
 *@brief:   软键盘信号处理槽
 *@author:  缪庆瑞
 *@date:    2017.5.5
 *@param:   text:软键盘输入的文本
 */
void AddEquipment::keyboardSlot(QString text)
{
    tempEdit->setText(text);//将软键盘输入的内容填入点击的edit上
}
/*
 *@brief:   事件过滤器处理函数
 *@author:  缪庆瑞
 *@date:    2017.5.5
 *@param:   obj:产生事件的对象
 *@param:   event:产生的事件
 */
bool AddEquipment::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type()==QEvent::MouseButtonRelease)//只处理鼠标释放事件
    {
        tempEdit = qobject_cast<QLineEdit *>(obj);
        /*因为将softKeyboard改成了全局对象，可能会导致同一个对象的信号引起多个槽响应，出现段错误
        所以每一次先断开全局对象的所有连接，再连接一次*/
        disconnect(softKeyboard,0,0,0);//断开软键盘对象的所有信号与槽的连接
        //连接软键盘信号
        connect(softKeyboard,SIGNAL(sendText(QString)),this,SLOT(keyboardSlot(QString)));
        softKeyboard->setInputText(tempEdit->text());
        //softKeyboard->showFullScreen();
        softKeyboard->show();
    }
    return QDialog::eventFilter(obj,event);
}

