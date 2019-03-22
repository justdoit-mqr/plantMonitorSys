/*
 *@file:   mymessagebox.h
 *@author: 缪庆瑞
 *@date:   2017.5.4
 *@brief:  消息弹出窗,在QMessageBox上封装，方便自由的调整弹出框部件大小及样式
 */
#include "mymessagebox.h"

MyMessageBox::MyMessageBox()
{

}

MyMessageBox::~MyMessageBox()
{

}
/*
 *@brief:   information提示框封装，改变字体和部件大小
 *@author:  缪庆瑞
 *@date:    2017.5.4
 *@param:   parent:父对象
 *@param:   title:标题
 *@param:   text:展示文本
 *@param:   fontSize:对话框字体大小
 *@param:   button:默认添加的标准按钮 ok
 */
QMessageBox::StandardButton MyMessageBox::myInformation(QWidget *parent, const QString &title, const QString &text, int fontSize, QMessageBox::StandardButton button)
{
    QMessageBox messageBox(parent);
    messageBox.setFont(QFont("msyh",fontSize));//字体
    messageBox.setWindowTitle(title);//标题
    messageBox.setText(text);//展示文本
    QPushButton *btn;
    btn = messageBox.addButton(button);//交互按钮
    btn->setFont(QFont("msyh",fontSize));
    btn->setFixedSize(100,40);
    messageBox.setIcon(QMessageBox::Information);//设置系统的information图片
    messageBox.exec();//模态阻塞执行，直到窗口被关闭，或者按钮被点击
    return button;
}
/*
 *@brief:   critical危险提示框封装，改变字体和窗体大小
 *@author:  缪庆瑞
 *@date:    2017.5.4
 *@param:   parent:父对象
 *@param:   title:标题
 *@param:   text:展示文本
 *@param:   fontSize:对话框字体大小
 *@param:   button:默认添加的标准按钮 cancel
 */
QMessageBox::StandardButton MyMessageBox::myCritical(QWidget *parent, const QString &title, const QString &text, int fontSize, QMessageBox::StandardButton button)
{
    QMessageBox messageBox(parent);
    messageBox.setFont(QFont("msyh",fontSize));//字体
    messageBox.setWindowTitle(title);//标题
    messageBox.setText(text);//展示文本
    QPushButton *btn;
    btn = messageBox.addButton(button);//交互按钮
    btn->setFont(QFont("msyh",fontSize));
    btn->setFixedSize(100,40);
    messageBox.setIcon(QMessageBox::Critical);//设置系统的critical图片
    messageBox.exec();//模态阻塞执行，直到窗口被关闭，或者按钮被点击
    return button;
}


