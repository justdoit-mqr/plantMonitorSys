/*
 *@file:   mianwidget.cpp
 *@author: 缪庆瑞
 *@date:   2017.3.16
 *@brief:  项目主界面
 */
#include "mainwidget.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QGridLayout>
#include <QBoxLayout>
#include <QDebug>
//声明全局变量,因为是Qt类，无法在为全局对象申请空间，但至少要声明一次
SoftKeyboard *softKeyboard;

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
{
    softKeyboard = new SoftKeyboard();//为全局变量对象申请空间
    //初始化标题label
    titleLabel = new QLabel(this);
    titleLabel->setText(tr("车间设备监测与控制系统"));
    titleLabel->setFixedHeight(42);//固定高度，不然因为timeBtn样式的原因，
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setFont(QFont("msyh",26));
    titleLabel->setFocus();//焦点设置在标题上

    //初始化时间按钮
    timeBtn = new QPushButton(this);
    connect(timeBtn,SIGNAL(clicked()),this,SLOT(shotScreen()));
    //timeBtn->setEnabled(false);//暂时不可点击
    timeBtn->setFlat(true);//无边界
    timeBtn->setFont(QFont("msyh",18));
    timeBtn->setStyleSheet("padding: -1px;border: -1px;");//消除边框及虚线框
    //timeBtn->setStyleSheet("outline:none;");//消除边框及虚线框 和上一句都可以但略有差别
    timeBtn->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
    QString dateTime = QDateTime::currentDateTime().toString("hh:mm\nyyyy-MM-dd");
    timeBtn->setText(dateTime);
    timeTimer = new QTimer(this);
    connect(timeTimer,SIGNAL(timeout()),this,SLOT(refreshTimeSlot()));
    timeTimer->start(10000);//界面不展示秒，所以不用刷新太快
    //初始化tabwidget
    initTabWidget();
    //整体界面布局
    QHBoxLayout *hBoxLayout = new QHBoxLayout();
    hBoxLayout->setContentsMargins(1,3,1,4);
    hBoxLayout->addWidget(titleLabel,0,Qt::AlignLeft);
    hBoxLayout->addWidget(timeBtn);
    QGridLayout *gridLayout = new QGridLayout(this);
    gridLayout->setContentsMargins(1,1,1,1);
    gridLayout->setSpacing(0);
    gridLayout->addLayout(hBoxLayout,0,0,1,1);
    gridLayout->addWidget(tabWidget,1,0,1,1);

}

MainWidget::~MainWidget()
{

}
/*
 *@brief:   初始化选项卡tabWidget界面
 *@author:  缪庆瑞
 *@date:    2017.3.16
 */
void MainWidget::initTabWidget()
{
    tabWidget = new QTabWidget(this);
    tabWidget->setFocusPolicy(Qt::NoFocus);//不设置焦点，避免虚线框
    tabWidget->setTabPosition(QTabWidget::South);//设置选项在下方
    tabWidget->setFont(QFont("msyh",16));
    plantGlobalWidget = new PlantGlobalWidget(this);
    tabWidget->addTab(plantGlobalWidget,tr("车间总况"));
    infoShowWidget = new InfoShowWidget(this);
    tabWidget->addTab(infoShowWidget,tr("信息展示"));
    infoCollectWidget = new InfoCollectWidget(this);
    tabWidget->addTab(infoCollectWidget,tr("信息汇总"));
    networkingWidget = new NetworkingWidget(this);
    tabWidget->addTab(networkingWidget,tr("网络设置"));

    connect(plantGlobalWidget,SIGNAL(sendEquipmentNoList(QStringList)),infoShowWidget,SLOT(updateEquipmentNoBox(QStringList)));
    connect(plantGlobalWidget,SIGNAL(sendEquipmentNoList(QStringList)),infoCollectWidget,SLOT(updateEqmNoBox(QStringList)));
    connect(plantGlobalWidget,SIGNAL(sendEquipmentData(QString,QStringList)),infoShowWidget,SLOT(setHashData(QString,QStringList)));
    connect(plantGlobalWidget,SIGNAL(currentTabChange(QString)),this,SLOT(changeTabSlot(QString)));
    connect(plantGlobalWidget,SIGNAL(sendEqmCount(int,int,int,int,int)),infoCollectWidget,SLOT(setStatusCount(int,int,int,int,int)));
    connect(infoShowWidget,SIGNAL(updateTableBtnName(QString)),plantGlobalWidget,SLOT(updateTableBtnName(QString)));
    plantGlobalWidget->getEqmStatusCount();//等待信息汇总界面申请空间并关联信号槽后，总况界面获取一次各状态设备数量并发送
    //设置tab选项卡按钮的样式
    tabWidget->setStyleSheet("::tab{"
                             "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                             "stop: 0 #E1E1E1, stop: 0.4 #DDDDDD,stop: 0.5 #D8D8D8, stop: 1.0 #D3D3D3); "
                             "font:  21px \"mysh\";"
                             "width: 100px;"//选项卡按钮宽度
                             "border-style: solid;"//边框样式
                             "border-color: #a3a3a3;"//边框颜色
                             "border-width: 2px 2px 3px 1px;"//边框宽度 上右下左
                             "border-bottom-color: #C2C7CB; "
                             "padding: 14px;}"
                             "::tab:selected{"//选项卡被选中时的状态
                             "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                             "stop: 0 #fafafa, stop: 0.4 #f4f4f4,stop: 0.5 #e7e7e7, stop: 1.0 #fafafa);"
                             "font-size: 23px;"//字体加2号，体现选中的感觉
                             "border-top-width: 0px;"//不显示边框上边框
                             "border-bottom-color: red;"
                             "color:red;}");//红色前景
    //qDebug()<<tabWidget->styleSheet();
}
/*
 *@brief:   定时刷新时间显示
 *@author:  缪庆瑞
 *@date:    2017.3.16
 */
void MainWidget::refreshTimeSlot()
{
    QString dateTime = QDateTime::currentDateTime().toString("hh:mm\nyyyy-MM-dd");
    timeBtn->setText(dateTime);
}
/*
 *@brief:   响应主页车间总况界面的信号，切换选项卡界面
 *@author:  缪庆瑞
 *@date:    2017.3.28
 *@param:   equipmentNo:表示某区域的设备号，进而展示对应的信息
 */
void MainWidget::changeTabSlot(QString equipmentNo)
{
    tabWidget->setCurrentIndex(1);//切换选项卡界面
    infoShowWidget->changeComboBoxIndex(equipmentNo);//根据点击的按钮设置展示的设备号
}
/*
 *@brief:   实现屏幕截图
 *@author:  缪庆瑞
 *@date:    2017.5.31
 */
void MainWidget::shotScreen()
{
    QPixmap savePix = QPixmap::grabWindow(QApplication::desktop()->winId());
    //当前日期时间
    QString dateTimeStr = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    QString dirStr = "/mnt/works/images/";
    QDir dir(dirStr);
    if(!dir.exists())//目录不存在则创建目录
    {
        qDebug()<<"create dir:"<<dirStr;
        dir.mkpath(dirStr);
    }
    QString nameStr = dirStr + dateTimeStr + ".png";
    //保存为图片格式
    savePix.save(nameStr, "PNG");
    qDebug()<<"save screen pic success!"<<endl<<nameStr;
}
