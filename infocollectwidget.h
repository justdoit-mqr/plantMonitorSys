/*
 *@file:   infocollectwidget.h
 *@author: 缪庆瑞
 *@date:   2017.4.23
 *@brief:  信息汇总界面，包括设备状态、运行时长、报警信息等统计信息
 */
#ifndef INFOCOLLECTWIDGET_H
#define INFOCOLLECTWIDGET_H

#include <QWidget>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QDate>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDriver>
#include "basetablewidget.h"
#include "mymessagebox.h"

class InfoCollectWidget : public QWidget
{
    Q_OBJECT
public:
    explicit InfoCollectWidget(QWidget *parent = 0);
    ~InfoCollectWidget();
    void initStatusBox();//初始化设备状态汇总区域
    void initRuntimesBox();//初始化运行时长汇总区域
    void initWarningBox();//初始化警报信息汇总区域

    void initEquipmentNoBox();//根据数据库初始化设备号列表

signals:

public slots:
    void setStatusCount(int all,int offline,int standby,int run,int warning);
    void updateEqmNoBox(QStringList list);//更新设备号列表
    void refreshRunTimesSlot();//刷新运行(待机)时长显示
    void queryWarningInfoSlot();//查询指定的报警信息

private:
    //设备状态汇总区
    QGroupBox *equipmentStatusBox;
    QLabel *allCountLabel;//所有设备数
    QLabel *allCount;
    QLabel *offLineCountLabel;//离线设备数
    QLabel *offLineCount;
    QLabel *standByCountLabel;//待机设备数
    QLabel *standByCount;
    QLabel *runCountLabel;//运行设备数
    QLabel *runCount;
    QLabel *warningCountLabel;//报警设备数
    QLabel *warningCount;
    //设备运行时长汇总区
    QGroupBox *equipmentRunTimesBox;
    QLabel *equipmentNoLabel;//设备号
    QComboBox *equipmentNoBox;
    QPushButton *refreshBtn;//刷新
    QLabel *standByTimesOfDayLabel;//日待机时长
    QLabel *standByTimesOfDay;
    QLabel *runTimesOfDayLabel;//日运行时长
    QLabel *runTimesOfDay;
    //设备警报信息汇总区
    QGroupBox *equipmentWarningBox;
    QWidget *historyQueryWidget;//查询指定日期/设备的部件
    QLabel *dateLable;//日期
    QComboBox *yearBox;
    QComboBox *monthBox;
    QComboBox *dayBox;
    QLabel *equipmentNoLabel2;
    QComboBox *equipmentNoBox2;
    QPushButton *queryBtn;
    BaseTableWidget *warningTableWidget;//警报信息展示表格

    /*定义数据库对象，此时是无效的，通过静态方法database(connectionName)获取已有的数据库连接
    操作同一数据库*/
    QSqlDatabase db;
};

#endif // INFOCOLLECTWIDGET_H
