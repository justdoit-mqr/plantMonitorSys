/****************************************************************************
**
** Copyright (C)  2016 Aotai Electric Co.,LTD   山东奥太电气有限公司
** All rights reserved.
**
****************************************************************************/
/*
 *@file:   basetablewidget.cpp
 *@author: 缪庆瑞
 *@date:   2016.8.1
 *@brief:  构建自定义Table组件
 */
#include "basetablewidget.h"
//#include "basetableitem.h"
#include <QDebug>
#include <QPushButton>
#include <QCheckBox>
#include <QFrame>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QMouseEvent>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QEvent>
#include <QMouseEvent>

/*
 *@brief:   自定义Table组件的构造方法
 *@author:  缪庆瑞
 *@date:    2016.8.8
 *@param:   rowNum:初始设置表格的行数
 *@param:   columnNum:初始设置表格的列数
 *@param:   items:按顺序存储初始化的单元格内容
 *@param:   rowInit:表格item按行初始化标志
 *@param:   parent:表格的父部件
 */
BaseTableWidget::BaseTableWidget(int rowNum, int columnNum, QList<BaseTableItem *> items, bool rowInit, QWidget *parent):
    QTableWidget(rowNum,columnNum,parent)
{
    this->setFocusPolicy(Qt::NoFocus);
    int cellCount = items.size();//items实际单元格数量
    if(rowInit)
    {
        //update：2017.3.31
        //当设定的行列数不足以存放items时,自动根据items的个数增加行数
        if(cellCount>rowNum*columnNum)
        {
            rowNum = cellCount/columnNum;
            if(cellCount%columnNum)
            {
                rowNum++;
            }
            this->setRowCount(rowNum);
        }
        for(int i=0;i<rowNum;i++)//按行初始化item
        {
            for(int j=0;j<columnNum&&(i*columnNum+j)<items.size();j++)
            {
                BaseTableItem * tmpItem= items.at(i*columnNum+j);//获取list中对应的item
                setItemContent(i,j,tmpItem);//在指定的行列设置item
            }
        }
    }
    else //update 缪庆瑞 2016.11.23  增加表格初始化时按列初始化items
    {
        //update：2017.3.31
        //当设定的行列数不足以存放items时,自动根据items的个数增加列数
        if(cellCount>rowNum*columnNum)
        {
            columnNum = cellCount/rowNum;
            if(cellCount%rowNum)
            {
                columnNum++;
            }
            this->setColumnCount(columnNum);
        }
        for(int i=0;i<columnNum;i++)//按列初始化item
        {
            for(int j=0;j<rowNum&&(i*rowNum+j)<items.size();j++)
            {
                BaseTableItem * tmpItem= items.at(i*rowNum+j);//获取list中对应的item
                setItemContent(j,i,tmpItem);//在指定的行列设置item
            }
        }
    }
    //this->setSelectRow(0,0);//设置默认选中行
    //信号与槽连接
    //connect(this,SIGNAL(cellClicked(int,int)),this,SLOT(getItemText(int,int)));
    //connect(this,SIGNAL(cellClicked(int,int)),this,SLOT(getItemWidget(int,int)));
    //connect(this,SIGNAL(cellClicked(int,int)),this,SLOT(getRowNo()));
    //connect(this,SIGNAL(cellClicked(int,int)),this,SLOT(getColumnNo()));
    //connect(this,SIGNAL(pressedSignal()),this,SLOT(tablePress()));
    //connect(this,SIGNAL(releasedSignal()),this,SLOT(tableRelease()));
    //connect(this,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(itemChangedSlot(QTableWidgetItem *)));//连接时不加具体参数

    pressStyle="::item:selected {"
            "background-color: white;"
            "border: 2px inset yellow;"
            "color: black;}";//对鼠标不同行为初始化样式
    releaseStyle="::item:selected {"
            "background-color: white;"
            "border: 2px outset orange;"
            "color: black;}";
}

BaseTableWidget::BaseTableWidget(QWidget *parent):
    QTableWidget(parent)
{
    this->setFocusPolicy(Qt::NoFocus);
//    connect(this,SIGNAL(cellClicked(int,int)),this,SLOT(getItemText(int,int)));
//    connect(this,SIGNAL(cellClicked(int,int)),this,SLOT(getItemWidget(int,int)));
//    connect(this,SIGNAL(cellClicked(int,int)),this,SLOT(getRowNo()));
//    connect(this,SIGNAL(cellClicked(int,int)),this,SLOT(getColumnNo()));
//    connect(this,SIGNAL(pressedSignal()),this,SLOT(tablePress()));
//    connect(this,SIGNAL(releasedSignal()),this,SLOT(tableRelease()));
//    connect(this,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(itemChangedSlot(QTableWidgetItem *)));//连接时不加具体参数

    pressStyle="::item:selected {"
            "background-color: white;"
            "border: 2px inset yellow;"
            "color: black;}";//对鼠标不同行为初始化样式
    releaseStyle="::item:selected {"
            "background-color: white;"
            "border: 2px outset orange;"
            "color: black;}";
}

BaseTableWidget::~BaseTableWidget()
{

}
/*
 *@brief:   设置Table单元格item的函数
 *@author:  缪庆瑞
 *@date:    2016.8.9
 *@param:   row: 指定的行
 *@param:   column: 指定的列
 *@param:   item:设置指定的item
 */
void BaseTableWidget::setItemContent(int row,int column,BaseTableItem *item)
{
    this->removeCellWidget(row,column);//移除原有的控件,不然有控件的话无法设置文本
    if(item->itemContentType == BaseTableItem::TEXT)//只有文本
    {
        this->setItem(row,column,new QTableWidgetItem(item->text));
    }
    else if(item->itemContentType == BaseTableItem::CHECKBOX_TEXT)//复选框加文本
    {
        QTableWidgetItem * checkBoxItem =new QTableWidgetItem(item->text);
        checkBoxItem->setCheckState(Qt::Unchecked);//给item本身设置复选框
        this->setItem(row,column,checkBoxItem);
    }
    else if(item->itemContentType == BaseTableItem::ICON_TEXT)//图标加文本
    {
        this->setItem(row,column,new QTableWidgetItem(QIcon(item->iconAddr),item->text));
    }
    else if(item->itemContentType == BaseTableItem::WIDGET)//单元格添加一个控件
    {
        QWidget *widget = new QWidget(this);//添加一个控件也需要将控件放在一个有布局(栅格布局)的widget上加入
        //样式表具有子部件继承性和重设回到默认性，所以为了子部件不使用表格的背景图片，也为子部件设置空的背景图
        widget->setStyleSheet("border-image: url();");//设置自缩放的背景图片样式为空，避免继承表格的背景图样式
        //widget->setAttribute(Qt::WA_TranslucentBackground);//设置透明
        //垂直布局无法对固定大小的部件居中操作
        QGridLayout *layout = new QGridLayout(widget);//因为默认的setCellWidget()不能设置控件的对齐方式,需要用上层的widget与布局调节
        layout->setContentsMargins(2,2,2,2);
        //item->widget1->setObjectName("widget1");//为控件设置objectname，为用findchild查找控件对象
        //item->widget1->installEventFilter(this);//控件在表格上安装过滤器
        layout->addWidget(item->widget1);
        this->setCellWidget(row,column,widget);
    }
    else if(item->itemContentType == BaseTableItem::WIDGET_WIDGET)//单元格添加两个控件
    {
        QWidget *widget = new QWidget(this);
        widget->setStyleSheet("border-image: url();");//设置自缩放的背景图片样式为空，避免继承表格的背景图样式
        QHBoxLayout *layout= new QHBoxLayout(widget);//创建水平布局
        layout->setContentsMargins(2,0,2,0);
        layout->setSpacing(2);
        item->widget1->setObjectName("widget1");//为控件设置objectname，为用findchild查找控件对象
        item->widget2->setObjectName("widget2");
        item->widget1->installEventFilter(this);//控件在表格上安装过滤器
        item->widget2->installEventFilter(this);
        layout->addWidget(item->widget1);//布局上添加控件
        layout->addWidget(item->widget2);
        this->setCellWidget(row,column,widget);
    }
}
/*
 *@brief:   设置item文本，用于响应changed信号
 *@author:  缪庆瑞
 *@date:    2016.8.5
 *@param:   row:指定行
 *@param:   column:指定列
 *@param:   text:设置的文本
 */
void BaseTableWidget::setItemValue(int row, int column, QString text)
{
    this->removeCellWidget(row,column);//移除原有的控件，不然有控件的话无法设置文本
    this->setItem(row,column,new QTableWidgetItem(text));
}
/*
 *@brief:   设置所有行列的列宽和行高
 *@author:  缪庆瑞
 *@date:    2016.11.9
 *@param:   columnWidth:列宽
 *@param:   rowHeight:行高
 */
void BaseTableWidget::setColWidthRowHeight(int columnWidth, int rowHeight)
{
    int rowNum = this->rowCount();//获取行数
    int colNum = this->columnCount();//获取列数
    for(int i=0;i<rowNum;i++)
    {
        this->setRowHeight(i,rowHeight);
    }
    for(int j=0;j<colNum;j++)
    {
        this->setColumnWidth(j,columnWidth);
    }
}
/*
 *@brief:   设置鼠标按下前Table单元格的样式
 *@author:  缪庆瑞
 *@date:    2016.8.3
 *@param:   backgroundColor:指定单元格背景色
 *@param:   border:指定单元格边框形状
 *@param:   textColor:指定文本颜色
 */
void BaseTableWidget::setNormalPressStyle(QString backgroundColor, QString border, QString textColor)
{
    this->setStyleSheet("::item {"
                        "background-color: "+backgroundColor+";"
                        "border: "+border+";"
                        "color: "+textColor+";}"
                        );
}
/*
 *@brief:   设置鼠标按下Table单元格的样式
 *@author:  缪庆瑞
 *@date:    2016.8.3
 *@param:   backgroundColor:指定单元格背景色
 *@param:   border:指定单元格边框形状
 *@param:   textColor:指定文本颜色
 */
void BaseTableWidget::setPressStyle(QString backgroundColor, QString border, QString textColor)
{
    pressStyle = "::item:selected {"
                 "background-color: "+backgroundColor+";"
                 "border: "+border+";"
                 "color: "+textColor+";}";
    /*this->setStyleSheet("::item:selected {"
                        "background-color: "+backgroundColor+";"
                        "border: "+border+";"
                        "color: "+textColor+"};"
                        );*/
}
/*
 *@brief:   设置鼠标按下Table的样式
 *@author:  缪庆瑞
 *@date:    2016.11.11
 *@param:   style:样式
 */
void BaseTableWidget::setPressStyle(QString style)
{
    pressStyle = style;
}
/*
 *@brief:   设置鼠标释放后Table单元格的样式
 *@author:  缪庆瑞
 *@date:    2016.8.4
 *@param:   backgroundColor:指定单元格背景色
 *@param:   border:指定单元格边框形状
 *@param:   textColor:指定文本颜色
 */
void BaseTableWidget::setReleaseStyle(QString backgroundColor, QString border, QString textColor)
{
    releaseStyle = "::item:selected {"
                 "background-color: "+backgroundColor+";"
                 "border: "+border+";"
                                   "color: "+textColor+";}";
}
/*
 *@brief:   设置鼠标释放Table单的样式
 *@author:  缪庆瑞
 *@date:    2016.11.11
 *@param:   style:样式
 */
void BaseTableWidget::setReleaseStyle(QString style)
{
    releaseStyle = style;
}
/*
 *@brief:   鼠标按下事件处理函数
 *@author:  缪庆瑞
 *@date:    2016.8.4
 *@param:   e:传递鼠标事件
 */
void BaseTableWidget::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
    {
        //在arm上，对鼠标按下事件处理中，设置样式使用了占空间较大的背景图片(800k左右)会比较费时，导致表格界面响应缓慢
        //this->setStyleSheet(pressStyle);//设置鼠标按下的样式
        emit pressedSignal();//发送按下的信号
        QTableWidget::mousePressEvent(e);//执行重写前的鼠标按下的处理操作
    }
}
/*
 *@brief:   鼠标释放事件处理函数
 *@author:  缪庆瑞
 *@date:    2016.8.4
 *@param:   e:传递鼠标事件
 */
void BaseTableWidget::mouseReleaseEvent(QMouseEvent *e)
{

    if(e->button() == Qt::LeftButton)
    {
        //this->setStyleSheet(releaseStyle);//设置鼠标释放的样式
        emit releasedSignal();//发送释放的信号
        QTableWidget::mouseReleaseEvent(e);
    }
}
/*
 *@brief:   事件过滤器的处理函数
 *@author:  缪庆瑞
 *@date:    2016.8.10
 *@param:   target:被监视的对象
 *@param:   event:被监控的事件类型
 */
bool BaseTableWidget::eventFilter(QObject *target, QEvent *event)
{
    if(event->type()==QEvent::MouseButtonRelease)//事件类型
    {
        int rowNum=this->rowCount();
        int colNum=this->columnCount();
        for(int i=0;i<rowNum;i++)
        {
            for(int j=0;j<colNum;j++)
            {
                if(this->getItemWidget(i,j)!=NULL)//对所有有控件的单元格进行判断
                {
                    //判断目标是否为监听的对象
                    if(target==getItemWidget(i,j)||target==getItemWidget(i,j,2))
                    {
                        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);//将事件强制转换
                        if(mouseEvent->button() == Qt::LeftButton)
                        {
                            setSelectRow(i,j);//设置选中行
                        }
                    }
                }
            }
        }
    }
    return QTableWidget::eventFilter(target,event);//返回父类的事件过滤器
}
/*
 *@brief:   设置默认选中的单元格
 *@author:  缪庆瑞
 *@date:    2016.8.5
 *@param:   row:指定行
 *@param:   column:指定列
 */
void BaseTableWidget::setSelectRow(int row, int column)
{
    /*if(this->currentItem() != NULL)
    {
        this->currentItem()->setSelected(false);//将之前被选中的item设置未选中状态
    }*/
    int preRow = currentRow();//存放上一次的当前行,解决多个单元格同时被选中
    int preColumn = currentColumn();
    this->setCurrentCell(preRow,preColumn,QItemSelectionModel::Deselect);//将之前的当前行设置未被选中
    this->setCurrentCell(row,column,QItemSelectionModel::Select);//设置单元格被选中
    //this->setStyleSheet(releaseStyle);//设置样式比较费时
    if(preRow!=-1 && preColumn !=-1)//手动设置当前行时，不发送点击信号
    {
        emit cellClicked(row,column);
    }
}
/*
 *@brief:   获取指定单元格文本值
 *@author:  缪庆瑞
 *@date:    2016.8.12
 *@param:   rowNum:指定行
 *@param:   columnNum:指定列
 *@return:  如果单元格存放QTableWidgetItem返回文本,否则返回NULL
 */
QString BaseTableWidget::getItemText(int row, int column)
{
    if(item(row,column)!=NULL)//如果单元格存放的是item，返回其显示的文本值
    {
        //qDebug()<<"当前单元格文本值："<<item(row,column)->text();
        return item(row,column)->text();
    }
    else
    {
        return NULL;
    }
}
/*
 *@brief:   获取指定单元格的指定控件
 *@author:  缪庆瑞
 *@date:    2016.8.12
 *@param:   rowNum:指定行
 *@param:   columnNum:指定列
 *@param:   num:表示返回第几个控件
 *@return:  如果单元格存放控件返回指定控件,否则返回NULL
 */
QWidget *BaseTableWidget::getItemWidget(int row, int column,int num)
{
    if(cellWidget(row,column)!=NULL)
    {
        if(num==1)//返回第一个控件
        {
            QWidget *itemWidget=this->cellWidget(row,column)->findChild<QWidget *>("widget1");
            if(itemWidget==0)
            {
                //return this->cellWidget(row,column);
                return NULL;
            }
            else
            {
                return itemWidget;
            }
            //qDebug()<<"当前单元格控件:"<<itemsRowCol[row][column]->widget1;
            //return itemsRowCol[row][column]->widget1;//返回的是QWidget类型指针，而具体存放的一般是其子类对象
            //所以使用时需要用static_cast强制转换为子类对象指针才能操作子类的相关功能，但这种强制转换不安全(将父类赋值给子类)
            //例:static_cast <BaseButton *>(baseTableWidget->getItemWidget(2,0));
        }
        else if(num==2)
        {
            QWidget *itemWidget=this->cellWidget(row,column)->findChild<QWidget *>("widget2");
            if(itemWidget==0)
            {
                return NULL;
            }
            else
            {
                return itemWidget;
            }
        }
    }
    else
    {
        return NULL;
    }
}
/*
 *@brief:   获取指定单元格行号
 *@author:  缪庆瑞
 *@date:    2016.8.3
 *@return:  返回行号
 */
int BaseTableWidget::getRowNo()
{
    //qDebug()<<"当前行号："<<currentRow();
    return currentRow();
}
/*
 *@brief:   获取指定单元格列号
 *@author:  缪庆瑞
 *@date:    2016.8.3
 *@return:  返回列号
 */
int BaseTableWidget::getColumnNo()
{
    //qDebug()<<"当前列号："<<currentColumn();
    return currentColumn();
}
/*
 *@brief:   鼠标按下事件发射信号的响应槽
 *@author:  缪庆瑞
 *@date:    2016.8.4
 */
void BaseTableWidget::tablePress()
{
    //qDebug()<<"鼠标按下";
}
/*
 *@brief:   鼠标释放事件发射信号的响应槽
 *@author:  缪庆瑞
 *@date:    2016.8.4
 */
void BaseTableWidget::tableRelease()
{
    //qDebug()<<"鼠标释放";
}

/*
 *@brief:   changed信号的响应槽
 *@author:  缪庆瑞
 *@date:    2016.8.4
 *@param:   item:发出changed信号的item
 */
void BaseTableWidget::itemChangedSlot(QTableWidgetItem *item)
{
    if(currentRow()!=-1)//在鼠标第一次点击单元格之前，当前行列为-1
    {
        item->setForeground(QBrush(Qt::red));
    }
    //在该函数中也可以对外部信号做出的改变而产生响应
}

