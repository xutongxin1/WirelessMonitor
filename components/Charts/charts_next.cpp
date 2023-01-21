#include "charts_next.h"
#include "ui_charts_next.h"
#include <string.h>
#include <QDebug>
#include <QPair>

Q_GLOBAL_STATIC(DataReceiverNext, s_DataReceiverNext)
//QList方便与图例顺序对应

//打开通道不能移动和放缩，默认和关闭可以
//DataPairs是负责后台更新维护显示数据的，因为图表显示需要double数组。
//Data_pools是中间数据池，用容器去维护。
//收到的数据存在这个容器(数据池)，然后调用addData。存进去之前先确保是否已经存在这个名称，不然会继续往相同名称里加
//QHash<QString, Datanode> Data_pools;
bool hide_flag = 1;//1是关闭隐藏，0是开启隐藏
/*颜色笔可选颜色，默认为红
 */
//enum Pen_color {
//  black,
//  white,
//  darkGray,
//  gray,
//  lightGray,
//  red,
//  green,
//  blue,
//  cyan,
//  magenta,
//  yellow,
//  darkRed,
//  darkGreen,
//  darkBlue,
//  darkCyan,
//  darkMagenta,
//  darkYellow,
//  transparent
//};

//QThread * move_thread = new QThread();
//graph.setPen,setName。每个曲线都会独占一个graph

ChartsNext::ChartsNext(int device_num, int win_num, QSettings *cfg, ToNewWidget *parentInfo, QWidget *parent) :
    RepeaterWidget(parent),
    ui_chart_(new Ui::charts_next) {
    ui_chart_->setupUi(this);

    //需要接入统一的设置文件系统
    this->cfg_ = cfg;
    this->group_name_ = "Win" + QString::number(win_num);
    this->device_num_ = device_num;
    this->parent_info_ = parentInfo;
    startedTime = QTime::currentTime();

    (*(parentInfo->devices_info))[device_num].has_chart = true;
//    (*(parentInfo->devices_info))[DeviceNum].charts_windows = this;

    timerChart = new QTimer(this);
    timerChart->setInterval(500);


//    timerChart->start();
//    DataReceiverNext::getInstance()->start();
//    connect(DataReceiverNext::getInstance(),
//            &DataReceiverNext::oneDataReady,
//            this,
//            [&] { ShowLine(ui_chart_->widget); });

    //connect(timerChart, &QTimer::timeout, this,[&]{timer_count++;updateData("test",timer_count);});
    connect(timerChart, &QTimer::timeout, this, [&] {
      timer_count++;
      updateData("test", timer_count, 1.0);
    });
//    updateData2("test",5.0,1.0);

// Allow user to drag axis ranges with mouse, zoom with mouse wheel and select graphs by clicking:
    //chart配置               ui_chart_->widget->graph(i)->setName();
    ui_chart_->widget->xAxis->setLabel("Time/秒");
    ui_chart_->widget->yAxis->setLabel("ADC");
    ui_chart_->widget->xAxis->setRange(0, 100);
    ui_chart_->widget->yAxis->setRange(0, 100);
    ui_chart_->widget->legend->setVisible(true);

    ui_chart_->widget->xAxis2->setVisible(true);
    ui_chart_->widget->xAxis2->setTickLabels(false);
    ui_chart_->widget->yAxis2->setVisible(true);
    ui_chart_->widget->yAxis2->setTickLabels(false);
    connect(ui_chart_->widget->xAxis,
            SIGNAL(rangeChanged(QCPRange)),
            ui_chart_->widget->xAxis2,
            SLOT(setRange(QCPRange)));
    connect(ui_chart_->widget->yAxis,
            SIGNAL(rangeChanged(QCPRange)),
            ui_chart_->widget->yAxis2,
            SLOT(setRange(QCPRange)));

    //设置基本坐标轴（左侧Y轴和下方X轴）可拖动、可缩放、曲线可选、legend可选、设置伸缩比例，使所有图例可见
    ui_chart_->widget->setInteractions(
        QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes | QCP::iSelectLegend | QCP::iSelectPlottables);

    connect(ui_chart_->widget, SIGNAL(mouseMove(QMouseEvent * )), this, SLOT(myMoveEvent(QMouseEvent * )));

    //设置legend只能选择图例
    ui_chart_->widget->legend->setSelectableParts(QCPLegend::spItems);
    connect(ui_chart_->widget, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));
    custom_plot_ = ui_chart_->widget;
    UpdateLine();

}

ChartsNext::~ChartsNext() {
    delete ui_chart_;
}

void ChartsNext::UpdateLine() {
    for (int i = 0; i < data_pool_.size(); i++) {
        if (data_pool_.at(i).last_draw_index == 0)//首次绘制(或修改颜色后首次)
        {
            QPen pen;
            pen.setWidth(data_pool_.at(i).line_width);//设置线宽,默认是2
            pen.setColor(data_pool_.at(i).line_color);//设置线条红色
            custom_plot_->graph(i)->setPen(pen);
            custom_plot_->graph(i)->setName(data_pool_.at(i).data_name);
            if (data_pool_.at(i).is_visible) {
                custom_plot_->graph(i)->setVisible(true);
            } else {
                custom_plot_->graph(i)->setVisible(false);
            }
        }
        if (data_pool_.at(i).is_update) {
            this->data_pool_[i].is_update = false;

            for (int j = data_pool_.at(i).last_draw_index; j < data_pool_.at(i).data_list->size(); j++) {

                if (chart_time_type_ == DATA_TIME) {
                    custom_plot_->graph(i)->addData(data_pool_[i].data_list->at(j).time.data_time_.toTime_t(),
                                                    data_pool_[i].data_list->at(j).data
                    );
                } else {
                    custom_plot_->graph(i)->addData(
                        data_pool_[i].data_list->at(j).time.program_time_,
                        data_pool_[i].data_list->at(j).data);
                }
            }
            data_pool_[i].last_draw_index = data_pool_[i].data_list->size();
        }
    }
    custom_plot_->replot(QCustomPlot::rpQueuedReplot);
}


void ChartsNext::myMoveEvent(QMouseEvent *event) {
    //获取鼠标坐标，相对父窗体坐标
    int c_flag = 0;
    int x_pos = event->pos().x();
    int y_pos = event->pos().y();
//    qDebug() << "event->pos()" << event->pos();

    //鼠标坐标转化为CustomPlot内部坐标
    float x_val = ui_chart_->widget->xAxis->pixelToCoord(x_pos);
    float y_val = ui_chart_->widget->yAxis->pixelToCoord(y_pos);
    float line_y_val = 0;
    //获得x轴坐标位置对应的曲线上y的值
    for (int i = 0; i < ui_chart_->widget->graphCount(); ++i) {
        QCPGraph *graph = ui_chart_->widget->graph(i);
        if (graph->selected()) {
            line_y_val = ui_chart_->widget->graph(i)->data()->at(x_val)->value;
            c_flag = 1;
        }
    }

    //曲线的上点坐标位置，用来显示QToolTip提示框
    float out_x = ui_chart_->widget->xAxis->coordToPixel(x_val);
    float out_y = ui_chart_->widget->yAxis->coordToPixel(y_val);
    float out_value = ui_chart_->widget->yAxis->coordToPixel(line_y_val);

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
        QToolTip::showText(mapToGlobal(QPoint(out_x, out_value)), strToolTip, ui_chart_->widget);

    } else {
        str = QString::number(y_val, 10, 3);
        strToolTip += "ADC: ";
        strToolTip += str;
        strToolTip += "\n";
        QToolTip::showText(mapToGlobal(QPoint(out_x, out_y)), strToolTip, ui_chart_->widget);
    }

}

void ChartsNext::on_pushButton_clicked() {
    if (checked == 0) {
        //开启，不可以放缩和移动
        //qDebug() << "开始画图" << endl;
        ui_chart_->start_stop->setText("暂停显示");
        //timerChart->start();//每200ms重绘一次折线图
        //ui_chart_->widget->setInteractions(QCP::iNone);
        ui_chart_->widget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
        hide_flag = 1;
        checked = 1;
    } else {
        //关闭，可以放缩和移动
        ui_chart_->start_stop->setText("开始显示");
        //timerChart->stop();
        ui_chart_->widget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
        hide_flag = 0;
        checked = 0;
    }
}

//void ChartsNext::on_pushButton_add_clicked() {
//
//    //获取下拉栏目前的内容，搜索有没有对应的变量名
//    QString Data_Search = ui_chart_->comboBox->currentText();
//    for (int i = 0; i < (DataPairs.size()); i++) {
//        if ((DataPairs.at(i).name) == Data_Search)//存在返回true
//        {
//
//            DataPairs[i].flag = 1;
//            //ui_chart_->widget->graph(i)->setVisible(true);//显示
//
//        }
//
//    }
//    ui_chart_->widget->replot();//重绘图形
//}

/*****
 * registerData是给外界的接口作用是增加可以绘图的变量，因此不用ui界面互动。
 * 如果识别到就先提前加图层准备画图
 * type两种类型，sys_time系统定时, user_time用户提供时间
 * 成功返回1，失败0
*****/
bool ChartsNext::RegisterDataPoint(const QString &point_name) {
    qDebug("注册数据点 %s", qPrintable(point_name));

    struct DataNode node;
    node.data_name = point_name;
    QVector<singaldata> data_tmp;
    node
    data_pool_.append(node);

    qDebug() << "RegisterDataPoint:register success!" << endl;
    qDebug() << (data_pool_.size()) << endl;
    return true;

}

void ChartsNext::on_pushButton_yincang_clicked() {
//    //点击鼠标然后隐藏
//    for (int i = 0; i < ui_chart_->widget->graphCount(); ++i) {
//        QCPGraph *graph = ui_chart_->widget->graph(i);
//        if (graph->selected()) {
//            DataPairs[i].flag = 2;
//            ui_chart_->widget->graph(i)->setVisible(false);//隐藏
//        }
//    }
//    ui_chart_->widget->replot();//重绘图形
}

/*****
 * antiRegisterData是给外界的接口作用是删除可以绘图的变量，因此不用ui界面互动。
 * 如果识别到就先删除图层
 * 成功返回1，失败0
*****/
bool ChartsNext::AntiRegisterDataPoint(const QString &point_name) {
    qDebug() << "反注册变量 " << point_name;
    //删除数据池里的数据和pair里的数据
    QList<DataNode>::iterator i;
    for (i = data_pool_.begin(); i != data_pool_.end(); ++i) {
        if (i->data_name == point_name) {
            i = data_pool_.erase(i);
//            i--;//让迭代器去指向下一个元素，这样for循环才不会出错
            return true;
        }
    }

    qCritical() << "AntiRegisterDataPoint: fail！" << endl;
    return false;
}

/*****
 * updateData
 * 中介是Data_pools
 * 成功返回1，失败0
*****/
bool ChartsNext::updateData(const QString &addName, double data) {
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

bool ChartsNext::updateData(const QString &addName, QTime ChangeTime, double data) {
    qDebug("启动时间%s,更新时间%s", qPrintable(startedTime.toString("h:m:s")), qPrintable(ChangeTime.toString("h:m:s")));
    qDebug("时间差%.02f", (double) startedTime.msecsTo(ChangeTime) / 1000.0);
    return updateData(addName, (double) startedTime.msecsTo(ChangeTime) / 1000, data);
}

bool ChartsNext::updateData(const QString &addName, double ChangeTime, double data) {
    if (Data_pools.contains(addName)) {
        QPair<double, double> *temppair = new QPair<double, double>;
        temppair->first = ChangeTime;
        temppair->second = data;
        Data_pools[addName].data_list->append(*temppair);
        qDebug("添加数据%s,时间%.2f", qPrintable(addName), ChangeTime);
        return true;
    } else {
        qDebug() << "updateData: find fail！" << endl;
        return false;
    }
}

void ChartsNext::UpdateDataPoolIndex() {
    data_pool_index_.clear();//清除索引
    QList<DataNode>::iterator i;
    for (i = data_pool_.begin(); i != data_pool_.end(); ++i) {
        data_pool_index_.append(i->data_list);//添加到索引
    }
}
/*****
 * checkRegister
 * 检测是否注册过的接口
 * 成功找到返回1，失败0
*****/
[[maybe_unused]] bool ChartsNext::checkRegister(QString addname) {
    if (Data_pools.contains(addname)) {
        qDebug() << "check: find！" << endl;
        return true;
    } else {
        qDebug() << "check: find fail！" << endl;
        return false;
    }
}

void ChartsNext::test(const QVector<double> &addDate) {
    double *temp;
    int size = addDate.size();
    temp = new double[size];

    for (int i = 0; i < addDate.size(); i++) {
        temp[i] = addDate.at(i);
    }
    qDebug() << temp[3] << endl;
}

void ChartsNext::selectionChanged() {
    // 将图形的选择与相应图例项的选择同步
    for (int i = 0; i < ui_chart_->widget->graphCount(); ++i) {
        QCPGraph *graph = ui_chart_->widget->graph(i);
        QCPPlottableLegendItem *item = ui_chart_->widget->legend->itemWithPlottable(graph);
        if (item->selected() || graph->selected()) {
            item->setSelected(true);
            //注意：这句需要Qcustomplot2.0系列版本
            graph->setSelection(QCPDataSelection(graph->data()->dataRange()));
            //这句1.0系列版本即可
            //graph->setSelected(true);
        }
    }
}

///****************************************************/
//DataReceiverNext *DataReceiverNext::getInstance() {
//    return s_DataReceiverNext;
//}
//
//DataReceiverNext::DataReceiverNext(QObject *parent) : QThread(parent) {}
//
//void DataReceiverNext::stop() {
//    this->requestInterruption();
//}
//
//void DataReceiverNext::run() {
//    while (!isInterruptionRequested()) {
//
//        //qDebug() << "xianchengrun"<< endl;
//        mutex.lock();
//        emit oneDataReady();
//        mutex.unlock();
//
//
//        //短暂睡眠让出线程
//        msleep(100);//不加这句CPU占用率高达50%
//    }
//}
//
