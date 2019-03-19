/****************************************************************************
**
** Copyright (C)  2016 Aotai Electric Co.,LTD   山东奥太电气有限公司
** All rights reserved.
**
****************************************************************************/
/*
 *@file:   basetablewidget.h
 *@author: 缪庆瑞
 *@date:   2016.8.1
 *@brief:  basetablewidget的头文件
 *@update: 2016.9.12修改 对于单元格控件对象不再用数组单独保存，省去删除操作所带来的数组移动时间，对于控件的获取直接用cellwidget（）多个部件使用findchild（）
 */
#ifndef BASETABLEWIDGET_H
#define BASETABLEWIDGET_H
//#define EVENTROW 100 //定义存放对应行列单元格item的二维数组行数

#include "basetableitem.h"
#include <QTableWidget>
#include <QHeaderView>
#include <QScrollBar>
#include <QEvent>
#include <QList>

class BaseTableItem;//类的前置声明，下面要使用

class BaseTableWidget : public QTableWidget
{
    Q_OBJECT
public:
    explicit BaseTableWidget(QWidget *parent=0);
    explicit BaseTableWidget(int rowNum,int columnNum,QList <BaseTableItem *> items,bool rowInit=true,QWidget *parent = 0);//构造指定行数和列数的table,并按顺序初始化单元格
    //explicit 声明为显式构造 不能在隐式转换中使用，规定只传一个参数的构造函数会定义一个隐式转换，隐式转换常带来逻辑错误，所以尽量声明显式
    ~BaseTableWidget();   

    void setItemContent(int row,int column,BaseTableItem *item);//设置Item内容的函数
    void setItemValue(int row,int column,QString text);//设置item文本
    void setColWidthRowHeight(int columnWidth,int rowHeight);//设置表格的列宽和行高，所有行列都一致

    void setNormalPressStyle(QString backgroundColor,QString border,QString textColor);//设置鼠标按下前的样式
    void setPressStyle(QString backgroundColor,QString border,QString textColor);//设置鼠标按下的样式
    void setPressStyle(QString style);//设置按下的样式
    void setReleaseStyle(QString backgroundColor,QString border,QString textColor);//设置鼠标释放后的设置
    void setReleaseStyle(QString style);//设置释放的样式

    void setSelectRow(int row=0,int column=0);//设置默认选择单元格

    //void deleteRow(int row);//删除指定行

    //BaseTableItem *itemsRowCol[EVENTROW][20];//存放对应行列单元格的项,主要是为了给控件加事件过滤监听,不初始化默认各元素为空

protected:
    virtual void mousePressEvent(QMouseEvent *e);//鼠标按下事件处理函数
    virtual void mouseReleaseEvent(QMouseEvent *e);//鼠标释放事件处理函数

    bool eventFilter(QObject *, QEvent *);//事件过滤器处理函数

signals:
    void pressedSignal();//鼠标按下发出的信号
    void releasedSignal();

public slots:
    QString getItemText(int row,int column);//获取选中行指定列值
    QWidget *getItemWidget(int row,int column,int num=1);//获取选中行指定列控件
    int getRowNo();//获取当前行号
    int getColumnNo();//获取当前列号

    void tablePress();//鼠标按下事件发射信号的响应槽
    void tableRelease();//鼠标释放事件发射信号的响应槽
    void itemChangedSlot(QTableWidgetItem *item);//changed 信号的响应槽

private:
    QString pressStyle;//存放设置的鼠标按下的样式
    QString releaseStyle;//存放设置的鼠标释放的样式

    //QList<int> eventRows;//存放安装事件监听的控件所在行号
    //QList<int> eventCols;

};

#endif // BASETABLEWIDGET_H
