/*
 *@file:   globalvar.h
 *@author: 缪庆瑞
 *@date:   2017.5.5
 *@brief:   全局变量声明头文件
 */
#ifndef GLOBALVAR_H
#define GLOBALVAR_H
#include "softkeyboard/softkeyboard.h"

/*声明可以在不同文件使用的全局变量,头文件中用extern 声明，但不可以定义。
在.cpp文件中包含头文件，并在类外定义或者声明(例如类对象声明一次，在类内定义)
一次全局变量，之后直接使用，这样在其他任何包含头文件的文件中都可以使用该全局变量*/
extern SoftKeyboard *softKeyboard;//全局的软键盘对象

#endif // GLOBALVAR_H

