/*
 *@file:   plantglobalwidget.h
 *@author: 缪庆瑞
 *@date:   2017.3.17
 *@brief:  车间概况总汇部件,通过按钮表示设备，通过颜色表示状态
 */
#ifndef PLANTGLOBALWIDGET_H
#define PLANTGLOBALWIDGET_H

#include <QWidget>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <QThread>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QBoxLayout>
#include "basetablewidget.h"
#include "equipmentinfothread.h"
#include "addequipment.h"
#include "mymessagebox.h"

class PlantGlobalWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PlantGlobalWidget(QWidget *parent = 0);
    ~PlantGlobalWidget();
    void initEquipmentNoBtn();//初始化设备号按钮
    void initStatusGroupAreaC();//初始化设备状态展示组C区
    bool createConnection(QString databaseName,QString connectionName);//创建数据库连接
    void saveDataToDb(QString tableNameSuffix,QString date,QString time,double voltage,int current);//保存实时数据到数据库
    void getEqmStatusCount();//获取各个状态的设备数量

signals:
    //以下信号都是为了实现车间总况和信息展示页面的交流
    void currentTabChange(QString equipmentNo);//改变选项卡界面的信号，参数对应区域的设备号
    void sendEquipmentNoList(QStringList equipmentNoList);//发送从数据库读到的所有设备号
    void sendEquipmentData(QString equipmentNo,QStringList equipmentDataList);//发送设备实时数据到展示页
    //该信号是关联车间总况页面与设备信息采集线程
    void replyDataToClient(QStringList);//告诉服务器线程向客户端回复数据，模拟控制
    //该信号是为了实现车间总况与信息汇总页面的交流
    void sendEqmCount(int all,int offline,int standby,int run,int warning);//向信息汇总界面发送各状态设备数量

public slots:
    void getEquipmentInfoSlot();
    void addEquipmentASlot();
    void addEquipmentBSlot();
    void updateTable();//重新读取数据库更新表格
    void handleEquipmentInfoSlot(QStringList infoList);//处理线程发来的设备信息
    void disconnectSlot(QString ipAddr);//客户端断开连接的处理槽

    void updateTableBtnName(QString equipmentNo);//更新设备的显示名字

private:
    QPushButton *addBtnA;//添加A区设备按钮
    QPushButton *addBtnB;//添加B区设备按钮
    //A区
    QGroupBox *plantAreaA;//车间A区
    QHBoxLayout *hBoxLayoutA;//A区上的布局
    BaseTableWidget *tableWidgetA;//自定义的表格部件,存放区域设备按钮
    QList<BaseTableItem *> itemsA;//存放当前表格中的按钮序列
    //B区
    QGroupBox *plantAreaB;//车间B区
    QHBoxLayout *hBoxLayoutB;//B区上的布局
    BaseTableWidget *tableWidgetB;
    QList<BaseTableItem *> itemsB;
    //C区
    QWidget *statusGroupAreaC;//设备状态展示组C区
    QStringList statusColor;//存储一组对应不同状态的颜色 按钮
    QStringList editStatusColor;//存储一组对应不同状态的颜色 编辑框
    QPushButton *statusBtn[4];
    QLabel  *statusLabel[4];

    EquipmentInfoThread *equipmentInfothread;//运行在子线程中的对象
    QThread *threadA;//开启子线程

    /*数据库管理对象  不需要将数据库crud操作封装成类，因为QsqlQuery本身使用就比较方便，
     且封装成类对select操作的返回值还需处理，有点多余，故直接用数据库对象创建连接后，通过
    QsqlQuery执行sql命令*/
    QSqlDatabase db;

    AddEquipment *addEquipmentDialog;//添加设备的对话框
};

#endif // PLANTGLOBALWIDGET_H
