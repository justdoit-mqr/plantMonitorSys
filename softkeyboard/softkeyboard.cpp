/*
 *@file:   softkeyboard.cpp
 *@author: 缪庆瑞
 *@date:   2016.12.25
 *@brief:  软键盘部件，实现中英文输入
 */
#include "softkeyboard.h"
#include <QDebug>
#include <QDateTime>
#define PINYINFILEPATH  "./config/ChinesePinyin"
#define FONTFAMILY "msyh"  //字体族
#define FONTSIZE 24      //字体大小
#define CANDIDATEWORDNUM 6   //中文输入 默认6个候选词

SoftKeyboard::SoftKeyboard(QWidget *parent) : QWidget(parent)
{
    /*设置键盘整体界面的最小大小，因为整体界面添加布局，布局的默认约束为SetDefaultConstraint
    这种约束只针对顶级窗口，会设置顶级窗口的最小大小为布局的minimumsize，而布局的最小大小是由内部的
    部件或子布局的推荐大小决定，所以设置窗口的最小大小，可以避免布局推荐大小过大，整个窗口无法缩小,只能
    通过setFixesize来固定不可变的大小*/
    this->setMinimumSize(500,300);
    //this->showFullScreen();
    this->resize(800,480);//默认大小
    //初始化ui显示
    this->initStyleSheet();
    this->initFirstArea();
    this->initSecondArea();
    this->initThirdArea();
    this->setLetterLow();//默认显示字母界面 小写
    this->initKeyboardStyle(0);
    //整体垂直布局
    globalVLayout = new QVBoxLayout(this);
    globalVLayout->setMargin(1);
    globalVLayout->setSpacing(0);
    globalVLayout->addWidget(textBufferArea,1);//布局中添加部件，并设置伸缩比例关系
    globalVLayout->addStretch(1);
    globalVLayout->addWidget(inputDisplayArea,1);
    globalVLayout->addWidget(keyBoardArea,8);

    skinNum = 0;
    isENInput = true;//初始化为英文输入
    isLetterInput = true;//初始化为字母界面
    isLetterLower = true;//小写字母
    readDictionary();//读拼音字典
}

SoftKeyboard::~SoftKeyboard()
{

}
/*
 *@author:  缪庆瑞
 *@date:    2016.12.29
 *@brief:   读拼音字典，将汉字与对应拼音存放到hash表中,也可以是QMap中，但不考虑排列顺序时，hash更快
 */
void SoftKeyboard::readDictionary()
{
    //通过打印时间测试读拼音文件的效率 8G内存windows测试大约用1/10s
    //qDebug()<<QDateTime::currentDateTime().toString("yyyy-MM-dd HH:m:s:z");
    QFile pinyinFile(PINYINFILEPATH);
    if(!pinyinFile.open(QIODevice::ReadOnly))//拼音文件打开失败提示
    {
        QMessageBox::critical(this,tr("Open File Failed"),tr("无法打开拼音文件。。。"));
    }
    else
    {
        QRegExp regExp("[a-z']+");//正则表达式，匹配1个或多个由a-z及 ' 组成的字母串，默认区分大小写
        QString lineText;//存放读取的一行数据 汉字-拼音
        QString linePinyin;//存放正则表达式匹配的拼音
        QString lineChinese;//存放拼音对应的汉字
        int pinyinPosition;//每一行匹配拼音的位置
        while(!pinyinFile.atEnd())//while循环读取拼音文件，直到读完
        {
            lineText = QString(tr(pinyinFile.readLine()));
            pinyinPosition=regExp.indexIn(lineText,0);//获取读取行的文本中匹配正则表达式的位置
            linePinyin = regExp.cap(0);//regExp.cap(0)表示完整正则表达式的匹配
            lineChinese = lineText.left(pinyinPosition);//lineText.left(n)可以获取左边那个字符即对应的汉字
            if(linePinyin.contains("'"))//如果有单引号表示是词组，则进行拆分词组
            {
                splitPhrase(linePinyin,lineChinese);
            }
            else//单个汉字
            {
                chinesePinyin.insert(regExp.cap(0),lineText.left(pinyinPosition));//往哈希表插入键值对
            }
        }
    }
    //qDebug()<<QDateTime::currentDateTime().toString("yyyy-MM-dd HH:m:s:z");
    //qDebug()<<chinesePinyin.size();
}
/*
 *@author:  缪庆瑞
 *@date:    2017.2.7
 *@brief:   拆分拼音词组，拼音字典文件词组用'分割，如"ai'qing"该函数的功能便是去掉'，将简拼、
 *              全拼存放到哈希表中
 *@param1:   phrase:要处理的拼音词组
 *@param2:  chinese:拼音对应的汉字
 */
void SoftKeyboard::splitPhrase(QString phrase,QString chinese)
{
    int count = phrase.count("'");
    if(count==1)//两个汉字
    {
        int index=phrase.indexOf("'");
        QString pinyin1=phrase.left(1);//两字首字母简拼 例aq
        pinyin1.append(phrase.at(index+1));
        chinesePinyin.insert(pinyin1,chinese);
        QString pinyin2=phrase.left(index);//全拼+首字母 aiq
        pinyin2.append(phrase.at(index+1));
        if(pinyin2!=pinyin1)//避免同一词组键值对插入哈希表多次 例如 e'xi
        {
            chinesePinyin.insert(pinyin2,chinese);
        }
        QString pinyin3=phrase.remove("'");//全拼 aiqing
        if(pinyin3!=pinyin2)
        {
             chinesePinyin.insert(pinyin3,chinese);
        }
    }
    else if(count==2)//三个汉字
    {
        int index1=phrase.indexOf("'");
        int index2=phrase.indexOf("'",index1+1);
        QString pinyin1=phrase.left(1);//三字首字母简拼
        pinyin1.append(phrase.at(index1+1));
        pinyin1.append(phrase.at(index2+1));
        chinesePinyin.insert(pinyin1,chinese);
        QString pinyin2=phrase.left(index1);//全拼+首字母+首字母
        pinyin2.append(phrase.at(index1+1));
        pinyin2.append(phrase.at(index2+1));
        if(pinyin2!=pinyin1)//避免同一词组键值对插入哈希表多次 例如 e'xi
        {
            chinesePinyin.insert(pinyin2,chinese);
        }
        QString pinyin3=phrase.left(index2);//全拼+全拼+首字母
        pinyin3.append(phrase.at(index2+1));
        pinyin3.remove("'");
        if(pinyin3!=pinyin2)//避免同一词组键值对插入哈希表多次 例如 e'xi
        {
            chinesePinyin.insert(pinyin3,chinese);
        }
        QString pinyin4=phrase.remove("'");//全拼
        if(pinyin4!=pinyin3)//避免同一词组键值对插入哈希表多次 例如 e'xi
        {
            chinesePinyin.insert(pinyin4,chinese);
        }
    }
    else if(count==3)//四个汉字
    {
        int index1=phrase.indexOf("'");
        int index2=phrase.indexOf("'",index1+1);
        int index3=phrase.indexOf("'",index2+1);
        QString pinyin1=phrase.left(1);//四字首字母简拼
        pinyin1.append(phrase.at(index1+1));
        pinyin1.append(phrase.at(index2+1));
        pinyin1.append(phrase.at(index3+1));
        chinesePinyin.insert(pinyin1,chinese);
        QString pinyin2=phrase.left(index1);//全拼+首字母+首字母+首字母
        pinyin2.append(phrase.at(index1+1));
        pinyin2.append(phrase.at(index2+1));
        pinyin2.append(phrase.at(index3+1));
        if(pinyin2!=pinyin1)//避免同一词组键值对插入哈希表多次 例如 e'xing'xun'huan
        {
            chinesePinyin.insert(pinyin2,chinese);
        }
        QString pinyin3=phrase.left(index2);//全拼+全拼+首字母+首字母
        pinyin3.append(phrase.at(index2+1));
        pinyin3.append(phrase.at(index3+1));
        pinyin3.remove("'");
        if(pinyin3!=pinyin2)//避免同一词组键值对插入哈希表多次 例如 e'xing'xun'huan
        {
            chinesePinyin.insert(pinyin3,chinese);
        }
        QString pinyin4=phrase.left(index3);//全拼+全拼+全拼+首字母
        pinyin4.append(phrase.at(index3+1));
        pinyin4.remove("'");
        if(pinyin4!=pinyin3)//避免同一词组键值对插入哈希表多次 例如 e'xing'xun'huan
        {
            chinesePinyin.insert(pinyin4,chinese);
        }
        QString pinyin5=phrase.remove("'");//全拼
        if(pinyin5!=pinyin4)//避免同一词组键值对插入哈希表多次 例如 e'xing'xun'huan
        {
            chinesePinyin.insert(pinyin5,chinese);
        }
    }
}
/*
 *@author:  缪庆瑞
 *@date:    2017.1.12
 *@brief:   初始化样式表，即用于界面皮肤选择
 */
void SoftKeyboard::initStyleSheet()
{
    //当前有三套皮肤，后期可以再添加
    //按键和候选词widget区域样式
    keyAndCandidateAreaStyle<<"background-color:black;color:white;"
                           <<"background-color:#89C8E7;color:black;"
                           <<"background-color:#F8859F;color:black;";
    //普通按键的样式 border-radius:10px;
    commonKeyStyle<<"QPushButton{background-color:gray;border-radius:4px;}"//正常样式
                    "QPushButton:pressed{background-color:#ec8d07;border-radius:4px;}"//按下样式
                    "QPushButton:focus{outline:none;}"//去除虚线框
                           <<"QPushButton{background-color:#D0DFF0;border-radius:4px;}"
                             "QPushButton:pressed{background-color:#ec8d07;border-radius:4px;}"
                             "QPushButton:focus{outline:none;}"//去除虚线框
                           <<"QPushButton{background-color:#F6CED6;border-radius:10px;}"
                             "QPushButton:pressed{background-color:#ec8d07;border-radius:10px;}"
                             "QPushButton:focus{outline:none;}";//去除虚线框
    //特殊按键的样式
    specialKeyStyle<<"QPushButton{background-color:#5D3F3F;border-radius:2px;}"
                     "QPushButton:pressed{background-color:#ec8d07;border-radius:2px;}"
                     "QPushButton:focus{outline:none;}"//去除虚线框
                          <<"QPushButton{background-color:#94B6EF;border-radius:2px;}"
                            "QPushButton:pressed{background-color:#ec8d07;border-radius:2px;}"
                            "QPushButton:focus{outline:none;}"//去除虚线框
                          <<"QPushButton{background-color:#FFA3B8;border-radius:8px;}"
                            "QPushButton:pressed{background-color:#ec8d07;border-radius:8px;}"
                            "QPushButton:focus{outline:none;}";//去除虚线框
}
/*
 *@author:  缪庆瑞
 *@date:    2016.12.25
 *@brief:   初始化第一部分，即编辑栏和确定退出按钮
 */
void SoftKeyboard::initFirstArea()
{
    QFont font(tr(FONTFAMILY),FONTSIZE-2);
    //每一个部分都是由一个widget部件主导，通过布局在该部件添加其他小部件
    textBufferArea = new QWidget();
    textBufferArea->setFixedHeight(85);
    //textBufferArea->setStyleSheet("background-color:orange;");
    QHBoxLayout *hBoxLayout = new QHBoxLayout(textBufferArea);
    hBoxLayout->setContentsMargins(5,16,5,15);//15是为了与中文输入显示区域有一定的间隔
    //键盘输入内容显示部件
    lineEdit = new QLineEdit();
    lineEdit->setFont(font);
    lineEdit->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    QString btnStyleSheet("QPushButton{background-color:lightGray;border-radius:4px;}"//正常样式
                          "QPushButton:pressed{background-color:orange;border-radius:4px;}"//按下样式
                          "QPushButton:focus{outline:none;}");//去除虚线框
    okBtn = new QPushButton();
    okBtn->setFont(font);
    okBtn->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    okBtn->setText(tr("确定"));
    okBtn->setStyleSheet(btnStyleSheet);
    connect(okBtn,SIGNAL(clicked()),this,SLOT(okBtnSlot()));
    quitBtn = new QPushButton();
    quitBtn->setFont(font);
    quitBtn->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    quitBtn->setText(tr("退出"));
    quitBtn->setStyleSheet(btnStyleSheet);
    connect(quitBtn,SIGNAL(clicked()),this,SLOT(quitBtnSlot()));
    hBoxLayout->addWidget(lineEdit,4);
    hBoxLayout->addWidget(quitBtn,1);
    hBoxLayout->addWidget(okBtn,1);
}
/*
 *@author:  缪庆瑞
 *@date:    2016.12.25
 *@brief:   初始化第二部分，输入显示区域，即中文输入时的候选词
 */
void SoftKeyboard::initSecondArea()
{
    QFont font(tr(FONTFAMILY),FONTSIZE);
    inputDisplayArea = new QWidget();
    //inputDisplayArea->setStyleSheet("background-color:green;");
    inputDisplayArea->setVisible(false);//该区域初始化时是不显示的 中文输入时特有区域
    QVBoxLayout *vBoxLayout = new QVBoxLayout(inputDisplayArea);
    vBoxLayout->setContentsMargins(0,0,0,0);
    vBoxLayout->setSpacing(0);
    //提供中文输入时，输入字母的显示
    candidateLetter = new QLineEdit();
    //candidateLetter->setStyleSheet("background-color:gray;");
    candidateLetter->setFrame(false);//不显示边框
    connect(candidateLetter,SIGNAL(textChanged(QString)),this,SLOT(candidateLetterChangedSlot(QString)));
    candidateLetter->setFont(QFont(FONTFAMILY,FONTSIZE));
    candidateLetter->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);//水平方向固定大小
    //candidateLetter->setStyleSheet("text-align:left;");//按钮设置显示文本居左，没有直接方法，使用样式设置

    candidateWordArea = new QWidget();//在其上布局候选词和翻页按钮
    //candidateWordArea->setStyleSheet("background-color:gray;");
    QHBoxLayout *hBoxLayout = new QHBoxLayout(candidateWordArea);
    hBoxLayout->setContentsMargins(9,5,9,0);//布局下方间隔为0，紧挨着键盘输入按键

    //默认有6个待选词
    for(int i=0;i<CANDIDATEWORDNUM;i++)
    {
        candidateWordBtn[i] = new QPushButton();
        candidateWordBtn[i]->setFlat(true);//不显示按钮的边框
        candidateWordBtn[i]->setFont(font);
        candidateWordBtn[i]->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
        connect(candidateWordBtn[i],SIGNAL(clicked()),this,SLOT(candidateWordBtnSlot()));
        hBoxLayout->addWidget(candidateWordBtn[i]);
    }
    //前后翻页按钮
    prePageBtn = new QPushButton();
    prePageBtn->setFlat(true);
    prePageBtn->setFont(font);
    prePageBtn->setText("<-");
    prePageBtn->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    connect(prePageBtn,SIGNAL(clicked()),this,SLOT(candidateWordPrePageSlot()));
    nextPageBtn = new QPushButton();
    nextPageBtn->setFlat(true);
    nextPageBtn->setFont(font);
    nextPageBtn->setText("->");
    nextPageBtn->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    connect(nextPageBtn,SIGNAL(clicked()),this,SLOT(candidateWordNextPageSlot()));
    hBoxLayout->addWidget(prePageBtn);
    hBoxLayout->addWidget(nextPageBtn);

    vBoxLayout->addWidget(candidateLetter);
    vBoxLayout->addWidget(candidateWordArea);
}
/*
 *@author:  缪庆瑞
 *@date:    2016.12.25
 *@brief:   初始化第三部分按键区域
 */
void SoftKeyboard::initThirdArea()
{
    initNumberLetterBtn();//初始化10个数字，26个字母或字符按键
    initSpecialBtn();//初始化特殊按键
    keyBoardArea = new QWidget();
    //keyBoardArea->setStyleSheet("background-color:black;");
    QHBoxLayout *firstRowHLayout = new QHBoxLayout();
    for(int i=0;i<10;i++)//布局第一排按键 10个数字
    {
        firstRowHLayout->addWidget(numberLetterBtn[i]);
    }
    QHBoxLayout *secondRowHLayout = new QHBoxLayout();
    for(int i=10;i<20;i++)//布局第二排按键
    {
        secondRowHLayout->addWidget(numberLetterBtn[i]);
    }
    QHBoxLayout *thirdRowHLayout = new QHBoxLayout();
    thirdRowHLayout->setContentsMargins(20,2,20,2);
    for(int i=20;i<29;i++)//布局第三排按键
    {
        thirdRowHLayout->addWidget(numberLetterBtn[i]);
    }
    QHBoxLayout *fourthRowHLayout = new QHBoxLayout();
    fourthRowHLayout->addWidget(upperOrLowerBtn);//大小写切换按键
    for(int i =29;i<36;i++)//布局第四排按键
    {
        fourthRowHLayout->addWidget(numberLetterBtn[i]);
    }
    fourthRowHLayout->addWidget(deleteBtn);//删除按键
    QHBoxLayout *fifthRowHLayout = new QHBoxLayout();
    //布局第五排按键，基本为特殊功能按键
    fifthRowHLayout->addWidget(skinBtn,2);
    fifthRowHLayout->addWidget(letterOrSymbolBtn,1);
    fifthRowHLayout->addWidget(commaBtn,1);
    fifthRowHLayout->addWidget(spaceBtn,4);
    fifthRowHLayout->addWidget(periodBtn,1);
    fifthRowHLayout->addWidget(chOrEnBtn,1);
    fifthRowHLayout->addWidget(enterBtn,2);

    QVBoxLayout *vBoxlayout = new QVBoxLayout(keyBoardArea);
    vBoxlayout->setContentsMargins(5,5,5,5);
    vBoxlayout->addLayout(firstRowHLayout);
    vBoxlayout->addLayout(secondRowHLayout);
    vBoxlayout->addLayout(thirdRowHLayout);
    vBoxlayout->addLayout(fourthRowHLayout);
    vBoxlayout->addLayout(fifthRowHLayout);
}
/*
 *@author:  缪庆瑞
 *@date:    2016.12.25
 *@brief:   初始化软键盘的数字，字母或字符按键
 */
void SoftKeyboard::initNumberLetterBtn()
{
    QFont font(tr(FONTFAMILY),FONTSIZE);
    //以下36个按键，仅作为普通输入，无其他功能，所以连接一个槽函数
    for(int i=0;i<36;i++)//为10个数字，26个字母按键申请空间,连接信号与槽
    {
        numberLetterBtn[i] = new QPushButton();
        numberLetterBtn[i]->setFont(font);
        //numberLetterBtn[i]->setStyleSheet("background-color:gray;");
        numberLetterBtn[i]->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
        connect(numberLetterBtn[i],SIGNAL(clicked()),this,SLOT(numberLetterBtnSlot()));
    }
}
/*
 *@author:  缪庆瑞
 *@date:    2016.12.26
 *@brief:   初始化软键盘的特殊功能按键
 */
void SoftKeyboard::initSpecialBtn()
{
    QFont font(tr(FONTFAMILY),FONTSIZE);
    upperOrLowerBtn = new QPushButton();
    upperOrLowerBtn->setFont(font);
    upperOrLowerBtn->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    upperOrLowerBtn->setText(tr("A/a"));
    connect(upperOrLowerBtn,SIGNAL(clicked()),this,SLOT(changeUpperLowerSlot()));

    deleteBtn = new QPushButton();
    deleteBtn->setFont(font);
    deleteBtn->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    deleteBtn->setText("del");
    //删除按钮按下即会删除
    connect(deleteBtn,SIGNAL(pressed()),this,SLOT(deleteTextSlot()));
    connect(deleteBtn,SIGNAL(released()),this,SLOT(closeDelTimer()));
    delTimer = new QTimer(this);
    connect(delTimer,SIGNAL(timeout()),this,SLOT(deleteTextSlot()));

    skinBtn = new QPushButton();
    skinBtn->setFont(font);
    skinBtn->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    skinBtn->setText(tr("  皮肤  "));
    connect(skinBtn,SIGNAL(clicked()),this,SLOT(changeSkinSlot()));

    letterOrSymbolBtn = new QPushButton();
    letterOrSymbolBtn->setFont(font);
    letterOrSymbolBtn->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    letterOrSymbolBtn->setText(tr("ab/符"));
    //letterOrSymbolBtn->setStyleSheet("background-color:gray;");
    connect(letterOrSymbolBtn,SIGNAL(clicked()),this,SLOT(changeLetterSymbolSlot()));

    commaBtn = new QPushButton();
    commaBtn->setMinimumSize(36,16);//设置最小大小，主要是为了清掉布局的默认最佳大小
    commaBtn->setFont(font);
    commaBtn->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    commaBtn->setText(",");
    //commaBtn->setStyleSheet("background-color:gray;");
    connect(commaBtn,SIGNAL(clicked()),this,SLOT(numberLetterBtnSlot()));

    spaceBtn = new QPushButton();
    spaceBtn->setText(" ");
    spaceBtn->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    //spaceBtn->setStyleSheet("background-color:gray;");
    connect(spaceBtn,SIGNAL(clicked()),this,SLOT(spaceSlot()));

    periodBtn = new QPushButton();
    periodBtn->setMinimumSize(36,16);
    periodBtn->setFont(font);
    periodBtn->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    periodBtn->setText(".");
    //periodBtn->setStyleSheet("background-color:gray;");
    connect(periodBtn,SIGNAL(clicked()),this,SLOT(numberLetterBtnSlot()));

    chOrEnBtn = new QPushButton();
    chOrEnBtn->setFont(font);
    chOrEnBtn->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    chOrEnBtn->setText(tr("   英   "));
    //chOrEnBtn->setStyleSheet("background-color:gray;");
    connect(chOrEnBtn,SIGNAL(clicked()),this,SLOT(changeChEnSlot()));

    enterBtn = new QPushButton();
    enterBtn->setFont(font);
    enterBtn->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    enterBtn->setText("  Enter  ");
    connect(enterBtn,SIGNAL(clicked()),this,SLOT(enterSlot()));
}
/*
 *@author:  缪庆瑞
 *@date:    2017.1.12
 *@brief:   初始化键盘样式，所有按键及区域背景的样式在此设置
 *@param:   num：确定用哪一套样式（皮肤）
 */
void SoftKeyboard::initKeyboardStyle(int num)
{
    if(num>=keyAndCandidateAreaStyle.size())
    {
        qDebug()<<"暂无该套皮肤";
        return;//避免超出list长度，程序异常
    }
    //用于候选词和按键widget区域样式
    QString tmpStyle = keyAndCandidateAreaStyle.at(num);
    candidateLetter->setStyleSheet(tmpStyle);//候选字母栏样式
    candidateWordArea->setStyleSheet(tmpStyle);//候选词区域
    keyBoardArea->setStyleSheet(tmpStyle);//键盘按键区域
    //用于普通按键的样式
    tmpStyle = commonKeyStyle.at(num);
    for(int i=0;i<36;i++)//36个普通输入按键样式
    {
        numberLetterBtn[i]->setStyleSheet(tmpStyle);
    }
    letterOrSymbolBtn->setStyleSheet(tmpStyle);//几个特殊按键同样设置成普通样式
    commaBtn->setStyleSheet(tmpStyle);
    spaceBtn->setStyleSheet(tmpStyle);
    periodBtn->setStyleSheet(tmpStyle);
    chOrEnBtn->setStyleSheet(tmpStyle);
    //用于特殊按键的样式
    tmpStyle = specialKeyStyle.at(num);
    upperOrLowerBtn->setStyleSheet(tmpStyle);
    deleteBtn->setStyleSheet(tmpStyle);
    skinBtn->setStyleSheet(tmpStyle);
    enterBtn->setStyleSheet(tmpStyle);
}
/*
 *@author:  缪庆瑞
 *@date:    2016.12.25
 *@brief:   设置小写字母显示
 */
void SoftKeyboard::setLetterLow()
{
    QStringList letterLowList;
    letterLowList<<"1"<<"2"<<"3"<<"4"<<"5"<<"6"<<"7"<<"8"<<"9"<<"0"
                 <<"q"<<"w"<<"e"<<"r"<<"t"<<"y"<<"u"<<"i"<<"o"<<"p"<<"a"<<"s"<<"d"
                 <<"f"<<"g"<<"h"<<"j"<<"k"<<"l"<<"z"<<"x"<<"c"<<"v"<<"b"<<"n"<<"m";
    for(int i=0;i<36;i++)
    {
        numberLetterBtn[i]->setText(letterLowList.at(i));
    }
}
/*
 *@author:  缪庆瑞
 *@date:    2016.12.25
 *@brief:   设置大写字母显示
 */
void SoftKeyboard::setLetterUpper()
{
    QStringList letterUpperList;
    letterUpperList<<"1"<<"2"<<"3"<<"4"<<"5"<<"6"<<"7"<<"8"<<"9"<<"0"
                   <<"Q"<<"W"<<"E"<<"R"<<"T"<<"Y"<<"U"<<"I"<<"O"<<"P"<<"A"<<"S"<<"D"
                   <<"F"<<"G"<<"H"<<"J"<<"K"<<"L"<<"Z"<<"X"<<"C"<<"V"<<"B"<<"N"<<"M";
    for(int i=0;i<36;i++)
    {
        numberLetterBtn[i]->setText(letterUpperList.at(i));
    }
}
/*
 *@author:  缪庆瑞
 *@date:    2016.12.31
 *@brief:   设置符号显示，包含所以可输入符号(英文状态)
 */
void SoftKeyboard::setSymbolsEN()
{
    QStringList symbolsENList;
    symbolsENList<<"!"<<"@"<<"#"<<"$"<<"%"<<"^"<<"&&"<<"*"<<"("<<")"
               <<"["<<"]"<<"{"<<"}"<<"<"<<">"<<"+"<<"-"<<"_"<<"="
               <<"`"<<"~"<<"|"<<"\\"<<"'"<<"\""<<":"<<";"<<"?"
               <<"/"<<"www."<<".com"<<".cn"<<".net"<<tr("→")<<tr("←");
    for(int i=0;i<36;i++)
    {
        numberLetterBtn[i]->setText(symbolsENList.at(i));
    }
}
/*
 *@author:  缪庆瑞
 *@date:    2016.12.31
 *@brief:   设置符号显示，包含所有可输入符号(中文状态)
 */
void SoftKeyboard::setSymbolsCH()
{
    QStringList symbolsCHList;
    //中文状态下的字符用tr编码转换，实现Qt4的兼容，Qt4使用QTextCodec实现utf-8编码中文
    symbolsCHList<<tr("！")<<tr("@")<<tr("#")<<tr("￥")<<tr("%")<<tr("……")<<tr("&&")<<tr("*")<<tr("（")<<tr("）")
                <<tr("【")<<tr("】")<<tr("{")<<tr("}")<<tr("《")<<tr("》")<<tr("+")<<tr("-")<<tr("——")<<tr("=")
                <<tr("·")<<tr("~")<<tr("|")<<tr("、")<<tr("’")<<tr("‘")<<tr("“")<<tr("”")<<tr("：")
                <<tr("；")<<tr("？")<<tr("★")<<tr("℃")<<tr("囍")<<tr("↑")<<tr("↓");
    for(int i=0;i<36;i++)
    {
        numberLetterBtn[i]->setText(symbolsCHList.at(i));
    }
}
/*
 *@author:  缪庆瑞
 *@date:    2017.1.1
 *@brief:   根据输入的拼音匹配中文
 */
void SoftKeyboard::matchChinese(QString pinyin)
{
    hanzi.clear();//每次匹配中文都先清空之前的列表
    //哈希表chinesePinyin中存放着拼音-汉字的键值对（一键多值），获取对应拼音的汉字列表
    hanzi = chinesePinyin.values(pinyin);
    //qDebug()<<hanzi;
    pageCount = 1;//每次匹配，候选词初始化显示第一页
}
/*
 *@author:  缪庆瑞
 *@date:    2017.1.1
 *@brief:   显示指定页的候选词 page从1开始
 */
void SoftKeyboard::displayCandidateWord(int page)
{
    int hanziCount = hanzi.size();//匹配的汉字列表个数
    if(page == 1)//当显示第一页是向前翻页按钮不可点击
    {
        prePageBtn->setEnabled(false);
    }
    else
    {
        prePageBtn->setEnabled(true);
    }
    if(page*CANDIDATEWORDNUM>=hanziCount)
    {
        nextPageBtn->setEnabled(false);
    }
    else
    {
        nextPageBtn->setEnabled(true);
    }
    //因为哈希表获取指定的key对应的值列表时，是按照后插入的先获取（后进先出），所以常见字在后面，
    //需要从列表的最后一个反向获取显示
    int num = hanziCount-1-(page-1)*CANDIDATEWORDNUM;
    for(int i=0;i<CANDIDATEWORDNUM;i++)//每页6个候选字词
    {
        if((page-1)*CANDIDATEWORDNUM+i<hanziCount)
        {
            candidateWordBtn[i]->setText(hanzi.at(num-i));
        }
        else
        {
            candidateWordBtn[i]->setText("");//清除上一页的缓存
        }
    }
}
/*
 *@author:  缪庆瑞
 *@date:    2017.5.5
 *@brief:   设置输入缓存区编辑行的文本
 */
void SoftKeyboard::setInputText(QString inputText)
{
    lineEdit->setText(inputText);
}
/*
 *@author:  缪庆瑞
 *@date:    2016.12.25
 *@brief:   字母(符号)按键被点击的响应槽
 */
void SoftKeyboard::numberLetterBtnSlot()
{
    QPushButton *clickedBtn = qobject_cast<QPushButton *>(sender());//获取信号发送者的对象
    if(isENInput||!isLetterInput)//英文输入模式或者字符输入界面
    {
        if(clickedBtn->text()=="&&")//因为可显示控件把&符号当成快捷键标志，一个不显示，所以这个要做下特别处理
        {
            lineEdit->insert("&");
            lineEdit->setFocus();
        }
        else
        {
            lineEdit->insert(clickedBtn->text());//文本输入框插入字母或符号
            lineEdit->setFocus();//为编辑框设置焦点，以使输入时在编辑框有闪烁光标提示
        }
    }
    else  //中文输入模式 键入的字母放在第二部分输入显示区域的候选字母按钮上
    {
        if(clickedBtn->text()==tr("，")||clickedBtn->text()==tr("。"))//，。特殊情况
        {
            lineEdit->insert(clickedBtn->text());//文本输入框插入字母或符号
            lineEdit->setFocus();//为编辑框设置焦点，以使输入时在编辑框有闪烁光标提示
        }
        else
        {
            inputDisplayArea->setVisible(true);//显示中文输入的候选区域
            globalVLayout->setStretch(1,0);//缩小主布局中空白的比例，此处置0，由显示的候选区域填补
            candidateLetter->insert(clickedBtn->text());//候选字母输入框插入字母
            candidateLetter->setFocus();
            this->matchChinese(candidateLetter->text());//匹配中文
            this->displayCandidateWord(pageCount);//显示候选词，默认第一页
            prePageBtn->setEnabled(false);//向前翻页不可点击
        }
    }
}
/*
 *@author:  缪庆瑞
 *@date:    2017.1.2
 *@brief:   中文输入 候选字母区域根据内容改变文本框的大小
 *@param:   text:即改变后的文本内容
 */
void SoftKeyboard::candidateLetterChangedSlot(QString text)
{
    //根据输入的内容自动改变文本区域大小
    int width = candidateLetter->fontMetrics().width(text)+10;
    candidateLetter->setFixedWidth(width);
}
/*
 *@author:  缪庆瑞
 *@date:    2016.12.25
 *@brief:   候选词被点击的响应槽
 */
void SoftKeyboard::candidateWordBtnSlot()
{
    QPushButton *clickedBtn = qobject_cast<QPushButton *>(sender());//获取信号发送者的对象
    lineEdit->insert(clickedBtn->text());
    lineEdit->setFocus();
    hideInputDiaplayArea();//隐藏中文候选区域
}
/*
 *@author:  缪庆瑞
 *@date:    2016.12.29
 *@brief:   候选词向前翻页
 */
void SoftKeyboard::candidateWordPrePageSlot()
{
    pageCount--;
    this->displayCandidateWord(pageCount);
    //避免点击向前翻页按钮时取消使能，焦点自动跑到下一个按钮上。
    candidateLetter->setFocus();
}
/*
 *@author:  缪庆瑞
 *@date:    2016.12.29
 *@brief:   候选词向后翻页
 */
void SoftKeyboard::candidateWordNextPageSlot()
{
    pageCount++;
    this->displayCandidateWord(pageCount);
    //避免点击向后翻页按钮时取消使能，焦点自动跑到下一个按钮上
    candidateLetter->setFocus();
}
/*
 *@author:  缪庆瑞
 *@date:    2017.1.4
 *@brief:   隐藏中文输入的候选区域
 */
void SoftKeyboard::hideInputDiaplayArea()
{
    candidateLetter->clear();//清空候选字母
    inputDisplayArea->setVisible(false);//隐藏中文输入显示区域
    //改变伸缩比例，主要是为了在中文候选区域隐藏或显示时，其他部件的位置基本不变，
    //因为布局中的部件隐藏时，整个布局的部件会重设最佳大小
    globalVLayout->setStretch(1,1);//将主布局空白部分比例设为初始化的值
}
/*
 *@author:  缪庆瑞
 *@date:    2016.12.26
 *@brief:   大小写切换 也可以切换数字字母与字符界面
 */
void SoftKeyboard::changeUpperLowerSlot()
{
    if(isLetterInput)//当前为字母界面
    {
        if(isLetterLower)//当前为小写状态
        {
            this->setLetterUpper();//设置为大写状态
        }
        else
        {
            this->setLetterLow();//设置为小写状态
        }
        isLetterLower = !isLetterLower;//将状态取反
    }
    else  //当前为数字字符界面,则返回之前的大小写字母界面
    {
        isLetterInput = true;
        if(isLetterLower)
        {
            this->setLetterLow();
        }
        else
        {
            this->setLetterUpper();
        }
    }
    lineEdit->setFocus();//将焦点返回到行编辑栏，避免某些按钮有焦点时会自动设置样式
}
/*
 *@author:  缪庆瑞
 *@date:    2016.12.26
 *@brief:   删除输入内容
 */
void SoftKeyboard::deleteTextSlot()
{
    delTimer->start(150);//开启150ms的定时器 连续删除
    if(inputDisplayArea->isVisible())//中文输入特有区域显示
    {
        //删除时不可以转移焦点，这样会使del按钮的释放事件得不到响应，无法关闭定时器
        //candidateLetter->setFocus();
        candidateLetter->backspace();//删除选中文本或光标前的一个字符，默认光标在最后
        if(candidateLetter->text().isEmpty())//删完了
        {
            hideInputDiaplayArea();
            delTimer->stop();
        }
        else
        {
            matchChinese(candidateLetter->text());//重新匹配拼音
            displayCandidateWord(pageCount);//显示候选词
        }
    }
    else
    {
        //lineEdit->setFocus();
        lineEdit->backspace();
    }
}
/*
 *@author:  缪庆瑞
 *@date:    2017.1.2
 *@brief:   关闭连续删除的定时器,并设置焦点
 */
void SoftKeyboard::closeDelTimer()
{
    delTimer->stop();
    if(inputDisplayArea->isVisible())
    {
        candidateLetter->setFocus();//候选字母设置焦点
    }
    else
    {
        lineEdit->setFocus();//编辑框设置焦点
    }
}
/*
 *@author:  缪庆瑞
 *@date:    2016.12.29
 *@brief:   切换皮肤
 */
void SoftKeyboard::changeSkinSlot()
{
    if(skinNum == 2)
    {
        skinNum = 0;
    }
    else
    {
        skinNum++;
    }
    initKeyboardStyle(skinNum);
    lineEdit->setFocus();//将焦点返回到行编辑栏，避免某些按钮有焦点时会自动设置样式
}
/*
 *@author:  缪庆瑞
 *@date:    2016.12.26
 *@brief:   切换符号界面 中英文输入下的部分符号不同
 */
void SoftKeyboard::changeLetterSymbolSlot()
{
    if(isLetterInput)//当前为字母界面
    {
        isLetterInput = false;//非字母界面
        if(isENInput)
        {
            this->setSymbolsEN();
        }
        else
        {
            this->setSymbolsCH();
        }
    }
    else  //如果当前为符号界面 返回到数字字母界面
    {
        isLetterInput = true;//字母界面
        if(isLetterLower)//小写
        {
            this->setLetterLow();
        }
        else //大写
        {
            this->setLetterUpper();
        }
    }
}
/*
 *@author:  缪庆瑞
 *@date:    2016.12.26
 *@brief:   空格响应槽
 */
void SoftKeyboard::spaceSlot()
{
    if(inputDisplayArea->isVisible())//中文输入显示区域显示时，空格选择第一个待选词
    {
        lineEdit->insert(candidateWordBtn[0]->text());
        lineEdit->setFocus();
        hideInputDiaplayArea();
    }
    else
    {
        lineEdit->insert(" ");//插入一个空格
        lineEdit->setFocus();
    }
}
/*
 *@author:  缪庆瑞
 *@date:    2016.12.26
 *@brief:   中英文切换
 */
void SoftKeyboard::changeChEnSlot()
{
    if(isENInput)
    {
        isENInput = false;//切换为中文输入
        chOrEnBtn->setText(tr("   中   "));
        commaBtn->setText(tr("，"));
        periodBtn->setText(tr("。"));
        if(isLetterInput)//字母界面 切换到中文输入时，默认小写
        {
            isLetterLower = true;
            this->setLetterLow();
        }
        else
        {
            this->setSymbolsCH();
        }
    }
    else
    {
        isENInput = true;//切换为英文输入
        chOrEnBtn->setText(tr("英"));
        commaBtn->setText(",");
        periodBtn->setText(".");
        if(!isLetterInput)//字符界面
        {
            this->setSymbolsEN();
        }
    }
    lineEdit->setFocus();//将焦点返回到行编辑栏，避免某些按钮有焦点时会自动设置样式
}
/*
 *@author:  缪庆瑞
 *@date:    2016.12.26
 *@brief:   回车响应槽
 */
void SoftKeyboard::enterSlot()
{
    if(!candidateLetter->text().isEmpty())//候选字母非空，则将字母插入到编辑框里
    {
        lineEdit->insert(candidateLetter->text());
        lineEdit->setFocus();
        hideInputDiaplayArea();
    }
    else
    {
        if(!lineEdit->text().isEmpty())//输入框内容非空，将内容发送出去
        {
            okBtnSlot();//相当于点击确定
        }
    }
}
/*
 *@author:  缪庆瑞
 *@date:    2017.1.2
 *@brief:   软键盘退出，不发出输入的文本信号
 */
void SoftKeyboard::quitBtnSlot()
{
    //退出时清空所有缓存
    lineEdit->clear();
    hideInputDiaplayArea();
    this->hide();
}
/*
 *@author:  缪庆瑞
 *@date:    2017.1.2
 *@brief:   软键盘退出，同时发出当前输入文本的信号
 */
void SoftKeyboard::okBtnSlot()
{
    emit sendText(lineEdit->text());
    quitBtnSlot();
}
