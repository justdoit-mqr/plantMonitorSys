/*
 *@file:   customplotwidget.cpp
 *@author: 缪庆瑞
 *@date:   2017.4.10
 *@brief:  绘制图表的类，该类主要调用QCustomPlot曲线组件类的方法绘图
 */
#include "customplotwidget.h"
#include <QGridLayout>
#define RTDATACOUNT 26  //qcustomPlot显示实时数据时关联的数据最大组数

CustomPlotWidget::CustomPlotWidget(QWidget *parent) : QWidget(parent)
{
    initCustomPlot();
    initOtherWidget();
    QGridLayout *gridLayout = new QGridLayout(this);
    gridLayout->setContentsMargins(1,1,1,1);
    gridLayout->setSpacing(0);
    gridLayout->addWidget(voltageLabel,0,0,1,2,Qt::AlignLeft);
    gridLayout->addWidget(realtimeBtn,0,2,1,1,Qt::AlignCenter);
    gridLayout->addWidget(historyBtn,0,3,1,1,Qt::AlignCenter);
    gridLayout->addWidget(currentLabel,0,4,1,2,Qt::AlignRight);
    gridLayout->addWidget(voltageSlider,2,0,4,1);
    gridLayout->addWidget(customPlot,1,1,10,4);
    gridLayout->addWidget(currentSlider,2,5,4,1);
    gridLayout->addWidget(historyQueryWidget,11,0,1,6);

//    gridLayout->setRowStretch(0,1);
//    gridLayout->setRowStretch(1,10);

    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(refreshTime()));
    timer->start(1000);
}

CustomPlotWidget::~CustomPlotWidget()
{

}
/*
 *@brief:   初始化曲线组件类对象，设置曲线图的各参数比如刻度范围等
 *@author:  缪庆瑞
 *@date:    2017.4.11
 */
void CustomPlotWidget::initCustomPlot()
{
    customPlot = new QCustomPlot(this);
    // x(下方),y(左侧)轴附加文本显示的
    //customPlot->xAxis->setLabel(tr("时间"));//太占空间
    //customPlot->yAxis->setLabel(tr("电压(V)"));
    //设置y(右侧)轴显示，默认不显示
    customPlot->yAxis2->setVisible(true);
    //customPlot->yAxis2->setLabel(tr("电流(A)"));
    //x(下方)轴刻度以时间格式显示
    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");//时分秒显示
    customPlot->xAxis->setTicker(timeTicker);
    //customPlot->xAxis->setRange(0,20,Qt::AlignLeft);//第一个参数时起始位置，第二个时范围的size
    customPlot->yAxis->setRange(0.0, 60.0);//电压范围
    customPlot->yAxis->setTickLabelColor(QColor(Qt::blue));//蓝色标记
    customPlot->yAxis->setTickLabelFont(QFont("msyh",14));
    customPlot->yAxis2->setRange(0,500);//电流范围
    customPlot->yAxis2->setTickLabelColor(QColor(Qt::darkGreen));
    customPlot->yAxis2->setTickLabelFont(QFont("msyh",14));
    //customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    //添加4条曲线，前两条为实时数据曲线，后两条为历史数据曲线
    customPlot->addGraph();//实时 电压/时间
    customPlot->addGraph(0,customPlot->yAxis2);//实时 电流/时间 指定value坐标轴为右侧y轴
    customPlot->addGraph();//历史 电压/时间
    customPlot->addGraph(0,customPlot->yAxis2);//历史 电流/时间 指定value坐标轴为右侧y轴
    //设置4条曲线的样式
    customPlot->graph(0)->setPen(QPen(QBrush(Qt::blue),2));//画笔颜色,2个像素宽
    //customPlot->graph(0)->setBrush(QBrush(Qt::red));
    customPlot->graph(1)->setPen(QPen(QBrush(Qt::darkGreen),2));
    customPlot->graph(2)->setPen(QPen(QBrush(Qt::blue),2));//画笔颜色,2个像素宽
    customPlot->graph(3)->setPen(QPen(QBrush(Qt::darkGreen),2));
}
/*
 *@brief:   初始化曲线绘制界面的其他控制及显示类部件
 *@author:  缪庆瑞
 *@date:    2017.4.12
 */
void CustomPlotWidget::initOtherWidget()
{
    voltageLabel = new QLabel(tr("电压(V)"),this);
    voltageLabel->setStyleSheet("color: blue;font: 18px \"msyh\";");
    currentLabel = new QLabel(tr("电流(A)"),this);
    currentLabel->setStyleSheet("color: #008000;font: 18px \"msyh\";");
    //切换实时和历史数据的部件 单选按钮
    buttonGroup = new QButtonGroup(this);
    realtimeBtn = new QRadioButton(tr("实时"),this);
    realtimeBtn->setFont(QFont("msyh",18));
    realtimeBtn->setChecked(true);//默认为实时
    realtimeFlag = true;//表示当前曲线展示实时数据
    historyBtn = new QRadioButton(tr("历史"),this);
    historyBtn->setFont(QFont("msyh",18));
    buttonGroup->addButton(realtimeBtn,1);
    buttonGroup->addButton(historyBtn,0);
    connect(buttonGroup,SIGNAL(buttonClicked(int)),this,SLOT(realtimeOrHistoryChanged(int)));
    //调节电压电流显示范围的滑动条部件
    voltageSlider = new QSlider(Qt::Vertical,this);
    voltageSlider->setMinimum(10);//图表上电压的最小范围0-10v
    voltageSlider->setMaximum(100);//图表上电压的最大范围0-100v
    voltageSlider->setValue(60);//图表上电压的当前范围0-60v
    connect(voltageSlider,SIGNAL(valueChanged(int)),this,SLOT(voltageSliderValueChanged(int)));
    //设置滑动控制条的样式
    voltageSlider->setStyleSheet("QSlider{min-width:25px;}"//整个滑动部件
                                 "QSlider::groove:vertical{" //滑动的条 -滑线
                                 "background:  red;"//不加这个背景，导致下面的滑块margin不起作用
                                 "width:8px;}"
                                 "QSlider::add-page:vertical {"//滑块下方滑条区域
                                 "background: qlineargradient(spread:pad,x1:0, y1:0, x2:0, y2:1, stop:0 #00ffff , stop:1 #ff00ff);"//渐变色设置
                                 "border: 1px solid white;}"
                                 "QSlider::sub-page:vertical {"//滑块上方滑条区域
                                 "background: lightGray;"
                                 "border: 1px solid white;}"
                                 "QSlider::handle:vertical {" //滑动部件-滑块 柄
                                 "height: 12px;"
                                 "border: 0px solid white;"
                                 "border-radius:3px;"
                                 "background: blue;"
                                 "margin: 0px -8px; }"/* expand outside the groove ，使滑块的宽度大于滑线*/
                                 );
    currentSlider = new QSlider(Qt::Vertical,this);
    currentSlider->setMinimum(60);//图表上电流的最小范围0-60A
    currentSlider->setMaximum(800);//图表上电流的最大范围0-800A
    currentSlider->setValue(500);//图表上电流的当前范围0-400v
    connect(currentSlider,SIGNAL(valueChanged(int)),this,SLOT(currentSliderValueChanged(int)));
    //设置滑动控制条的样式
    currentSlider->setStyleSheet("QSlider{min-width:25px;}"//整个滑动部件
                                 "QSlider::groove:vertical{" //滑动的条 -滑线
                                 "background:  red;"//不加这个背景，导致下面的滑块margin不起作用
                                 "width:8px;}"
                                 "QSlider::add-page:vertical {"//滑块下方滑条区域
                                 "background: qlineargradient(spread:pad,x1:0, y1:0, x2:0, y2:1, stop:0 #00ffff , stop:1 #ff00ff);"//渐变色设置
                                 "border: 1px solid white;}"
                                 "QSlider::sub-page:vertical {"//滑块上方滑条区域
                                 "background: lightGray;"
                                 "border: 1px solid white;}"
                                 "QSlider::handle:vertical {" //滑动部件-滑块 柄
                                 "height: 12px;"
                                 "border: 0px solid white;"
                                 "border-radius:3px;"
                                 "background: green;"
                                 "margin: 0px -8px; }"/* expand outside the groove ，使滑块的宽度大于滑线*/
                                 );
    //初始化历史查询部件
    initHistoryQueryWidget();
}
/*
 *@brief:   初始化历史数据曲线查询的部件
 *@author:  缪庆瑞
 *@date:    2017.4.17
 */
void CustomPlotWidget::initHistoryQueryWidget()
{
    historyQueryWidget = new QWidget(this);
    historyQueryWidget->setMinimumSize(20,38);
    historyQueryWidget->setVisible(false);//初始化不显示
    QFont font("msyh",16);
    //设置下拉框滚动条的样式,主要是加宽滚动条以至于在触摸屏上便于滑动
    QString scrollBarStyleSheet(":vertical {"
                                "border: 4px groove grey;"
                                "background: #f3f3f3;"
                                "width: 25px;}");
    //startTimeLable = new QLabel(tr("起始时间:"),this);
    //startTimeLable->setFont(font);
    yearBox = new QComboBox(this);
    yearBox->setFont(font);
    yearBox->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    yearBox->addItem("2017");
    yearBox->view()->verticalScrollBar()->setStyleSheet(scrollBarStyleSheet);
    monthBox = new QComboBox(this);
    monthBox->setFont(font);
    monthBox->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    QStringList monthList;
    //设置下拉框滚动条的样式,主要是加宽滚动条以至于在触摸屏上便于滑动
    monthBox->view()->verticalScrollBar()->setStyleSheet(scrollBarStyleSheet);
    dayBox = new QComboBox(this);
    dayBox->setFont(font);
    dayBox->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    QStringList dayList;
    dayBox->view()->verticalScrollBar()->setStyleSheet(scrollBarStyleSheet);
    hourBox = new QComboBox(this);
    hourBox->setFont(font);
    hourBox->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    QStringList hourList;
    hourBox->view()->verticalScrollBar()->setStyleSheet(scrollBarStyleSheet);
    minuteBox = new QComboBox(this);
    minuteBox->setFont(font);
    minuteBox->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    QStringList minuteList;
    minuteBox->view()->verticalScrollBar()->setStyleSheet(scrollBarStyleSheet);
    timeIntervalLabel = new QLabel(tr("间隔(分):"),this);
    timeIntervalLabel->setFont(font);
    timeIntervalBox = new QComboBox(this);
    timeIntervalBox->setFont(font);
    timeIntervalBox->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    QStringList timeIntervalList ;
    timeIntervalBox->view()->verticalScrollBar()->setStyleSheet(scrollBarStyleSheet);
    for(int i=0;i<60;i++)//12个月
    {
        QString number = QString::number(i);//将数字转换成字符
        if(i==0)
        {
            hourList.append("0"+number);
            minuteList.append("0"+number);
        }
        else if(i<10)
        {
            monthList.append("0"+number);//个位数前补0，与数据库保持一致
            dayList.append("0"+number);
            hourList.append("0"+number);
            minuteList.append("0"+number);
            timeIntervalList .append("0"+number);
        }
        else if(i<=12)
        {
            monthList.append(number);
            dayList.append(number);
            hourList.append(number);
            minuteList.append(number);
            timeIntervalList .append(number);
        }
        else if(i<=20)
        {
            dayList.append(number);
            hourList.append(number);
            minuteList.append(number);
            timeIntervalList .append(number);
        }
        else if(i<24)
        {
            dayList.append(number);
            hourList.append(number);
            minuteList.append(number);
            //timeIntervalList .append(number);
        }
        else if(i<=31)
        {
            dayList.append(number);
            minuteList.append(number);
            //timeIntervalList .append(number);
        }
        else
        {
            minuteList.append(number);
            //timeIntervalList .append(number);
        }
    }
    monthBox->addItems(monthList);
    dayBox->addItems(dayList);
    hourBox->addItems(hourList);
    minuteBox->addItems(minuteList);
    timeIntervalBox->addItems(timeIntervalList);
    queryBtn = new QPushButton(tr("查询"),this);
    queryBtn->setFont(font);
    queryBtn->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    connect(queryBtn,SIGNAL(clicked()),this,SLOT(queryHistoryDataSlot()));
    QHBoxLayout *hBoxLayout = new QHBoxLayout(historyQueryWidget);
    hBoxLayout->setContentsMargins(1,1,1,1);
    hBoxLayout->setSpacing(1);
    //hBoxLayout->addWidget(startTimeLable,1);
    hBoxLayout->addWidget(yearBox,1);
    hBoxLayout->addWidget(monthBox,1);
    hBoxLayout->addWidget(dayBox,1);
    hBoxLayout->addWidget(hourBox,1);
    hBoxLayout->addWidget(minuteBox,1);
    hBoxLayout->addWidget(timeIntervalLabel,1);
    hBoxLayout->addWidget(timeIntervalBox,1);
    hBoxLayout->addWidget(queryBtn,2);

}
/*
 *@brief:   实时数据曲线展示时，定时刷新横坐标(时间)，这要求保证服务器与客户端时间同步
 *@author:  缪庆瑞
 *@date:    2017.4.13
 */
void CustomPlotWidget::refreshTime()
{
    //获取当前时间在一天中的秒数，
    //double secondsOfDay = QTime::currentTime().msecsSinceStartOfDay()/1000.0;
    //qt4.8.2 没有msecsSinceStartOfDay方法，使用下面的代替
    double secondsOfDay = qAbs(QTime::currentTime().secsTo(QTime(0,0,0)));
    //第一个参数是起始位置，第二个是范围的size
    customPlot->xAxis->setRange(secondsOfDay-RTDATACOUNT+2,RTDATACOUNT-1,Qt::AlignLeft);
    customPlot->replot();
}
/*
 *@brief:   调节电压范围的滑动条值改变
 *@author:  缪庆瑞
 *@date:    2017.4.17
 *@param:   value:滑动条当前值
 */
void CustomPlotWidget::voltageSliderValueChanged(int value)
{
    //qDebug()<<"sliderValue"<<value;
    customPlot->yAxis->setRange(0,value);
    customPlot->replot();
}
/*
 *@brief:   调节电流范围的滑动条值改变
 *@author:  缪庆瑞
 *@date:    2017.4.17
 *@param:   value:滑动条当前值
 */
void CustomPlotWidget::currentSliderValueChanged(int value)
{
    customPlot->yAxis2->setRange(0,value);
    customPlot->replot();
}
/*
 *@brief:   实时数据与历史数据切换
 *@author:  缪庆瑞
 *@date:    2017.4.17
 *@param:   id:点击的单选按钮id
 */
void CustomPlotWidget::realtimeOrHistoryChanged(int id)
{
    //根据点击的按钮id(0 =历史；1 = 实时)和realtimeFlag实时标记变量比较，实现只对状态改变做响应
    if(realtimeFlag == (bool)id)
    {
        qDebug()<<"no change:"<<id;
        return;
    }
    else//实时/历史 切换
    {
        realtimeFlag = (bool)id;
        qDebug()<<"button id="<<id;
        if(id)//实时
        {
            graphing(equipmentNo);
            customPlot->xAxis->setTickLabels(true);//显示刻度值
            historyQueryWidget->setVisible(false);
            timer->start();//开启定时器动态刷新时间
        }
        else//历史
        {
            customPlot->graph(0)->setVisible(false);//隐藏实时曲线
            customPlot->graph(1)->setVisible(false);
            customPlot->xAxis->setTickLabels(false);//不显示刻度值
            //历史查询默认设置为当前日期与时间
            monthBox->setCurrentIndex(QDate::currentDate().month()-1);//设置为当前日期
            dayBox->setCurrentIndex(QDate::currentDate().day()-1);
            hourBox->setCurrentIndex(QTime::currentTime().hour());
            minuteBox->setCurrentIndex(QTime::currentTime().minute());
            historyQueryWidget->setVisible(true);
            timer->stop();
        }
    }
}
/*
 *@brief:   查询指定的历史数据
 *@author:  缪庆瑞
 *@date:    2017.4.20
 */
void CustomPlotWidget::queryHistoryDataSlot()
{
    //获取要查询的日期
    QString tableName("equipment_"+equipmentNo);
    QString date = yearBox->currentText()+"-"+monthBox->currentText()+"-"+dayBox->currentText();
    QString timeStart = hourBox->currentText()+":"+minuteBox->currentText()+":00";//要查询的起始日期
    int timeStartSecond = qAbs(QTime::fromString(timeStart,"hh:mm:ss").secsTo(QTime(0,0,0)));
    int timeSecondLength = timeIntervalBox->currentText().toInt()*60;
    QString timeEnd = QTime::fromString(timeStart,"hh:mm:ss").addSecs(timeSecondLength).toString("hh:mm:ss");
    //qDebug()<<timeStart<<timeEnd;
    QSqlDatabase db = QSqlDatabase::database("connection1");
    QSqlQuery query(db);
    //查询指定时间段的数据
    if(!query.exec(QString("select Etime,Evoltage,Ecurrent from '%1' where Edate = '%2' and Etime between '%3' and '%4'")
                  .arg(tableName).arg(date).arg(timeStart).arg(timeEnd)))
    {
        qDebug()<<"select date time failed..."<<query.lastError();
        MyMessageBox::myInformation(this,"Info",tr("该设备无任何数据记录"),20);
        //QMessageBox::information(this,"Info",tr("该设备无任何数据记录"));
        return;
    }
    QVector<double> time;
    QVector<double> voltage;
    QVector<double> current;
    while(query.next())
    {
        time.append((double)qAbs(QTime::fromString(query.value(0).toString(),"hh:mm:ss").secsTo(QTime(0,0,0))));
        voltage.append(query.value(1).toDouble());
        current.append(query.value(2).toDouble());
    }
    if(time.isEmpty())
    {
        MyMessageBox::myInformation(this,"Info",tr("该时间段无数据记录"),20);
        //QMessageBox::information(this,"Info",tr("该时间段无数据记录"));
    }
    else
    {
        customPlot->graph(2)->setVisible(true);
        customPlot->graph(3)->setVisible(true);
        customPlot->graph(2)->setData(time,voltage);
        customPlot->graph(3)->setData(time,current);
        customPlot->xAxis->setRange(timeStartSecond,timeSecondLength,Qt::AlignLeft);
        customPlot->xAxis->setTickLabels(true);
        customPlot->replot();
        //qDebug()<<timeStartSecond<<timeSecondLength<<"bbbbbbbbbbbbb";
    }
}
/*
 *@brief:   设置哈希表键值对,用来对应具体设备的一组电压数据，一组电流数据，一组时间数据
 *@author:  缪庆瑞
 *@date:    2017.4.11
 *@param:   equipmentNo：设备号(键)
 *@param:   graphDataList：图表信息，电压/电流/时间串
 */
void CustomPlotWidget::setHashData(QString equipmentNo, QStringList graphDataList)
{
    if(graphInfoHash.contains(equipmentNo))//如果hash表中已经存了该键对应的键值对
    {
        //qDebug()<<"aaaaaaaa:1"<<QDateTime::currentDateTime().toString("hh:mm:ss:zzz");
        //hash表返回的值是const类型的，不可以直接更改，所以下面又新定义一个graphInfoList2
        QList<QVector <double> > graphInfoList = graphInfoHash.value(equipmentNo);
        QList<QVector <double> > graphInfoList2;
        QVector <double> voltage = graphInfoList.at(0);//电压数组
        QVector <double> current = graphInfoList.at(1);//电流数组
        QVector <double> time = graphInfoList.at(2);//时间数组
        //往数组中追加新的数据
        voltage.append(graphDataList.at(0).toDouble());//电压
        current.append(graphDataList.at(1).toDouble());//电流
        //time.append((double)QTime::fromString(graphDataList.at(2),"hh:mm:ss").msecsSinceStartOfDay()/1000.0);//时间
        //qt4.8.2 msecsSinceStartOfDay()方法，使用以下方法代替
        time.append((double)qAbs(QTime::fromString(graphDataList.at(2),"hh:mm:ss").secsTo(QTime(0,0,0))));
        //电压/电流/时间数组是一一对应的，保持数组的大小固定
        if(voltage.size()>RTDATACOUNT)
        {
            voltage.remove(0);//删除最前面的数据
            current.remove(0);//QVector当数据比较巨大时，帮助文档介绍insert操作会很费时，但没说remove，且这里数据不多，并不费时
            time.remove(0);
        }
        graphInfoList2.append(voltage);
        graphInfoList2.append(current);
        graphInfoList2.append(time);
        //将新的数据追加后，重新设置键值对
        //hash默认一键一值，如果重新给键设值，之前的会自动擦除
        graphInfoHash.insert(equipmentNo,graphInfoList2);
        //qDebug()<<"aaaaaaaa:2"<<QDateTime::currentDateTime().toString("hh:mm:ss:zzz");
    }
    else//如果hash表中不存在该键，表示设备第一次发送数据
    {
        QList<QVector <double> > graphInfoList;
        QVector <double> voltage;
        QVector <double> current;
        QVector <double> time;
        voltage.append(graphDataList.at(0).toDouble());
        current.append(graphDataList.at(1).toDouble());
        //time.append((double)QTime::fromString(graphDataList.at(2),"hh:mm:ss").msecsSinceStartOfDay()/1000.0);
        //qt4.8.2 msecsSinceStartOfDay()方法，使用以下方法代替
        time.append((double)qAbs(QTime::fromString(graphDataList.at(2),"hh:mm:ss").secsTo(QTime(0,0,0))));
        graphInfoList.append(voltage);
        graphInfoList.append(current);
        graphInfoList.append(time);
        graphInfoHash.insert(equipmentNo,graphInfoList);
    }
}
/*
 *@brief:   根据当前设备号绘制曲线图
 *@author:  缪庆瑞
 *@date:    2017.4.11
 *@param:   equipmentNo：设备号(键)
 */
void CustomPlotWidget::graphing(QString equipmentNo)
{
    //定义两个静态变量，只初始化一次，表示横纵坐标只存一个值，用来对曲线界面清空
    //static QVector<double> x(1,0),y(1,0);
    if(realtimeFlag)//实时展示
    {
        customPlot->graph(2)->setVisible(false);//隐藏历史数据曲线
        customPlot->graph(3)->setVisible(false);
        //如果没有对应设备的图表数据，隐藏界面曲线，然后返回
        if(!graphInfoHash.contains(equipmentNo))
        {
            customPlot->graph(0)->setVisible(false);//对应的曲线隐藏，但已绘制的界面不会清除
            customPlot->graph(1)->setVisible(false);//实际可以清除，replot()一下就好了
            customPlot->replot();
//            customPlot->graph(0)->setData(x,y);//实现界面的清空
//            customPlot->graph(1)->setData(x,y);
            return;
        }
        else
        {
            //qDebug()<<graphInfoHash.value(equipmentNo).at(0)<<graphInfoHash.value(equipmentNo).at(2);
            customPlot->graph(0)->setVisible(true);//对应的曲线显示
            customPlot->graph(1)->setVisible(true);
            //参数1表示横坐标时间数据，参数2表示纵坐标电压数据
            customPlot->graph(0)->setData(graphInfoHash.value(equipmentNo).at(2), graphInfoHash.value(equipmentNo).at(0));
            customPlot->graph(1)->setData(graphInfoHash.value(equipmentNo).at(2),graphInfoHash.value(equipmentNo).at(1));
            customPlot->replot();
        }
    }
    else//历史
    {
    }
}
/*
 *@brief:   设置当前的设备号
 *@author:  缪庆瑞
 *@date:    2017.4.
 *@param:   equipmentNo：设备号
 */
void CustomPlotWidget::setEquipmentNo(QString equipmentNo)
{
    this->equipmentNo = equipmentNo;
    customPlot->graph(2)->setVisible(false);//隐藏历史曲线
    customPlot->graph(3)->setVisible(false);
    customPlot->replot();
    graphing(equipmentNo);
    //qDebug()<<equipmentNo;
}

