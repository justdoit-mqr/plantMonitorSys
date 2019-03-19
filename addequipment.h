/*
 *@file:   addequipment.h
 *@author: 缪庆瑞
 *@date:   2017.3.31
 *@brief:  添加设备按钮的对话框
 */
#ifndef ADDEQUIPMENT_H
#define ADDEQUIPMENT_H
#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QEvent>
#include <QDebug>
#include "mymessagebox.h"
#include "globalvar.h"

class AddEquipment : public QDialog
{
    Q_OBJECT
public:
    AddEquipment(QWidget *parent = 0);
    ~AddEquipment();
    void setArea(QString Area);

signals:
    void addSuccessSignal();

public slots:
    void okSlot();
    void cancelSlot();
    void keyboardSlot(QString text);//处理软键盘信号的槽

protected:
    virtual bool eventFilter(QObject *obj,QEvent *event);

private:
    QString Area;//标示设备区域
    QLineEdit *tempEdit;//用来记录当前引发事件过滤器处理的edit部件
    QLineEdit *equipmentNoEdit;//设备号
    QLineEdit *equipmentNameEdit;//设备名
    QLineEdit *equipmentAddrEdit;//设备ip
    QLineEdit *equipmentPresetVEdit;//设备给定电压
    QLineEdit *equipmentPresetAEdit;//设备给定电流
    QPushButton *okBtn;//确定按钮
    QPushButton *cancelBtn;//取消

    /*定义数据库对象，此时是无效的，通过静态方法database(connectionName)获取已有的数据库连接
    操作同一数据库*/
    QSqlDatabase db;
};

#endif // ADDEQUIPMENT_H
