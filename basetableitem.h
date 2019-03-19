/****************************************************************************
**
** Copyright (C)  2016 Aotai Electric Co.,LTD   山东奥太电气有限公司
** All rights reserved.
**
****************************************************************************/
/*
 *@file:   basetableitem.h
 *@author: 缪庆瑞
 *@date:   2016.8.8
 *@brief:  basetableitem的头文件
 */
#ifndef BASETABLEITEM_H
#define BASETABLEITEM_H
#include <QString>
#include <QWidget>

class BaseTableItem
{
public:
    enum ItemContentType{
                    TEXT,  //只设置文本
                    CHECKBOX_TEXT,   //带复选框的文本
                    ICON_TEXT,   //带图标的文本
                    WIDGET,     //设置一个控件
                    WIDGET_WIDGET    //设置两个控件
    };
    BaseTableItem();
    BaseTableItem(ItemContentType itemContentType,QString text,QString iconAddr=NULL,QWidget *widget1=NULL,QWidget *widget2=NULL);

    QString text;//存放单元格显示文本
    QString iconAddr;//存放图标路径
    QWidget *widget1;//存放部件
    QWidget *widget2;
    ItemContentType itemContentType;//存放item类型

};

#endif // BASETABLEITEM_H
