/*
 *@file:   infocollectwidget.cpp
 *@author: 缪庆瑞
 *@date:   2017.4.23
 *@brief:  信息汇总界面，包括设备状态、运行时长、报警信息等统计信息
 */
#include "infocollectwidget.h"
#include <QGridLayout>
#include <QStyleFactory>

InfoCollectWidget::InfoCollectWidget(QWidget *parent) : QWidget(parent)
{
    initStatusBox();   
    initRuntimesBox();
    initWarningBox();
    initEquipmentNoBox();

    QGridLayout *gridLayout = new QGridLayout(this);
    gridLayout->setMargin(1);
    gridLayout->setSpacing(1);
    gridLayout->addWidget(equipmentStatusBox,0,0,1,1);
    gridLayout->addWidget(equipmentRunTimesBox,1,0,1,1);
    gridLayout->addWidget(equipmentWarningBox,0,1,2,3);
}

InfoCollectWidget::~InfoCollectWidget()
{

}
/*
 *@brief:   初始化设备状态汇总区域
 *@author:  缪庆瑞
 *@date:    2017.4.23
 */
void InfoCollectWidget::initStatusBox()
{
    equipmentStatusBox = new QGroupBox(tr("设备状态统计"),this);
    //equipmentStatusBox->setStyle(QStyleFactory::create("Windows"));//使用windows风格的样式，在arm上可以不用
    equipmentStatusBox->setFont(QFont("msyh",20));
    QString styleSheetString("font: 18pt \"msyh\";");
    allCountLabel = new QLabel(tr("共有设备:"));
    allCountLabel->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);//设置大小策略，供布局使用
    allCountLabel->setStyleSheet(styleSheetString);
    allCount = new QLabel();
    allCount->setStyleSheet("color:black;font: 20pt \"msyh\";");
    allCount->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
    offLineCountLabel = new QLabel(tr("离线设备:"));
    offLineCountLabel->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
    offLineCountLabel->setStyleSheet(styleSheetString);
    offLineCount = new QLabel();
    offLineCount->setStyleSheet("color:lightGray;font: 20pt \"msyh\";");
    offLineCount->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
    standByCountLabel = new QLabel(tr("待机设备:"));
    standByCountLabel->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
    standByCountLabel->setStyleSheet(styleSheetString);
    standByCount = new QLabel();
    standByCount->setStyleSheet("color:gray;font: 20pt \"msyh\";");
    standByCount->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
    runCountLabel = new QLabel(tr("运行设备:"));
    runCountLabel->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
    runCountLabel->setStyleSheet(styleSheetString);
    runCount = new QLabel();
    runCount->setStyleSheet("color:#00aa00;font: 20pt \"msyh\";");
    runCount->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
    warningCountLabel = new QLabel(tr("报警设备:"));
    warningCountLabel->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
    warningCountLabel->setStyleSheet(styleSheetString);
    warningCount = new QLabel();
    warningCount->setStyleSheet("color:red;font: 20pt \"msyh\";");
    warningCount->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);

    QGridLayout *gridLayout = new QGridLayout(equipmentStatusBox);
    gridLayout->setSpacing(5);
    gridLayout->addWidget(allCountLabel,0,0,1,1,Qt::AlignLeft);
    gridLayout->addWidget(allCount,0,1,1,1,Qt::AlignLeft);
    gridLayout->addWidget(offLineCountLabel,1,0,1,1,Qt::AlignLeft);
    gridLayout->addWidget(offLineCount,1,1,1,1,Qt::AlignLeft);
    gridLayout->addWidget(standByCountLabel,2,0,1,1,Qt::AlignLeft);
    gridLayout->addWidget(standByCount,2,1,1,1,Qt::AlignLeft);
    gridLayout->addWidget(runCountLabel,3,0,1,1,Qt::AlignLeft);
    gridLayout->addWidget(runCount,3,1,1,1,Qt::AlignLeft);
    gridLayout->addWidget(warningCountLabel,4,0,1,1,Qt::AlignLeft);
    gridLayout->addWidget(warningCount,4,1,1,1,Qt::AlignLeft);
}
/*
 *@brief:   初始化运行时长汇总区域
 *@author:  缪庆瑞
 *@date:    2017.4.23
 */
void InfoCollectWidget::initRuntimesBox()
{
    equipmentRunTimesBox = new QGroupBox(tr("运行时长统计"),this);
    //equipmentRunTimesBox->setStyle(QStyleFactory::create("Windows"));//使用windows风格的样式，在arm上可以不用
    equipmentRunTimesBox->setFont(QFont("msyh",20));
    QString styleSheetString("font: 18pt \"msyh\";");
    equipmentNoLabel = new QLabel(tr("设  备  号:"));
    equipmentNoLabel->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
    equipmentNoLabel->setStyleSheet(styleSheetString);
    equipmentNoBox = new QComboBox();
    equipmentNoBox->setFixedWidth(65);
    equipmentNoBox->setFont(QFont("msyh",16));
    equipmentNoBox->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    //设置下拉框滚动条的样式,主要是加宽滚动条以至于在触摸屏上便于滑动
    equipmentNoBox->view()->verticalScrollBar()->setStyleSheet(":vertical {"
                                                               "border: 4px groove grey;"
                                                               "background: #f3f3f3;"
                                                               "width: 26px;}"
                                                               );
    refreshBtn = new QPushButton();
    refreshBtn->setFocusPolicy(Qt::NoFocus);
    refreshBtn->setFixedSize(45,40);
    refreshBtn->setStyleSheet("border-image: url(:/images/refresh.ico)");
    connect(refreshBtn,SIGNAL(clicked()),this,SLOT(refreshRunTimesSlot()));
    standByTimesOfDayLabel = new  QLabel(tr("待机时长:"));
    standByTimesOfDayLabel->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
    standByTimesOfDayLabel->setStyleSheet(styleSheetString);
    standByTimesOfDay = new QLabel();
    standByTimesOfDay->setStyleSheet(styleSheetString);
    standByTimesOfDay->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
    runTimesOfDayLabel = new QLabel(tr("运行时长:"));
    runTimesOfDayLabel->setStyleSheet(styleSheetString);
    runTimesOfDayLabel->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
    runTimesOfDay = new QLabel();
    runTimesOfDay->setStyleSheet(styleSheetString);
    runTimesOfDay->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);

    QGridLayout *gridLayout = new QGridLayout(equipmentRunTimesBox);
    gridLayout->setContentsMargins(5,5,1,5);
    gridLayout->setSpacing(4);
    gridLayout->addWidget(equipmentNoLabel,0,0,1,1,Qt::AlignLeft);
    gridLayout->addWidget(equipmentNoBox,0,1,1,1,Qt::AlignLeft);
    gridLayout->addWidget(refreshBtn,0,2,1,1,Qt::AlignLeft);
    gridLayout->addWidget(standByTimesOfDayLabel,1,0,1,1,Qt::AlignLeft);
    gridLayout->addWidget(standByTimesOfDay,1,1,1,2,Qt::AlignLeft);
    gridLayout->addWidget(runTimesOfDayLabel,2,0,1,1,Qt::AlignLeft);
    gridLayout->addWidget(runTimesOfDay,2,1,1,2,Qt::AlignLeft);
}
/*
 *@brief:   初始化警报信息汇总区域
 *@author:  缪庆瑞
 *@date:    2017.4.23
 */
void InfoCollectWidget::initWarningBox()
{
    equipmentWarningBox = new QGroupBox(tr("报警信息统计"),this);
    equipmentWarningBox->setFont(QFont("msyh",20));
    //equipmentWarningBox->setStyle(QStyleFactory::create("Windows"));//使用windows风格的样式，在arm上可以不用
    //设置下拉框滚动条的样式,主要是加宽滚动条以至于在触摸屏上便于滑动
    QString scrollBarStyleSheet(":vertical {"
                                                   "border: 4px groove grey;"
                                                   "background: #f3f3f3;"
                                                   "width: 28px;}");
    //初始化查询相关部件
    historyQueryWidget = new QWidget(this);
    dateLable = new QLabel(tr("日期:"));
    dateLable->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
    yearBox = new QComboBox(this);
    yearBox->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    yearBox->addItem("2019");
    yearBox->view()->verticalScrollBar()->setStyleSheet(scrollBarStyleSheet);
    monthBox = new QComboBox(this);
    monthBox->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    monthBox->view()->verticalScrollBar()->setStyleSheet(scrollBarStyleSheet);
    QStringList monthList;
    dayBox = new QComboBox(this);
    dayBox->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    dayBox->view()->verticalScrollBar()->setStyleSheet(scrollBarStyleSheet);
    QStringList dayList;
    for(int i=1;i<=31;i++)
    {
        QString number = QString::number(i);//将数字转换成字符
        if(i<10)
        {
            monthList.append("0"+number);//个位数前补0，与数据库保持一致
            dayList.append("0"+number);
        }
        else if(i<13)
        {
            monthList.append(number);
            dayList.append(number);
        }
        else
        {
            dayList.append(number);
        }
    }
    monthBox->addItems(monthList);
    dayBox->addItems(dayList);
    monthBox->setCurrentIndex(QDate::currentDate().month()-1);//设置为当前日期
    dayBox->setCurrentIndex(QDate::currentDate().day()-1);
    equipmentNoLabel2 = new QLabel(tr(" 设备号:"));
    equipmentNoLabel2->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
    equipmentNoBox2 = new QComboBox();
    equipmentNoBox2->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    equipmentNoBox2->view()->verticalScrollBar()->setStyleSheet(scrollBarStyleSheet);
    queryBtn = new QPushButton(tr("查询"),this);
    queryBtn->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    connect(queryBtn,SIGNAL(clicked()),this,SLOT(queryWarningInfoSlot()));
    QHBoxLayout *hBoxLayout = new QHBoxLayout(historyQueryWidget);
    hBoxLayout->setContentsMargins(1,1,1,1);
    hBoxLayout->setSpacing(4);
    hBoxLayout->addWidget(dateLable,1);
    hBoxLayout->addWidget(yearBox,1);
    hBoxLayout->addWidget(monthBox,1);
    hBoxLayout->addWidget(dayBox,1);
    hBoxLayout->addWidget(equipmentNoLabel2,1);
    hBoxLayout->addWidget(equipmentNoBox2,1);
    hBoxLayout->addWidget(queryBtn,1);
    //初始化报警信息展示表格部件
    warningTableWidget = new BaseTableWidget();
    warningTableWidget->setColumnCount(4);
    QStringList columnNames;
    columnNames<<tr("设备号")<<tr("报警信息")<<tr("起始时间")<<tr("持续时间");
    warningTableWidget->setHorizontalHeaderLabels(columnNames);
    warningTableWidget->horizontalHeader()->setFixedHeight(35);//设置水平表头固定高度，避免字体过大显不全
    //warningTableWidget->horizontalHeader()->setDragDropMode(QAbstractItemView::NoDragDrop);//设置表头不可拖动  单用这一句没用
    //warningTableWidget->horizontalHeader()->setResizeMode(QHeaderView::Fixed);//设置水平表头不可调整大小，同样不可拖动
    warningTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);//Qt5 取代setResizeMode
    warningTableWidget->verticalHeader()->setVisible(false);//垂直表头不显示
    warningTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//设置表格不可编辑
    warningTableWidget->setSelectionMode(QAbstractItemView::NoSelection);//设置单元格不可选中
    warningTableWidget->setColumnWidth(0,85);//设置列宽
    warningTableWidget->setColumnWidth(1,220);
    warningTableWidget->setColumnWidth(2,120);
    warningTableWidget->setColumnWidth(3,120);
    //设置表格垂直滚动条的样式,主要是加宽滚动条以至于在触摸屏上便于滑动
    warningTableWidget->verticalScrollBar()->setStyleSheet(":vertical {"
                                                           "border: 4px groove grey;"
                                                           "background: #f3f3f3;"
                                                           "width: 28px;}");

    QGridLayout *gridLayout = new QGridLayout(equipmentWarningBox);
    gridLayout->setMargin(2);
    gridLayout->addWidget(historyQueryWidget,0,0);
    gridLayout->addWidget(warningTableWidget,1,0);
}
/*
 *@brief:   根据数据库初始化设备号列表
 *@author:  缪庆瑞
 *@date:    2017.4.24
 */
void InfoCollectWidget::initEquipmentNoBox()
{
    QStringList equipmentNoList;
    //通过连接名获取在车间总况界面连接的数据库,即共用一个数据库连接
    db = QSqlDatabase::database("connection1");
    QSqlQuery query(db);
    if(!query.exec("select No from equipmentInfo where Area = 'A'"))
    {
        qDebug()<<query.lastError().text();
        return;
    }
    while(query.next())
    {
        equipmentNoList.append(query.value(0).toString());
    }
    if(!query.exec("select No from equipmentInfo where Area = 'B'"))
    {
        qDebug()<<query.lastError().text();
        return;
    }
    while(query.next())
    {
        equipmentNoList.append(query.value(0).toString());
    }
    equipmentNoBox->addItems(equipmentNoList);//运行时长区域的设备列表
    equipmentNoBox2->addItem("All");//报警信息区域的设备列表
    equipmentNoBox2->addItems(equipmentNoList);
}
/*
 *@brief:   设置各状态设备的数量
 *@author:  缪庆瑞
 *@date:    2017.4.24
 *@param:   all:共有设备数
 *@param:   offline:离线设备数
 *@param:   standby:待机设备数
 *@param:   run:运行设备数
 *@param:   warning:报警设备数
 */
void InfoCollectWidget::setStatusCount(int all, int offline, int standby, int run, int warning)
{
    allCount->setText(QString::number(all)+tr("台"));
    offLineCount->setText(QString::number(offline)+tr("台"));
    standByCount->setText(QString::number(standby)+tr("台"));
    runCount->setText(QString::number(run)+tr("台"));
    warningCount->setText(QString::number(warning)+tr("台"));
}
/*
 *@brief:   更新设备号列表
 *@author:  缪庆瑞
 *@date:    2017.4.24
 *@param:   list:新的设备号列表
 */
void InfoCollectWidget::updateEqmNoBox(QStringList list)
{
    equipmentNoBox->clear();
    equipmentNoBox->addItems(list);
    equipmentNoBox2->clear();
    equipmentNoBox2->addItem("All");
    equipmentNoBox2->addItems(list);
}
/*
 *@brief:   刷新运行(待机)时长显示
 *@author:  缪庆瑞
 *@date:    2017.4.24
 */
void InfoCollectWidget::refreshRunTimesSlot()
{
    QString equipementNo = equipmentNoBox->currentText();
    QString tableName("equipment_"+equipementNo);
    QString date = QDate::currentDate().toString("yyyy-MM-dd");
    //通过连接名获取在车间总况界面连接的数据库,即共用一个数据库连接
    db = QSqlDatabase::database("connection1");
    QSqlQuery query1(db);//待机
    if(!query1.exec(QString("select count(Ecurrent) from '%1' where Edate='%2' and Ecurrent<=10")
                  .arg(tableName).arg(date)))
    {
        MyMessageBox::myInformation(this,"Information",tr("该设备无任何数据记录"),20);
        return;
    }
    //测试发现sqlite数据库不支持获取querysize的信息调用query.size()返回负1
    //qDebug()<<db.driver()->hasFeature(QSqlDriver::QuerySize);
    if(query1.next())
    {
        int standbySeconds = query1.value(0).toInt();
        QString second = QString::number(standbySeconds%60);
        QString minute = QString::number(standbySeconds/60%60);
        QString hour = QString::number(standbySeconds/3600%60);
        standByTimesOfDay->setText(hour+tr("时")+minute+tr("分")+second+tr("秒"));
    }
    QSqlQuery query2(db);//运行
    if(!query2.exec(QString("select count(Ecurrent) from '%1' where Edate='%2' and Ecurrent>10")
                  .arg(tableName).arg(date)))
    {
        //qDebug()<<"select Ecurrent failed..."<<query2.lastError();
        MyMessageBox::myInformation(this,"Information",tr("该设备无任何数据记录"),20);
        //QMessageBox::information(this,"Info",tr("该设备无任何数据记录"));
        return;
    }
    if(query2.next())
    {
        int standbySeconds = query2.value(0).toInt();
        QString second = QString::number(standbySeconds%60);
        QString minute = QString::number(standbySeconds/60%60);
        QString hour = QString::number(standbySeconds/3600%60);
        runTimesOfDay->setText(hour+tr("时")+minute+tr("分")+second+tr("秒"));
    }
}
/*
 *@brief:   查询指定的报警信息
 *@author:  缪庆瑞
 *@date:    2017.4.25
 */
void InfoCollectWidget::queryWarningInfoSlot()
{
    QStringList tableContentList;//按行顺序存放表格中对应内容
    //获取要查询的日期
    QString date = yearBox->currentText()+"-"+monthBox->currentText()+"-"+dayBox->currentText();
    //要查询的设备
    QString equipmentNo = equipmentNoBox2->currentText();
    int equipmentCount = equipmentNoBox2->count()-1;//所有的设备数量
    //通过连接名获取在车间总况界面连接的数据库,即共用一个数据库连接
    db = QSqlDatabase::database("connection1");
    if(equipmentNo == "All")//查询所有的设备报警信息
    {
        QSqlQuery query(db);
        for(int i=1;i<=equipmentCount;i++)
        {
            QString tableName("equipment_"+equipmentNoBox2->itemText(i));
            if(!query.exec(QString("select * from '%1' where Edate = '%2' and (Evoltage>40 or Ecurrent>400)")
                           .arg(tableName).arg(date)))
            {
                //qDebug()<<"no table "<<tableName<<query.lastError();
                continue;//如果没有对应设备的表格，则退出本次循环，开始下一次
            }
            if(query.next())
            {
                QTime preTime = query.value(1).toTime();//表示上一次的时间，
                QTime curTime;//本次时间，用来与上一次比较，判断时间是否连续
                int seconds = 0;//连续的时间长度
                tableContentList.append(equipmentNoBox2->itemText(i));//设备号
                if(query.value(2).toDouble()>40&&query.value(3).toInt()>400)
                {
                    tableContentList.append(tr("电压、电流参数超限"));//报警信息
                }
                else if(query.value(2).toDouble()>40)
                {
                    tableContentList.append(tr("电压参数超限"));//报警信息
                }
                else
                {
                    tableContentList.append(tr("电流参数超限"));//报警信息
                }
                tableContentList.append(query.value(1).toString());//起始时间
                tableContentList.append(QString::number(seconds));//持续时间
                while(query.next())
                {
                    curTime = query.value(1).toTime();
                    if(preTime.secsTo(curTime)>1)//时间不连续  新添加一行报警信息
                    {
                        preTime = curTime;
                        seconds = 0;
                        tableContentList.append(equipmentNoBox2->itemText(i));
                        if(query.value(2).toDouble()>40&&query.value(3).toInt()>400)
                        {
                            tableContentList.append(tr("电压、电流参数超限"));//报警信息
                        }
                        else if(query.value(2).toDouble()>40)
                        {
                            tableContentList.append(tr("电压参数超限"));//报警信息
                        }
                        else
                        {
                            tableContentList.append(tr("电流参数超限"));//报警信息
                        }
                        tableContentList.append(query.value(1).toString());
                        tableContentList.append(QString::number(seconds));
                    }
                    else //时间连续
                    {
                        preTime = curTime;
                        seconds++;
                        tableContentList.removeLast();//移除最后一个项(持续时间)
                        tableContentList.append(QString::number(seconds));//更新持续时间
                    }
                }
            }
        }
    }
    else // 查询中某一个设备的报警信息
    {
        QSqlQuery query(db);
        QString tableName("equipment_"+equipmentNoBox2->currentText());
        if(!query.exec(QString("select * from '%1' where Edate = '%2' and (Evoltage>40 or Ecurrent>400)")
                       .arg(tableName).arg(date)))
        {
            //qDebug()<<"no table "<<tableName<<query.lastError();
            warningTableWidget->setRowCount(0);
            MyMessageBox::myInformation(this,"Information",tr("该日期无任何报警记录"),20);
            //QMessageBox::information(this,"Info",tr("该日期无任何报警记录"));
            return;
        }
        if(query.next())
        {
            QTime preTime = query.value(1).toTime();//表示上一次的时间，
            QTime curTime;//本次时间，用来与上一次比较，判断时间是否连续
            int seconds = 0;//连续的时间长度
            tableContentList.append(equipmentNoBox2->currentText());//设备号
            if(query.value(2).toDouble()>40&&query.value(3).toInt()>400)
            {
                tableContentList.append(tr("电压、电流参数超限"));//报警信息
            }
            else if(query.value(2).toDouble()>40)
            {
                tableContentList.append(tr("电压参数超限"));//报警信息
            }
            else
            {
                tableContentList.append(tr("电流参数超限"));//报警信息
            }
            tableContentList.append(query.value(1).toString());//起始时间
            tableContentList.append(QString::number(seconds));//持续时间
            while(query.next())
            {
                curTime = query.value(1).toTime();
                if(preTime.secsTo(curTime)>1)//时间不连续
                {
                    preTime = curTime;
                    seconds = 0 ;
                    tableContentList.append(equipmentNoBox2->currentText());
                    if(query.value(2).toDouble()>40&&query.value(3).toInt()>400)
                    {
                        tableContentList.append(tr("电压、电流参数超限"));//报警信息
                    }
                    else if(query.value(2).toDouble()>40)
                    {
                        tableContentList.append(tr("电压参数超限"));//报警信息
                    }
                    else
                    {
                        tableContentList.append(tr("电流参数超限"));//报警信息
                    }
                    tableContentList.append(query.value(1).toString());
                    tableContentList.append(QString::number(seconds));
                }
                else
                {
                    preTime = curTime;
                    seconds++;
                    tableContentList.removeLast();
                    tableContentList.append(QString::number(seconds));
                }
            }
        }
    }
    //qDebug()<<tableContentList;
    //将报警信息添加到表格中
    int rowCount = tableContentList.size()/4;
    warningTableWidget->setRowCount(rowCount);
    for(int j=0;j<rowCount;j++)
    {
        //设备号 列
        QTableWidgetItem *eqmNoItem = new QTableWidgetItem(tableContentList.at(j*4));
        eqmNoItem->setTextAlignment(Qt::AlignCenter);//显示文本居中
        warningTableWidget->setItem(j,0,eqmNoItem);
        //报警信息 列
        QTableWidgetItem *warningInfoItem = new QTableWidgetItem(tableContentList.at(j*4+1));
        warningInfoItem->setForeground(QBrush(Qt::red));//文本前景色
        warningInfoItem->setTextAlignment(Qt::AlignCenter);//居中
        warningTableWidget->setItem(j,1,warningInfoItem);
        //起始时间 列
        QTableWidgetItem *startTimeItem = new QTableWidgetItem(tableContentList.at(j*4+2));
        startTimeItem->setTextAlignment(Qt::AlignCenter);//居中
        warningTableWidget->setItem(j,2,startTimeItem);
        int continueSeconds = tableContentList.at(j*4+3).toInt();//持续时间(秒)
        QString second = QString::number(continueSeconds%60);
        if(second.size()==1)
        {
            second.prepend("0");//前补0
        }
        QString minute = QString::number(continueSeconds/60%60);
        if(minute.size()==1)
        {
            minute.prepend("0");
        }
        QString hour = QString::number(continueSeconds/3600%60);
        if(hour.size()==1)
        {
            hour.prepend("0");
        }
        //持续时间 列
        QTableWidgetItem *continueTimeItem = new QTableWidgetItem(hour+":"+minute+":"+second);
        continueTimeItem->setTextAlignment(Qt::AlignCenter);//居中
        warningTableWidget->setItem(j,3,continueTimeItem);
    }
    if(!tableContentList.size())
    {
        MyMessageBox::myInformation(this,"Information",tr("该日期无任何报警记录"),20);
        //QMessageBox::information(this,"Info",tr("该日期无任何报警记录"));
    }
}

