/*
 *@file:   softkeyboard.h
 *@author: 缪庆瑞
 *@date:   2016.12.25
 *@brief:  软键盘部件，实现中英文输入
 */
#ifndef SOFTKEYBOARD_H
#define SOFTKEYBOARD_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QBoxLayout>
#include <QMultiHash>
#include <QFile>
#include <QMessageBox>
#include <QRegExp>
#include <QFont>
#include <QTimer>
#include <QString>

class SoftKeyboard : public QWidget
{
    Q_OBJECT
public:
    explicit SoftKeyboard(QWidget *parent = 0);
    ~SoftKeyboard();
    void readDictionary();//读拼音字典，将汉字与拼音的对应存放到hash表中
    void splitPhrase(QString phrase,QString chinese);//拆分拼音词组
    void initStyleSheet();//初始化可选样式表，用于不同的皮肤展示
    void initFirstArea();//初始化第一部分 输入缓存，确定 退出
    void initSecondArea();//初始化第二部分 输入显示区域
    void initThirdArea();//初始化第三部分 按键区域

    void initNumberLetterBtn();//初始化数字字母按键，分配空间，连接信号与槽
    void initSpecialBtn();//初始化特殊功能按键，诸如大小写切换，删除按键等
    void initKeyboardStyle(int num);//初始化键盘样式

    void setLetterLow();//设置小写字母显示
    void setLetterUpper();//设置大写字母显示
    void setSymbolsEN();//设置符号（英文状态）
    void setSymbolsCH();//设置符号（中文状态）

    void matchChinese(QString pinyin);//根据输入的拼音匹配中文
    void displayCandidateWord(int page);//显示指定页的候选词

    void setInputText(QString inputText);//设置输入缓冲区行编辑框文本

signals:
    void sendText(QString text);//以信号的形式将输入文本发出去
public slots:
    void numberLetterBtnSlot();//数字字母(符号)按键被点击的响应槽

    void candidateLetterChangedSlot(QString text);//候选字母改变响应槽
    void candidateWordBtnSlot();//候选词被点击的响应槽
    void candidateWordPrePageSlot();//候选词向前翻页
    void candidateWordNextPageSlot();//候选词向后翻页
    void hideInputDiaplayArea();//隐藏输入显示区域

    void changeUpperLowerSlot();//切换大小写，也可以切换数字字母与符号界面
    void deleteTextSlot();//删除输入
    void closeDelTimer();//关闭连续删除的定时器
    void changeSkinSlot();//切换皮肤
    void changeLetterSymbolSlot();//数字字母与字符切换
    void spaceSlot();//空格被按下时的响应槽
    void changeChEnSlot();//中英文切换
    void enterSlot();//回车被按下的响应槽

    void quitBtnSlot();//退出按钮的响应槽
    void okBtnSlot();//确定按钮的响应槽

private:
    QMultiHash<QString,QString> chinesePinyin;//使用哈希表来存放拼音汉字的键值对 可以一键多值
    QList<QString> hanzi;//存储匹配的汉字词
    int pageCount;//候选词当前页
    bool isENInput;//中英文输入模式
    bool isLetterInput;//数字字母或符号输入模式
    bool isLetterLower;//大小写模式
    int skinNum;//标记当前皮肤编号

    QTimer *delTimer;//实现长按删除键 删除定时

    QVBoxLayout *globalVLayout;
    //第一部分 输入缓存区
    QWidget *textBufferArea;//键盘的输入缓存区
    QLineEdit *lineEdit;
    QPushButton *okBtn;
    QPushButton *quitBtn;
    //第二部分 输入显示区域  中文输入特有区域
    QWidget *inputDisplayArea;//键盘的输入显示区域
    QLineEdit *candidateLetter;//中文输入时对应的字母显示
    QWidget *candidateWordArea;//候选词区域 在上面布局中有六个候选词和翻页按钮
    QPushButton *candidateWordBtn[6];//中文输入时的候选词
    QPushButton *prePageBtn;//前一页
    QPushButton *nextPageBtn;//后一页
    //第三部分 键盘按键区域
    QWidget *keyBoardArea;//键盘的按键区域
    QPushButton *numberLetterBtn[36];//10个数字按键，26个字母按键,同时可以显示符号
    //特殊功能按键
    QPushButton *upperOrLowerBtn;//大小写转换按键
    QPushButton *deleteBtn;//删除按键
    QPushButton *skinBtn;//切换皮肤
    QPushButton *letterOrSymbolBtn;//数字字母与符号切换按键
    QPushButton *commaBtn;//逗号按键
    QPushButton *spaceBtn;//空格按键
    QPushButton *periodBtn;//句号按键
    QPushButton *chOrEnBtn;//中英文切换按键
    QPushButton *enterBtn;//回车按键
    //按键以及整个键盘的样式
    QStringList keyAndCandidateAreaStyle;//按键和候选词widget区域样式
    QStringList commonKeyStyle;//普通按键的样式
    QStringList specialKeyStyle;//特殊按键的样式
};

#endif // SOFTKEYBOARD_H
