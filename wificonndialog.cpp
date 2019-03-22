/*
 *@file:   wificonndialog.cpp
 *@author: 缪庆瑞
 *@date:   2017.5.24
 *@brief:  连接wifi输入密码的界面
 */
#include "wificonndialog.h"
#include <QGridLayout>
#include <QHBoxLayout>

WifiConnDialog::WifiConnDialog(QString name, QWidget *parent):
    QDialog(parent)
{
    QFont font("msyh",20);
    this->setFont(font);
    this->setWindowTitle(tr("连接WIFI......"));
    this->resize(320,200);
    this->setModal(true);//将对话框设置为模态,默认为应用模态：阻塞所有的窗口输入
    //窗口模态，仅阻塞父窗口及以上的窗口，以免阻塞对话框的子窗口-软键盘无法输入
    this->setWindowModality(Qt::WindowModal);
    QLabel *wifiNameLabel = new QLabel(tr("无线名称:"));
    wifiNameLabel->setFont(font);
    wifiName = new QLineEdit(this);
    wifiName->setFont(font);
    wifiName->setText(name);
    wifiName->setEnabled(false);
    wifiName->setFixedHeight(45);
    QLabel *wifiPasswdLabel = new QLabel(tr("无线密码:"));
    wifiPasswdLabel->setFont(font);
    wifiPasswd = new QLineEdit(this);
    wifiPasswd->setFont(font);
    wifiPasswd->setFixedHeight(45);
    wifiPasswd->installEventFilter(this);//lineEdit部件在该对象上安装事件过滤器
    okBtn = new QPushButton(tr("确定"),this);
    okBtn->setFixedHeight(50);
    okBtn->setStyleSheet("QPushButton{outline:none;font: 20px \"msyh\";}"
                         "QPushButton:pressed{background-color: orange;}");
    cancelBtn = new QPushButton(tr("取消"),this);
    cancelBtn->setFixedHeight(50);
    cancelBtn->setStyleSheet("QPushButton{outline:none;font: 22px \"msyh\";}"
                             "QPushButton:pressed{background-color: orange;}");
    connect(okBtn,SIGNAL(clicked()),this,SLOT(okSlot()));
    connect(cancelBtn,SIGNAL(clicked()),this,SLOT(cancelSlot()));
    QHBoxLayout *hBoxLayout = new QHBoxLayout();
    hBoxLayout->setMargin(1);
    hBoxLayout->addWidget(cancelBtn);
    hBoxLayout->addWidget(okBtn);

    QGridLayout *gridLayout = new QGridLayout(this);
    gridLayout->setSpacing(5);
    gridLayout->addWidget(wifiNameLabel,0,0,1,1);
    gridLayout->addWidget(wifiName,0,1,1,1);
    gridLayout->addWidget(wifiPasswdLabel,1,0,1,1);
    gridLayout->addWidget(wifiPasswd,1,1,1,1);
    gridLayout->addLayout(hBoxLayout,2,0,1,2);
}

WifiConnDialog::~WifiConnDialog()
{

}
/*
 *@brief:   确定按钮响应槽,将对应的wifi及密码写入配置文件
 *@author:  缪庆瑞
 *@date:    2017.5.24
 */
void WifiConnDialog::okSlot()
{
    QString name = wifiName->text();
    QString passwd = wifiPasswd->text();
    if(passwd.size()<8)
    {
        MyMessageBox::myInformation(this,"information",tr("密码长度不够,至少8位!!!"));
        return;
    }
    emit connectingSignal(name,passwd);
    cancelSlot();
}
/*
 *@brief:   取消连接wifi
 *@author:  缪庆瑞
 *@date:    2017.5.24
 */
void WifiConnDialog::cancelSlot()
{
    this->close();
    this->deleteLater();
}
/*
 *@brief:   处理软键盘发来的文本
 *@author:  缪庆瑞
 *@date:    2017.5.24
 */
void WifiConnDialog::keyboardSlot(QString text)
{
    tempEdit->setText(text);//将软键盘输入的内容填入点击的edit上
}
/*
 *@brief:   事件过滤器处理函数
 *@author:  缪庆瑞
 *@date:    2017.5.24
 *@param:   obj:产生事件的对象
 *@param:   event:产生的事件
 */
bool WifiConnDialog::eventFilter(QObject *obj, QEvent *event)
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

