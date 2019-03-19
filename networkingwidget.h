/*
 *@file:   networkingWidget.h
 *@author: 缪庆瑞
 *@date:   2017.5.22
 *@brief:  配置网络连接的界面
 */
#ifndef NETWORKINGWIDGET_H
#define NETWORKINGWIDGET_H

#include <QWidget>
#include <QTabWidget>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QFrame>
#include <QLineEdit>
#include <QFile>
#include <QTimer>
#include <QEvent>
#include "basetablewidget.h"
#include "wificonndialog.h"
#include "globalvar.h"
#include "mymessagebox.h"

class NetworkingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NetworkingWidget(QWidget *parent = 0);
    ~NetworkingWidget();
    void initTabWidget();

    void initWirelessWidget();//初始化无线设置界面
    void initLeftConnectionBox();//初始化无线设置界面左侧区域
    void initRightPropertyBox();//初始化无线设置界面右侧区域

signals:

public slots:
    void wifiSwitchSlot();//切换wifi开关
    void staticipOrDHCPSlot(int id);//切换DHCP或静态ip设置
    void scanWifi();//扫描无线网络
    void connectWifi(bool reconfig);//连接无线网络
    void getConnectStatus();//获取连接状态

    void selectWifi(int row,int column);//选择列表中的wifi连接
    void configWifi(QString wifiName,QString wifiPasswd);

    void keyboardSlot(QString text);//处理软键盘信号的槽
    void saveSlot();//保存静态ip设置

protected:
    virtual bool eventFilter(QObject *obj,QEvent *event);

private:
    QTabWidget *tabWidget;//实现选项卡式切换界面 无线-有线设置

    QWidget *wirelessWidget;//无线设置界面
    QVBoxLayout *vBoxLayout;//左侧布局
    QVBoxLayout *vBoxLayout2;//右侧布局
    QLineEdit *tempEdit;//用来记录当前引发事件过滤器处理的edit部件
    //左侧连接界面
    QGroupBox *connectionBox;
    QLabel *swicthLabel;
    QPushButton *switchBtn;//无线开关切换
    bool isWifiOn;//标记wifi开启/关闭状态
    BaseTableWidget *currentConnection;//当前连接的wifi
    QLabel *selectableConnectionLabel;
    QPushButton *scanBtn;//扫描按钮
    BaseTableWidget *selectableConnection;//可选的连接
    //右侧属性界面
    QGroupBox *propertyBox;
    QButtonGroup *buttonGroup;
    QRadioButton *DHCPBtn;//动态获取ip
    QRadioButton *staticBtn;//静态设置ip等
    bool isDHCPOn;//标记DHCP/静态ip 状态
    QPushButton *saveBtn;//保存静态ip设置
    QFrame *staticFrame;
    QLabel *ipAddrLabel;
    QLabel *subMaskLabel;
    QLabel *defaultGatewayLabel;
    QLabel *DNSServerLabel1;
    QLabel *DNSServerLabel2;
    QLineEdit *ipAddr;//ip地址
    QLineEdit *subMask;//子网掩码
    QLineEdit *defaultGateway;//默认网关
    QLineEdit *DNSServer1;//首选DNS服务器
    QLineEdit *DNSServer2;//备用DNS服务器

};

#endif // NETWORKINGWIDGET_H
