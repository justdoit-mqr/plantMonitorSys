/*
 *@file:   wificonndialog.h
 *@author: 缪庆瑞
 *@date:   2017.5.24
 *@brief:  连接wifi输入密码的界面
 */
#ifndef WIFICONNDIALOG_H
#define WIFICONNDIALOG_H

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QEvent>
#include "globalvar.h"
#include "mymessagebox.h"

class WifiConnDialog : public QDialog
{
    Q_OBJECT
public:
    WifiConnDialog(QString name,QWidget *parent);
    ~WifiConnDialog();

public slots:
    void okSlot();
    void cancelSlot();
    void keyboardSlot(QString text);//处理软键盘信号的槽

signals:
    void connectingSignal(QString wifiName,QString wifiPasswd);

protected:
    virtual bool eventFilter(QObject *obj,QEvent *event);

private:
    QLineEdit *tempEdit;//用来记录当前引发事件过滤器处理的edit部件
    QLineEdit *wifiName;
    QLineEdit *wifiPasswd;
    QPushButton *okBtn;//确定按钮
    QPushButton *cancelBtn;//取消
};

#endif // WIFICONNDIALOG_H
