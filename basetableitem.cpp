/****************************************************************************
**
** Copyright (C)  2016 Aotai Electric Co.,LTD   山东奥太电气有限公司
** All rights reserved.
**
****************************************************************************/
/*
 *@file:   basetableitem.cpp
 *@author: 缪庆瑞
 *@date:   2016.8.8
 *@brief:  存放item单元格数据
 */
#include "basetableitem.h"

/*
 *@brief:   定义存放item信息的类
 *@author:  缪庆瑞
 *@date:    2016.8.8
 *@param:   ItemContentType:item的类型
 *@param:   text:item的显示文本
 *@param:   iconAddr:图标文件的路径
 *@param:   widget1:控件1
 *@param:   widget2:控件2
 */
BaseTableItem::BaseTableItem(ItemContentType itemContentType,QString text,QString iconAddr,QWidget *widget1,QWidget *widget2)
{
    this->text=text;//由参数对item对象传递数据
    this->iconAddr=iconAddr;
    this->widget1=widget1;
    this->widget2=widget2;
    this->itemContentType=itemContentType;
}

BaseTableItem::BaseTableItem()
{

}
