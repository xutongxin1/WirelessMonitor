#include "charts.h"
#include "ui_charts.h"
#include <string.h>
#include <QDebug>
#include <QPair>


Q_GLOBAL_STATIC(DataReceiver, s_DataReceiver)
QList<Datanode> DataPairs;  //QList方便与图例顺序对应

//打开通道不能移动和放缩，默认和关闭可以
//DataPairs是负责后台更新维护显示数据的，因为图标显示需要double数组。
//Data_pools是中间数据池，用容器去维护。
//收到的数据存在这个容器(数据池)，然后调用addData。存进去之前先确保是否已经存在这个名称，不然会继续往相同名称里加
QHash<QString, Datanode> Data_pools;
bool hide_flag = 1;//1是关闭隐藏，0是开启隐藏
/*颜色笔可选颜色，默认为红
 */
enum Pen_color {
    black,
    white,
    darkGray,
    gray,
    lightGray,
    red,
    green,
    blue,
    cyan,
    magenta,
    yellow,
    darkRed,
    darkGreen,
    darkBlue,
    darkCyan,
    darkMagenta,
    darkYellow,
    transparent
};

//QThread * move_thread = new QThread();
//graph.setPen,setName。每个曲线都会独占一个graph

Charts::Charts(int DeviceNum, int winNum, QSettings *cfg, ToNewWidget *parentInfo, QWidget *parent) :
        RepeaterWidget(parent),
        uiChart(new Ui::Charts) {
    uiChart->setupUi(this);

    //需要接入统一的设置文件系统
    this->cfg_ = cfg;
    this->group_name_ = "Win" + QString::number(winNum);
    this->device_num_ = DeviceNum;
    this->parent_info_ = parentInfo;
    startedTime = QTime::currentTime();

    (*(parentInfo->devices_info))[DeviceNum].has_chart = true;
    (*(parentInfo->devices_info))[DeviceNum].charts_windows = this;

    timerChart = new QTimer(this);
    timerChart->setInterval(500);


//    timerChart->start();
    DataReceiver::getInstance()->start();
    connect(DataReceiver::getInstance(), &DataReceiver::oneDataReady, this, [&] { ShowLine(uiChart->widget); });

    //connect(timerChart, &QTimer::timeout, this,[&]{timer_count++;updateData("test",timer_count);});
    connect(timerChart, &QTimer::timeout, this, [&] {
        timer_count++;
        updateData("test", timer_count, 1.0);
    });
//    updateData2("test",5.0,1.0);

// Allow user to drag axis ranges with mouse, zoom with mouse wheel and select graphs by clicking:
    //chart配置               uiChart->widget->graph(i)->setName();
    uiChart->widget->xAxis->setLabel("Time/秒");
    uiChart->widget->yAxis->setLabel("ADC");
    uiChart->widget->xAxis->setRange(0, 100);
    uiChart->widget->yAxis->setRange(0, 100);
    uiChart->widget->legend->setVisible(true);

    uiChart->widget->xAxis2->setVisible(true);
    uiChart->widget->xAxis2->setTickLabels(false);
    uiChart->widget->yAxis2->setVisible(true);
    uiChart->widget->yAxis2->setTickLabels(false);
    connect(uiChart->widget->xAxis, SIGNAL(rangeChanged(QCPRange)), uiChart->widget->xAxis2, SLOT(setRange(QCPRange)));
    connect(uiChart->widget->yAxis, SIGNAL(rangeChanged(QCPRange)), uiChart->widget->yAxis2, SLOT(setRange(QCPRange)));

    //设置基本坐标轴（左侧Y轴和下方X轴）可拖动、可缩放、曲线可选、legend可选、设置伸缩比例，使所有图例可见
    uiChart->widget->setInteractions(
            QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes | QCP::iSelectLegend | QCP::iSelectPlottables);

    connect(uiChart->widget, SIGNAL(mouseMove(QMouseEvent * )), this, SLOT(myMoveEvent(QMouseEvent * )));

    //设置legend只能选择图例
    uiChart->widget->legend->setSelectableParts(QCPLegend::spItems);
    connect(uiChart->widget, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));

    ShowLine(uiChart->widget);

}

Charts::~Charts() {
    delete uiChart;
}

void Charts::ShowLine(QCustomPlot *customPlot) {

    //qDebug() << "showline" << endl;
    if (hide_flag)//1是关闭隐藏，0是开启隐藏
    {
        //查找那些变量需要显示并且显示出来
        for (int i = 0; i < (DataPairs.size()); i++) {
            //记录每个变量的画图次数
            if ((DataPairs.at(i).flag) == 1) {//显示
                //qDebug() << "showline:" << DataPairs.at(i).data_node_type <<endl;
                switch (DataPairs.at(i).data_node_type) {
                    case SYS_TIME: {
                        while (!(DataPairs.at(i).double_list->isEmpty()))    //如果需要更新去画
                        {
                            //可以后期设置
                            QPen pen;
                            pen.setWidth(2);//设置线宽,默认是2

                            pen.setColor(Qt::red);//设置线条红色
                            customPlot->graph(i)->setName(DataPairs.at(i).name);
                            customPlot->graph(i)->setPen(pen);

                            customPlot->graph(i)->addData(timer_count, (DataPairs.at(i).double_list->takeFirst()));

                            customPlot->graph(i)->setVisible(true);
                            customPlot->graph(i)->rescaleAxes(true); //自动调成范围，只能放大。想要缩小把true去掉
                            //qDebug() << "red1"<< endl;

                        }
                    }
                        break;
                    case USER_TIME: {
                        while (!(DataPairs.at(i).data_list->isEmpty()))    //如果需要更新去画
                        {
                            QPair<double, double> chartpair = DataPairs.at(i).data_list->takeFirst();
                            //可以后期设置
                            QPen pen;
                            pen.setWidth(2);//设置线宽,默认是2

                            pen.setColor(Qt::red);//设置线条红色
                            customPlot->graph(i)->setName(DataPairs.at(i).name);
                            customPlot->graph(i)->setPen(pen);
                            customPlot->graph(i)->addData(chartpair.first, chartpair.second);

                            customPlot->graph(i)->setVisible(true);
                            customPlot->graph(i)->rescaleAxes(true); //自动调成范围，只能放大。想要缩小把true去掉
                            //qDebug() << "red1"<< endl;

                        }
                    }
                        break;

                }


            }
            else if ((DataPairs.at(i).flag) == 2) {
                switch (DataPairs.at(i).data_node_type) {
                    case SYS_TIME: {
                        while (!(DataPairs.at(i).double_list->isEmpty()))    //如果需要更新去画
                        {

                            customPlot->graph(i)->addData(timer_count, (DataPairs.at(i).double_list->takeFirst()));

                            customPlot->graph(i)->setVisible(false);

                            //qDebug() << "red2" << endl;

                        }
                    }
                        break;
                    case USER_TIME: {
                        while (!(DataPairs.at(i).data_list->isEmpty()))    //如果需要更新去画
                        {
                            QPair<double, double> chartpair = DataPairs.at(i).data_list->takeFirst();
                            customPlot->graph(i)->addData(chartpair.first, chartpair.second);

                            customPlot->graph(i)->setVisible(true);
                            //qDebug() << "red1"<< endl;

                        }
                    }
                        break;

                }


            }
        }
    }
    else    //全部屏蔽
    {
        //查找那些变量需要显示并且显示出来
        for (int i = 0; i < (DataPairs.size()); i++) {
            //记录每个变量的画图次数
            {

                switch (DataPairs.at(i).data_node_type) {
                    case SYS_TIME: {
                        while (!(DataPairs.at(i).double_list->isEmpty()))    //如果需要更新去画
                        {

                            customPlot->graph(i)->addData(timer_count, (DataPairs.at(i).double_list->takeFirst()));

                            customPlot->graph(i)->setVisible(false);

                            //qDebug() << "red2" << endl;

                        }
                    }
                        break;
                    case USER_TIME: {
                        while (!(DataPairs.at(i).data_list->isEmpty()))    //如果需要更新去画
                        {
                            QPair<double, double> chartpair = DataPairs.at(i).data_list->takeFirst();
                            customPlot->graph(i)->addData(chartpair.first, chartpair.second);

                            customPlot->graph(i)->setVisible(true);
                            //qDebug() << "red1"<< endl;

                        }
                    }
                        break;

                }


            }

        }
    }
    customPlot->replot();//重绘图形

}

void Charts::ReadyShowLine() {
    //timer_count += 0.2;

    //实时增加数据

    qDebug() << "111！" << endl;
    //move_thread->start();
    ShowLine(uiChart->widget);

}


void Charts::myMoveEvent(QMouseEvent *event) {
    //获取鼠标坐标，相对父窗体坐标
    int c_flag = 0;
    int x_pos = event->pos().x();
    int y_pos = event->pos().y();
//    qDebug() << "event->pos()" << event->pos();

    //鼠标坐标转化为CustomPlot内部坐标
    float x_val = uiChart->widget->xAxis->pixelToCoord(x_pos);
    float y_val = uiChart->widget->yAxis->pixelToCoord(y_pos);
    float line_y_val = 0;
    //获得x轴坐标位置对应的曲线上y的值
    for (int i = 0; i < uiChart->widget->graphCount(); ++i) {
        QCPGraph *graph = uiChart->widget->graph(i);
        if (graph->selected()) {
            line_y_val = uiChart->widget->graph(i)->data()->at(x_val)->value;
            c_flag = 1;
        }
    }

    //曲线的上点坐标位置，用来显示QToolTip提示框
    float out_x = uiChart->widget->xAxis->coordToPixel(x_val);
    float out_y = uiChart->widget->yAxis->coordToPixel(y_val);
    float out_value = uiChart->widget->yAxis->coordToPixel(line_y_val);

    QString str, strToolTip;
    str = QString::number(x_val, 10, 3);
    strToolTip += "Time: ";
    strToolTip += str;
    strToolTip += "\n";

    if (c_flag) {
        str = QString::number(line_y_val, 10, 3);
        strToolTip += "ADC: ";
        strToolTip += str;
        strToolTip += "\n";
        QToolTip::showText(mapToGlobal(QPoint(out_x, out_value)), strToolTip, uiChart->widget);

    }
    else {
        str = QString::number(y_val, 10, 3);
        strToolTip += "ADC: ";
        strToolTip += str;
        strToolTip += "\n";
        QToolTip::showText(mapToGlobal(QPoint(out_x, out_y)), strToolTip, uiChart->widget);
    }


}


void Charts::on_pushButton_clicked() {
    if (checked == 0) {
        //开启，不可以放缩和移动
        //qDebug() << "开始画图" << endl;
        uiChart->pushButton->setText("暂停显示");
        //timerChart->start();//每200ms重绘一次折线图
        //uiChart->widget->setInteractions(QCP::iNone);
        uiChart->widget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
        hide_flag = 1;
        checked = 1;
    }
    else {
        //关闭，可以放缩和移动
        uiChart->pushButton->setText("开始显示");
        //timerChart->stop();
        uiChart->widget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
        hide_flag = 0;
        checked = 0;
    }
}


void Charts::on_pushButton_add_clicked() {

    //获取下拉栏目前的内容，搜索有没有对应的变量名
    QString Data_Search = uiChart->comboBox->currentText();
    for (int i = 0; i < (DataPairs.size()); i++) {
        if ((DataPairs.at(i).name) == Data_Search)//存在返回true
        {

            DataPairs[i].flag = 1;
            //uiChart->widget->graph(i)->setVisible(true);//显示

        }

    }
    uiChart->widget->replot();//重绘图形
}

/*****
 * registerData是给外界的接口作用是增加可以绘图的变量，因此不用ui界面互动。
 * 如果识别到就先提前加图层准备画图
 * type两种类型，sys_time系统定时, user_time用户提供时间
 * 成功返回1，失败0
*****/
bool Charts::registerData(const QString &addName, DataType datatype) {
    qDebug("注册变量 %s", qPrintable(addName));
    if (startedTime.isNull()) {
        startedTime = QTime::currentTime();
    }
    if ((DataPairs.size()) == 0)//如果是空链表
    {
        Datanode *temp = new Datanode;
        temp->name = addName;

        temp->flag = 1;
        switch (datatype) {
            case SYS_TIME: {
                temp->data_node_type = SYS_TIME;
                temp->double_list = new QList<double>;
            }
            break;
            case USER_TIME: {
                temp->data_node_type = USER_TIME;
                temp->data_list = new QList<QPair<double, double>>;
            }
            break;
            default: {
                qDebug() << "addtype fail!" << endl;
                return 0;
                break;
            }

        }

        DataPairs.append(*temp);//插入数据          //DataPairs是负责后台更新维护显示数据的，因为图标显示需要double数组。
        //Data_pools是中间数据池，用容器去维护。

        Data_pools.insert(addName, *temp);

        uiChart->comboBox->addItem(addName);//combox插入项
        uiChart->widget->addGraph();//加图层准备画图
        qDebug() << "registerData:emptyadd" << endl;
        return 1;
    }
    else {   //有数据
        for (int i = 0; i < (DataPairs.size()); i++) {
            if ((DataPairs.at(i).name) == addName)//存在返回true
            {
                //加入错误,返回0
                qDebug() << "registerData：已存在" << endl;
                return 0;
            }
            else//如果识别到就先提前加图层准备画图
            {
                Datanode *temp = new Datanode;
                temp->name = addName;

                temp->flag = 1;
                switch (datatype) {
                    case SYS_TIME: {
                        temp->data_node_type = SYS_TIME;
                        temp->double_list = new QList<double>;
                    }
                        break;
                    case USER_TIME: {
                        temp->data_node_type = USER_TIME;
                        temp->data_list = new QList<QPair<double, double>>;
                    }
                        break;
                    default: {
                        qDebug() << "addtype fail!" << endl;
                        return 0;
                    }
                        break;
                }

                DataPairs.append(*temp);//插入数据

                //！！！数据池插入！！！
                Data_pools.insert(addName, *temp);

                uiChart->comboBox->addItem(addName);//combox插入项
                uiChart->widget->addGraph();//创建新画布
                qDebug() << "registerData:register success!" << endl;
                return 1;
            }
        }
    }
    qDebug() << (DataPairs.size()) << endl;
    qDebug() << "registerData:fail!" << endl;
    return 0;
}

void Charts::on_pushButton_yincang_clicked() {
    //点击鼠标然后隐藏
    for (int i = 0; i < uiChart->widget->graphCount(); ++i) {
        QCPGraph *graph = uiChart->widget->graph(i);
        if (graph->selected()) {
            DataPairs[i].flag = 2;
            uiChart->widget->graph(i)->setVisible(false);//隐藏
        }
    }
    uiChart->widget->replot();//重绘图形
}

/*****
 * antiRegisterData是给外界的接口作用是删除可以绘图的变量，因此不用ui界面互动。
 * 如果识别到就先删除图层
 * 成功返回1，失败0
*****/
bool Charts::antiRegisterData(QString addName) {
    qDebug() << "反注册变量 " << addName;
    //删除数据池里的数据和pair里的数据
    QHash<QString, DataNode>::iterator i;

    i = Data_pools.find(addName);
    if (i != Data_pools.end())   //如果找到了
    {
        Data_pools.erase(i);//删除数据池项目

        //查找Datapairs并删除
        for (int i = 0; i < (DataPairs.size()); i++) {
            if (DataPairs.at(i).name == addName) {
                uiChart->comboBox->removeItem(i);//combox删除项
                uiChart->widget->removeGraph(i);//减少图层

                DataPairs[i].~DataNode();
                DataPairs.removeAt(i);//减少list
                //delete Data_pools[addName].DataBuff;
                Data_pools.remove(addName);
                delete Data_pools[addName].double_list;

                qDebug() << "antiRegisterData: ok！" << endl;
                return 1;
            }
        }


    }
    else//没找到
    {
        qDebug() << "antiRegisterData: fail！" << endl;
        return 0;
    }
    qDebug() << "antiRegisterData: fail！" << endl;
    return 0;
}

/*****
 * updateData
 * 中介是Data_pools
 * 成功返回1，失败0
*****/
bool Charts::updateData(const QString &addName, double data) {
//    if (Data_pools.contains(addName)) {
//
//        Data_pools[addName].double_list->Append(ChangeData);
//
//        return true;
//    }
//    else {
//        qDebug() << "updateData: find fail！" << endl;
//        return false;
//    }
//    qDebug() << "updateData: fail！" << endl;
//    return 0;
    return updateData(addName, QTime::currentTime(), data);
}

bool Charts::updateData(const QString &addName, QTime ChangeTime, double data) {
    qDebug("启动时间%s,更新时间%s", qPrintable(startedTime.toString("h:m:s")),qPrintable(ChangeTime.toString("h:m:s")));
    qDebug("时间差%.02f", (double) startedTime.msecsTo(ChangeTime) / 1000.0);
    return updateData(addName, (double) startedTime.msecsTo(ChangeTime) / 1000, data);
}

bool Charts::updateData(const QString &addName, double ChangeTime, double data) {
    if (Data_pools.contains(addName)) {
        QPair<double, double> *temppair = new QPair<double, double>;
        temppair->first = ChangeTime;
        temppair->second = data;
        Data_pools[addName].data_list->append(*temppair);
        qDebug("添加数据%s,时间%.2f", qPrintable(addName), ChangeTime);
        return true;
    }
    else {
        qDebug() << "updateData: find fail！" << endl;
        return false;
    }
}

/*****
 * checkRegister
 * 检测是否注册过的接口
 * 成功找到返回1，失败0
*****/
[[maybe_unused]] bool Charts::checkRegister(QString addname) {
    if (Data_pools.contains(addname)) {
        qDebug() << "check: find！" << endl;
        return true;
    } else {
        qDebug() << "check: find fail！" << endl;
        return false;
    }
}


void Charts::test(const QVector<double> &addDate) {
    double *temp;
    int size = addDate.size();
    temp = new double[size];

    for (int i = 0; i < addDate.size(); i++) {
        temp[i] = addDate.at(i);
    }
    qDebug() << temp[3] << endl;
}


void Charts::selectionChanged() {
    // 将图形的选择与相应图例项的选择同步
    for (int i = 0; i < uiChart->widget->graphCount(); ++i) {
        QCPGraph *graph = uiChart->widget->graph(i);
        QCPPlottableLegendItem *item = uiChart->widget->legend->itemWithPlottable(graph);
        if (item->selected() || graph->selected()) {
            item->setSelected(true);
            //注意：这句需要Qcustomplot2.0系列版本
            graph->setSelection(QCPDataSelection(graph->data()->dataRange()));
            //这句1.0系列版本即可
            //graph->setSelected(true);
        }
    }
}


/****************************************************/
DataReceiver *DataReceiver::getInstance() {
    return s_DataReceiver;
}

DataReceiver::DataReceiver(QObject *parent) : QThread(parent) {}


void DataReceiver::stop() {
    this->requestInterruption();
}

void DataReceiver::run() {
    while (!isInterruptionRequested()) {

        //qDebug() << "xianchengrun"<< endl;
        mutex.lock();
        emit oneDataReady();
        mutex.unlock();


        //短暂睡眠让出线程
        msleep(100);//不加这句CPU占用率高达50%
    }
}

