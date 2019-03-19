/*
 *@file:   infoshowwidget.h
 *@author: 缪庆瑞
 *@date:   2017.3.20
 *@brief:  信息展示界面，包括设备参数，视频监控等
 */
#ifndef INFOSHOWWIDGET_H
#define INFOSHOWWIDGET_H

#include <QWidget>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QAbstractItemView>
#include <QScrollBar>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QHash>
#include <QTabWidget>
#include <QThread>
#include "customplotwidget.h"
#include "mymessagebox.h"
#include "videomonitorwidget.h"
#include "globalvar.h"

class InfoShowWidget : public QWidget
{
    Q_OBJECT
public:
    explicit InfoShowWidget(QWidget *parent = 0);
    ~InfoShowWidget();
    void initEquipmentParamArea();//初始化设备参数区域
    void initTabWidget();//初始化曲线绘制与视频监控区域
    void initEquipmentNoBox();//初始化设备号组
    void changeComboBoxIndex(QString equipmentNo);//根据文本改变comboBox当前index值

    void setEquipmentStatus(QStringList &list);//设置设备运行状态/电压/电流参数

protected:
    virtual bool eventFilter(QObject *obj, QEvent *event);

signals:
    void updateTableBtnName(QString equipmentNo);

public slots:
    void updateEquipmentNoBox(QStringList equipmentNoList);//更新设备号组
    void setHashData(QString equipmentNo,QStringList equipmentDataList);//设置hash键值对
    void refreshDisplay(QString equipmentNo);//根据设备号刷新界面参数显示

    void alterParamSlot();//修改参数
    void updateParamSlot();//更新参数

    void keyboardSlot(QString text);//处理软键盘信号的槽

private:
    //QString equipmentNo;//设备号 整个界面的信息都是根据对应的设备号从数据库和客户端获取的
    QStringList equipmentNoList;//设备号组
    //设备信息哈希表，键为设备号，值为设备信息的一个列表（运行状态，电压，电流等）
    QHash<QString,QStringList> equipmentInfoHash;
    //设备参数部件
    QLineEdit *tempEdit;//用来记录当前引发事件过滤器处理的edit部件
    QGroupBox *equipmentParamBox;//设备参数部件容器
    QLabel *titleLabel;//显示名字
    QPushButton *alterParamBtn;//修改设备参数  参数可编辑
    QPushButton *updateParamBtn;//更新设备参数到数据库

    QPushButton *equipmentNoBtn;//设备号
    QComboBox *equipmentNoBox;
    QPushButton *equipmentNameBtn;//设备名
    QLineEdit *equipmentNameEdit;
    QPushButton *equipmentAddrBtn;//设备ip地址
    QLineEdit *equipmentAddrEdit;
    QPushButton *equipmentStatusBtn;//状态
    QLineEdit *equipmentStatusEdit;
    QPushButton *equipmentPresetVBtn;//预置电压
    QLineEdit *equipmentPresetVEdit;
    QPushButton *equipmentVoltageBtn;//电压
    QLineEdit *equipmentVoltageEdit;
    QPushButton *equipmentPresetABtn;//预置电流
    QLineEdit *equipmentPresetAEdit;
    QPushButton *equipmentCurrentBtn;//电流
    QLineEdit *equipmentCurrentEdit;

    //曲线绘制与视频监控部件
    QTabWidget *tabWidget;//将曲线绘制与视频监控放在同一页面，使用选项卡部件的切换
    CustomPlotWidget *customPlotWidget;//曲线绘制界面
    VideoMonitorWidget *videoMonitorWidget;//视频监控界面

    /*定义数据库对象，此时是无效的，通过静态方法database(connectionName)获取已有的数据库连接
    操作同一数据库*/
    QSqlDatabase db;
};

#endif // INFOSHOWWIDGET_H
