/****************************************************************************
**
** Copyright (C)  2016 Aotai Electric Co.,LTD   山东奥太电气有限公司
** All rights reserved.
**
****************************************************************************/
/*
 *@file:   mymessagebox.h
 *@author: 缪庆瑞
 *@date:   2017.5.4
 *@brief:  消息弹出窗,在QMessageBox上封装，方便自由的调整弹出框部件大小及样式
 */
#ifndef MYMESSAGEBOX_H
#define MYMESSAGEBOX_H

#include <QWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QDebug>

class MyMessageBox : public QMessageBox
{
    Q_OBJECT

public:
    MyMessageBox();
    ~MyMessageBox();
    //该方法类似于information()静态方法，只是修改了下字体和按钮大小
    static StandardButton myInformation(QWidget * parent, const QString & title, const QString & text, int fontSize=20,StandardButton button = Ok);
    //该方法类似于critical()静态方法，只是修改了下字体和按钮大小
    static StandardButton myCritical(QWidget * parent, const QString & title, const QString & text, int fontSize=20,StandardButton button = Cancel);
protected:
    //void showEvent(QShowEvent *event);//QMessageBox默认不能自定义大小，由showevent()强制改变成QT认为合适的大小
};

#endif // MYMESSAGEBOX_H
