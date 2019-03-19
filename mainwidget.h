/*
 *@file:   mainwidget.h
 *@author: 缪庆瑞
 *@date:   2017.3.16
 *@brief:  项目主界面部件
 */
#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QTabWidget>
#include <QTabBar>
#include <QDateTime>
#include <QTimer>
#include "plantglobalwidget.h"
#include "infoshowwidget.h"
#include "infocollectwidget.h"
#include "networkingwidget.h"
#include "globalvar.h"

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(QWidget *parent = 0);
    ~MainWidget();
    void initTabWidget();

signals:

public slots:
    void refreshTimeSlot();//刷新时间
    void changeTabSlot(QString equipmentNo);//响应主页选项卡界面的信号，切换选项卡
    void shotScreen();//截图

private:
    QLabel *titleLabel;//系统名称
    QPushButton *timeBtn;//系统时间
    QTimer *timeTimer;//刷新时间的定时器
    QTabWidget *tabWidget;//实现选项卡式切换界面
    PlantGlobalWidget *plantGlobalWidget;//第一个选项卡界面，车间总况
    InfoShowWidget *infoShowWidget;//第二个选项卡界面，信息展示
    InfoCollectWidget *infoCollectWidget;//第三个选项卡界面，信息汇总
    NetworkingWidget *networkingWidget;//第四个选项卡界面，网络设置

};

#endif // MAINWIDGET_H
