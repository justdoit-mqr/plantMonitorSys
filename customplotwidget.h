/*
 *@file:   customplotwidget.h
 *@author: 缪庆瑞
 *@date:   2017.4.10
 *@brief:  绘制图表的类，该类主要调用QCustomPlot曲线组件类的方法绘图
 */
#ifndef CUSTOMPLOTWIDGET_H
#define CUSTOMPLOTWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QButtonGroup>
#include <QComboBox>
#include <QAbstractItemView>
#include <QScrollBar>
#include <QHash>
#include <QTimer>
#include <QSlider>
#include <QtGlobal>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include "qcustomplot/qcustomplot.h"
#include "mymessagebox.h"

class CustomPlotWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CustomPlotWidget(QWidget *parent = 0);
    ~CustomPlotWidget();

    void setHashData(QString equipmentNo,QStringList graphDataList);//设置hash键值对
    void graphing(QString equipmentNo);//根据当前设备号绘制实时曲线图
    void setEquipmentNo(QString equipmentNo);//设置当前的设备号，并刷新实时显示

private:
    void initCustomPlot();//初始化曲线组件QCustomPlot类对象
    void initOtherWidget();//初始化曲线绘制界面的其他控制及显示类部件
    void initHistoryQueryWidget();//初始化历史数据曲线查询的部件

signals:

public slots:
    void refreshTime();//实时数据曲线刷新
    void voltageSliderValueChanged(int value);//滑动条改变的响应槽
    void currentSliderValueChanged(int value);
    void realtimeOrHistoryChanged(int id);//实时数据与历史数据切换
    void queryHistoryDataSlot();//查询指定的历史数据

private:
    QCustomPlot *customPlot;//绘图组件类对象
    QTimer *timer;//定时器 定时刷新实时数据曲线的横坐标(时间)
    bool realtimeFlag;//标记曲线图的实时状态
    QString equipmentNo;//标记当前的设备号
    //图表信息哈希表，键为设备号，值为图表信息的一个列表（电压，电流，时间）
    QHash<QString,QList<QVector<double> > > graphInfoHash;

    //图表绘制界面的其他部件
    QLabel *voltageLabel;
    QLabel *currentLabel;
    QButtonGroup *buttonGroup;//管理实时和历史按钮
    QRadioButton *realtimeBtn;//实时
    QRadioButton *historyBtn;//历史
    QSlider *voltageSlider;//控制电压的范围range
    QSlider *currentSlider;//控制电流的范围

    //历史数据曲线查询的相关部件
    QWidget *historyQueryWidget;
    //QLabel *startTimeLable;//起始时间label
    QComboBox *yearBox;
    QComboBox *monthBox;
    QComboBox *dayBox;
    QComboBox *hourBox;
    QComboBox *minuteBox;
    QLabel *timeIntervalLabel;//时间间隔label
    QComboBox *timeIntervalBox;
    QPushButton *queryBtn;

};

#endif // CUSTOMPLOTWIDGET_H
