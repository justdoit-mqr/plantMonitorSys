/*
 *@file:   networkingWidget.cpp
 *@author: 缪庆瑞
 *@date:   2017.5.22
 *@brief:  配置网络连接的界面
 */
#define COLUMNWIDTH 400
#define ROWHEIGHT 42
#include "networkingwidget.h"
#include <QBoxLayout>
#include <QGridLayout>
#include <QDebug>

NetworkingWidget::NetworkingWidget(QWidget *parent) : QWidget(parent)
{
    initWirelessWidget();
    initTabWidget();

    QVBoxLayout *vBoxLayout = new QVBoxLayout(this);
    vBoxLayout->setMargin(1);
    vBoxLayout->addWidget(tabWidget);
    //wifiSwitchSlot();//开启wifi
}

NetworkingWidget::~NetworkingWidget()
{

}
/*
 *@brief:   初始化选项卡tabWidget界面
 *@author:  缪庆瑞
 *@date:    2017.5.22
 */
void NetworkingWidget::initTabWidget()
{
    tabWidget = new QTabWidget(this);
    tabWidget->setFont(QFont("msyh",20));
    tabWidget->setFocusPolicy(Qt::NoFocus);//不设置焦点，避免虚线框
    tabWidget->addTab(wirelessWidget,tr("无线连接"));
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
 *@brief:   初始化无线连接配置界面
 *@author:  缪庆瑞
 *@date:    2017.5.22
 */
void NetworkingWidget::initWirelessWidget()
{
    initLeftConnectionBox();
    initRightPropertyBox();
    wirelessWidget = new QWidget(this);
    QHBoxLayout *hBoxLayout = new QHBoxLayout(wirelessWidget);
    hBoxLayout->setMargin(1);
    hBoxLayout->setSpacing(0);
    hBoxLayout->addWidget(connectionBox);
    hBoxLayout->addWidget(propertyBox);
}
/*
 *@brief:   初始化无线设置左侧区域
 *@author:  缪庆瑞
 *@date:    2017.5.24
 */
void NetworkingWidget::initLeftConnectionBox()
{
        //左侧连接页面
    QFont font("msyh",20);
    connectionBox = new QGroupBox(tr("连接状态"),this);
    connectionBox->setFont(font);
    QString styleSheetString("font: 18pt \"msyh\";");
    swicthLabel = new QLabel(tr("无线网络开关"),connectionBox);
    swicthLabel->setStyleSheet(styleSheetString);
    switchBtn = new QPushButton(this);//wifi开关按钮
    switchBtn->setFixedSize(60,32);
    isWifiOn = false;//初始化wifi关闭
    switchBtn->setStyleSheet("border-image: url(:/images/WIFI_OFF.ico);outline: none;");
    connect(switchBtn,SIGNAL(clicked()),this,SLOT(wifiSwitchSlot()));
    QHBoxLayout *hBoxLayout1 = new QHBoxLayout();
    hBoxLayout1->setMargin(0);
    hBoxLayout1->addWidget(swicthLabel);
    hBoxLayout1->addStretch(1);
    hBoxLayout1->addWidget(switchBtn);
    currentConnection = new BaseTableWidget(this);//当前连接
    currentConnection->setFont(font);
    currentConnection->setRowCount(1);
    currentConnection->setColumnCount(1);//无线名/信号强度及是否加密
    currentConnection->horizontalHeader()->setVisible(false);//表头不可显示
    currentConnection->verticalHeader()->setVisible(false);
    currentConnection->setColWidthRowHeight(COLUMNWIDTH,ROWHEIGHT);
    currentConnection->setFixedHeight(ROWHEIGHT);
    currentConnection->setEditTriggers(QAbstractItemView::NoEditTriggers);//设置表格不可编辑
    selectableConnectionLabel = new QLabel(tr("选择附近的网络"),this);
    selectableConnectionLabel->setStyleSheet(styleSheetString);
    scanBtn = new QPushButton(tr("扫描"),this);
    scanBtn->setStyleSheet(styleSheetString);
    connect(scanBtn,SIGNAL(clicked()),this,SLOT(scanWifi()));
    QHBoxLayout *hBoxLayout2 = new QHBoxLayout();
    hBoxLayout2->setMargin(0);
    hBoxLayout2->addWidget(selectableConnectionLabel);
    hBoxLayout2->addStretch(1);
    hBoxLayout2->addWidget(scanBtn);
    selectableConnection = new BaseTableWidget(this);//可连接wifi列表
    selectableConnection->setFont(font);
    selectableConnection->setColumnCount(1);
    selectableConnection->horizontalHeader()->setVisible(false);//表头不可显示
    selectableConnection->verticalHeader()->setVisible(false);
    selectableConnection->setColWidthRowHeight(COLUMNWIDTH,ROWHEIGHT);
    selectableConnection->setSelectionMode(QAbstractItemView::SingleSelection);//单选
    connect(selectableConnection,SIGNAL(cellClicked(int,int)),this,SLOT(selectWifi(int,int)));
    //设置垂直滚动条的样式，主要是宽些
    selectableConnection->verticalScrollBar()->setStyleSheet(":vertical {"
                                                     "border: 4px groove grey;"
                                                     "background: #f3f3f3;"
                                                     "width: 28px;}"
                                                     );
    //selectableConnection->setRowCount(0);

    vBoxLayout = new QVBoxLayout(connectionBox);
    vBoxLayout->setContentsMargins(5,1,5,1);
    vBoxLayout->setSpacing(4);
    vBoxLayout->addLayout(hBoxLayout1);
    vBoxLayout->addWidget(currentConnection);
    vBoxLayout->addLayout(hBoxLayout2);
    vBoxLayout->addWidget(selectableConnection);
}
/*
 *@brief:   初始化无线设置右侧区域
 *@author:  缪庆瑞
 *@date:    2017.5.24
 */
void NetworkingWidget::initRightPropertyBox()
{
    //右侧属性页面
    QFont font("msyh",20);
    QString styleSheetString("font: 18pt \"msyh\";");
    propertyBox = new QGroupBox(tr("网络属性"),this);
    propertyBox->setFont(font);
    buttonGroup = new QButtonGroup(this);
    DHCPBtn = new QRadioButton(tr("自动获取ip地址"),this);
    DHCPBtn->setStyleSheet(styleSheetString);
    DHCPBtn->setChecked(true);//使能DHCP状态
    isDHCPOn = true;
    staticBtn = new QRadioButton(tr("使用下面的ip地址"));
    staticBtn->setStyleSheet(styleSheetString);
    buttonGroup->addButton(DHCPBtn,0);
    buttonGroup->addButton(staticBtn,1);
    connect(buttonGroup,SIGNAL(buttonClicked(int)),this,SLOT(staticipOrDHCPSlot(int)));
    saveBtn = new QPushButton(tr("保存"),this);
    saveBtn->setStyleSheet("QPushButton{outline:none;font: 20px \"msyh\";}"
                         "QPushButton:pressed{background-color: orange;}");
    saveBtn->setEnabled(false);
    connect(saveBtn,SIGNAL(clicked()),this,SLOT(saveSlot()));
    QHBoxLayout *hBoxLayout3 = new QHBoxLayout();
    hBoxLayout3->addWidget(staticBtn);
    hBoxLayout3->addStretch(1);
    hBoxLayout3->addWidget(saveBtn);

    //静态ip设置区域
    staticFrame = new QFrame(this);
    staticFrame->setFrameShape(QFrame::Box);
    staticFrame->setFrameShadow(QFrame::Sunken);
    staticFrame->setEnabled(false);
    ipAddrLabel = new QLabel(tr("ip  地址:"),staticFrame);
    ipAddrLabel->setStyleSheet(styleSheetString);
    subMaskLabel = new QLabel(tr("子网掩码:"),staticFrame);
    subMaskLabel->setStyleSheet(styleSheetString);
    defaultGatewayLabel = new QLabel(tr("默认网关:"),staticFrame);
    defaultGatewayLabel->setStyleSheet(styleSheetString);
    DNSServerLabel1 = new QLabel(tr("首选DNS服务器:"),staticFrame);
    DNSServerLabel1->setStyleSheet(styleSheetString);
    DNSServerLabel2 = new QLabel(tr("备用DNS服务器:"),staticFrame);
    DNSServerLabel2->setStyleSheet(styleSheetString);
    ipAddr = new QLineEdit(staticFrame);
    ipAddr->installEventFilter(this);
    ipAddr->setFont(font);
    ipAddr->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    subMask = new QLineEdit(staticFrame);
    subMask->setFont(font);
    subMask->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    defaultGateway = new QLineEdit(staticFrame);
    defaultGateway->setFont(font);
    defaultGateway->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    DNSServer1 = new QLineEdit(staticFrame);
    DNSServer1->setFont(font);
    DNSServer1->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    DNSServer2 = new QLineEdit(staticFrame);
    DNSServer2->setFont(font);
    DNSServer2->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    QGridLayout *gridLayout = new QGridLayout(staticFrame);
    gridLayout->setContentsMargins(3,3,3,3);
    gridLayout->addWidget(ipAddrLabel,0,0,1,1);
    gridLayout->addWidget(ipAddr,0,1,1,1);
    gridLayout->addWidget(subMaskLabel,1,0,1,1);
    gridLayout->addWidget(subMask,1,1,1,1);
    gridLayout->addWidget(defaultGatewayLabel,2,0,1,1);
    gridLayout->addWidget(defaultGateway,2,1,1,1);
    gridLayout->addWidget(DNSServerLabel1,3,0,1,1);
    gridLayout->addWidget(DNSServer1,3,1,1,1);
    gridLayout->addWidget(DNSServerLabel2,4,0,1,1);
    gridLayout->addWidget(DNSServer2,4,1,1,1);

    vBoxLayout2 = new QVBoxLayout(propertyBox);
    vBoxLayout2->setContentsMargins(5,1,5,1);
    vBoxLayout2->setSpacing(3);
    vBoxLayout2->addWidget(DHCPBtn);
    vBoxLayout2->addLayout(hBoxLayout3);
    vBoxLayout2->addWidget(staticFrame);
}
/*
 *@brief:   扫描无线网络
 *@author:  缪庆瑞
 *@date:    2017.5.23
 */
void NetworkingWidget::scanWifi()
{
    //将可连接wifi写入文件
    if(system("wpa_cli scan_result | grep WPA | awk '{print $5> \"./config/wifilist.txt\"}'"))//执行成功返回0
    {
        qDebug()<<"scanWifi failed!!!";
        MyMessageBox::myInformation(this,tr("Scan Wifi Failed"),tr("请接入无线网卡!!!"));
        return ;
    }
    QFile file("./config/wifilist.txt");
    if(file.exists())
    {
        file.open(QIODevice::ReadOnly);
        char wifiName[40];
        int count = 0;
        int size;
        qDebug()<<"selectable wifi list:";
        while((size=file.readLine(wifiName,40))!=-1)
        {
            wifiName[size-1]='\0';
            qDebug()<<tr(wifiName);
            selectableConnection->setRowCount(count+1);
            selectableConnection->setItemValue(count,0,QString(wifiName));
            count++;
        }
        selectableConnection->setColWidthRowHeight(COLUMNWIDTH,ROWHEIGHT);
        file.close();
        file.remove();
    }
    connectWifi(false);
}
/*
 *@brief:   连接无线网络
 *@author:  缪庆瑞
 *@date:    2017.5.23
 *@param: reconfig: 表示是否需要重新配置网络，执行配置文件
 */
void NetworkingWidget::connectWifi(bool reconfig)
{
    /*重新连接wifi前一定要断开当前连接，否则就会造成无线网卡扫描不到信号
     (SCANNING)或者一直在连接中(ASSOCIATED)*/
    system("wpa_cli disconnect");
    if(reconfig)//表示重新配置
    {
        system("wpa_cli reconfigure");//重新执行配置文件
    }
    system("wpa_cli reconnect");//重连
    QTimer::singleShot(5000,this,SLOT(getConnectStatus()));
}
/*
 *@brief:   获取连接状态
 *@author:  缪庆瑞
 *@date:    2017.5.23
 */
void NetworkingWidget::getConnectStatus()
{
    system("wpa_cli status | grep -E ^ssid= > \"./config/wifiname.txt\"");
    system("wpa_cli status | grep wpa_state= >\"./config/wifistatus.txt\"");
    QFile file("./config/wifistatus.txt");
    if(file.exists())
    {
        file.open(QIODevice::ReadOnly);
        char wifistatus[40];
        int size = file.readLine(wifistatus,40);
        wifistatus[size-1]='\0';//readline会读\n
        qDebug()<<wifistatus;
        if(strcmp(wifistatus,"wpa_state=4WAY_HANDSHAKE")==0)
        {
            currentConnection->setItemValue(0,0,tr("当前Wifi密码错误!!!"));
        }
        else if(strcmp(wifistatus,"wpa_state=COMPLETED")==0)
        {
            QFile file2("./config/wifiname.txt");
            if(file2.exists())
            {
                file2.open(QIODevice::ReadOnly);
                char wifiName[40];
                int size = file2.readLine(wifiName,40);
                wifiName[size-1]='\0';
                qDebug()<<wifiName;
                currentConnection->setItemValue(0,0,QString(wifiName).remove(0,5));
                file2.close();
                file2.remove();
            }
            if(isDHCPOn)
            {
                system("udhcpc -i wlan0 -q");
                system("wpa_cli status | grep ip_address=> \"./config/wifiip.txt\"");
                QFile file3("./config/wifiip.txt");
                if(file3.exists())
                {
                    file3.open(QIODevice::ReadOnly);
                    char wifiIp[40];
                    int size = file3.readLine(wifiIp,40);
                    wifiIp[size-1]='\0';
                    qDebug()<<wifiIp;
                    ipAddr->setText(QString(wifiIp).remove(0,11));
                    file3.close();
                    file3.remove();
                }
            }
        }
        file.close();
        file.remove();
    }
}
/*
 *@brief:   选择列表的wifi连接
 *@author:  缪庆瑞
 *@date:    2017.5.24
 */
void NetworkingWidget::selectWifi(int row, int column)
{
    QString wifiName = selectableConnection->getItemText(row,column);
    WifiConnDialog *wifiConnDialog = new WifiConnDialog(wifiName,this);
    connect(wifiConnDialog,SIGNAL(connectingSignal(QString,QString)),this,SLOT(configWifi(QString,QString)));
    wifiConnDialog->show();
}
/*
 *@brief:   设置新WiFi的配置文件
 *@author:  缪庆瑞
 *@date:    2017.5.23
 */
void NetworkingWidget::configWifi(QString wifiName, QString wifiPasswd)
{
    QString configStr = QString("#WPA-PSK/TKIP\n"
                      "ctrl_interface=/var/run/wpa_supplicant\n"
                      "network={\n\t"
                      "ssid=\"%1\"\n\t"
                      "scan_ssid=1\n\t"
                      "key_mgmt=WPA-EAP WPA-PSK IEEE8021X NONE\n\t"
                      "pairwise=CCMP TKIP\n\t"
                      "group=CCMP TKIP WEP104 WEP40\n\t"
                      "psk=\"%2\"\n}").arg(wifiName).arg(wifiPasswd);
    QByteArray array;
    array.append(configStr);
    //qDebug()<<array;
    QFile file("/etc/wpa_supplicant.conf");
    if(file.exists())
    {
        file.open(QIODevice::WriteOnly|QIODevice::Truncate);//以读写且清空的方式打开
        file.write(array);
        file.close();
    }
    connectWifi(true);
}
/*
 *@brief:   处理软键盘发来的文本
 *@author:  缪庆瑞
 *@date:    2017.5.24
 */
void NetworkingWidget::keyboardSlot(QString text)
{
    tempEdit->setText(text);//将软键盘输入的内容填入点击的edit上
}
/*
 *@brief:   保存静态ip设置
 *@author:  缪庆瑞
 *@date:    2017.5.24
 */
void NetworkingWidget::saveSlot()
{
    QString ipAddrStr = ipAddr->text();
    QStringList ipAddrSegment = ipAddrStr.split(".");//按点分割ip地址段
    if(ipAddrSegment.size()!=4)
    {
        MyMessageBox::information(this,"infomation",tr("ip地址格式不符合规范"));
        //QMessageBox::information(this,"infomation",tr("ip地址格式不符合规范"));
        return;
    }
    else
    {
        bool ok;
        for(int i=0;i<4;i++)
        {
            int ip = ipAddrSegment.at(i).toInt(&ok);
            if(!ok||ip<0||ip>255)
            {
                MyMessageBox::information(this,"infomation",tr("ip地址格式不符合规范2"));
                //QMessageBox::information(this,"infomation",tr("ip地址格式不符合规范2"));
                return;
            }
        }
    }
    QString cmdStr = ipAddrStr.prepend("ifconfig wlan0 ");
    system(cmdStr.toUtf8().data());
}
/*
 *@brief:   事件过滤器处理函数
 *@author:  缪庆瑞
 *@date:    2017.5.24
 *@param:   obj:产生事件的对象
 *@param:   event:产生的事件
 */
bool NetworkingWidget::eventFilter(QObject *obj, QEvent *event)
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
    return QWidget::eventFilter(obj,event);
}
/*
 *@brief:   开启或关闭无线功能
 *@author:  缪庆瑞
 *@date:    2017.5.23
 */
void NetworkingWidget::wifiSwitchSlot()
{
    if(isWifiOn)//关闭wifi
    {
        //system("ifconfig wlan0 down");//一旦关闭再次开启就会造成wifi连不上
        //system("wpa_cli terminate");
        isWifiOn = !isWifiOn;
        scanBtn->setEnabled(false);
        switchBtn->setStyleSheet("border-image: url(:/images/WIFI_OFF.ico);outline: none;");
        currentConnection->setItemValue(0,0,"");
        selectableConnection->setRowCount(0);
    }
    else //开启wifi
    {
        if(system("ifconfig wlan0 up"))//执行成功返回0
        {
            qDebug()<<"wireless start failed!!!";
            MyMessageBox::myInformation(this,tr("Start Wifi Failed"),tr("请接入无线网卡!!!"));
            return ;
        }
        else
        {
            isWifiOn = !isWifiOn;
            scanBtn->setEnabled(true);
            switchBtn->setStyleSheet("border-image: url(:/images/WIFI_ON.ico);outline: none;");
            /*如果wpa_supplicant后台服务已开启，则返回0，就不再次开启，因为再次开启会导致
            无线网卡不能正常连接*/
            if(system("wpa_cli status"))
            {
                system("wpa_supplicant -B -Dwext -i wlan0 -c /etc/wpa_supplicant.conf");//后台工作
            }
            QTimer::singleShot(2000,this,SLOT(scanWifi()));
        }
    }
}
/*
 *@brief:   DHCP或静态ip设置切换
 *@author:  缪庆瑞
 *@date:    2017.5.23
 *@param:   id点击的单选按钮id
 */
void NetworkingWidget::staticipOrDHCPSlot(int id)
{
    if(id) //id ==1
    {
        isDHCPOn = false;
        staticFrame->setEnabled(true);
        saveBtn->setEnabled(true);
    }
    else
    {
        isDHCPOn = true;
        staticFrame->setEnabled(false);
        saveBtn->setEnabled(false);
    }
}


