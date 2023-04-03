#include "charts_next.h"
#include "ui_charts_next.h"

#include <QDebug>
#include <QPair>
#include <QColorDialog>


//QList方便与图例顺序对应

//打开通道不能移动和放缩，默认和关闭可以
//DataPairs是负责后台更新维护显示数据的，因为图表显示需要double数组。
//Data_pools是中间数据池，用容器去维护。
//收到的数据存在这个容器(数据池)，然后调用addData。存进去之前先确保是否已经存在这个名称，不然会继续往相同名称里加
//QHash<QString, Datanode> data_pool_index_;

//TODO:可选的点绘制，颜色，线的显示与颜色
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

ChartsNext::ChartsNext(int device_num, int win_num, QSettings *cfg, ToNewWidget *parent_info, QWidget *parent) :
    RepeaterWidget(parent),
    ui_chart_(new Ui::charts_next) {
    ui_chart_->setupUi(this);

    //需要接入统一的设置文件的系统
    this->cfg_ = cfg;
    this->group_name_ = "Win" + QString::number(win_num);
    this->device_num_ = device_num;
    this->parent_info_ = parent_info;

    (*(parent_info->devices_info))[device_num].has_chart = true;
    (*(parent_info->devices_info))[device_num].charts_windows = this;

    paint_timer_ = new QTimer(this);
    paint_timer_->setInterval(100);

    connect(paint_timer_, &QTimer::timeout, this, [&] {
      UpdateLine();
    });//绘图定时器

    // 设置表格列平分
    ui_chart_->line_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

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
        if (data_pool_.at(i).last_draw_index == -1)//首次绘制(或修改颜色后首次)
        {
            QPen pen;
            pen.setWidth(data_pool_.at(i).line_width);//设置线宽,默认是2
            pen.setColor(data_pool_.at(i).line_color);//设置线条红色
            custom_plot_->addGraph();
            custom_plot_->graph(i)->setPen(pen);
            custom_plot_->graph(i)->setName(data_pool_.at(i).data_name);
            if (data_pool_.at(i).is_visible) {
                custom_plot_->graph(i)->setVisible(true);
            } else {
                custom_plot_->graph(i)->setVisible(false);
            }
            data_pool_[i].last_draw_index = 0;

        }
        if (data_pool_.at(i).is_update) {
            this->data_pool_[i].is_update = false;

            for (int j = data_pool_.at(i).last_draw_index; j < data_pool_.at(i).data_list.size(); j++) {

                if (chart_time_type_ == DATA_TIME) {
                    custom_plot_->graph(i)->addData(data_pool_[i].data_list.at(j).time.date_time_->toTime_t(),
                                                    data_pool_[i].data_list.at(j).data
                    );
                } else {
                    custom_plot_->graph(i)->addData(
                        data_pool_[i].data_list.at(j).time.program_time_,
                        data_pool_[i].data_list.at(j).data);
                }
            }
            data_pool_[i].last_draw_index = data_pool_[i].data_list.size();
            custom_plot_->graph(0)->rescaleAxes();
        }
    }

    custom_plot_->replot(QCustomPlot::rpQueuedReplot);
}


void ChartsNext::myMoveEvent(QMouseEvent *event) {
    //获取鼠标坐标，相对父窗体坐标
    int c_flag = 0;         // 应该是一个是否选中线条的标志
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

    /// 显示提示框
    // c_flag
    if (0) {
        /// 这部分是选中逻辑
        str = QString::number(line_y_val, 10, 3);
        strToolTip += "ADC: ";
        strToolTip += str;
        strToolTip += "\n";
        /// 这里这么写是有什么用吗，选中之后提示有什么作用？
        QToolTip::showText(mapToGlobal(QPoint(out_x, out_value)), strToolTip, ui_chart_->widget);

    }
    else {
        /// 这部分是默认逻辑
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
 * 成功返回1，失败0
*****/
bool ChartsNext::RegisterDataPoint(const QString &point_name) {
    qDebug("注册数据点 %s", qPrintable(point_name));

    struct DataNode node;
    node.data_name = point_name;
//    QVector<singaldata> data_tmp;
//    node.data_list = &data_tmp;
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
    //释放DataTime里的对象（如果使用了DATA_TIME），然后释放data_list，最后释放data_pool_的对应节点
    QList<DataNode>::iterator i;
    for (i = data_pool_.begin(); i != data_pool_.end(); ++i) {
        if (i->data_name == point_name) {
            if (chart_time_type_ == DATA_TIME) {//释放DataTime里的对象（如果使用了DATA_TIME）
                for (QVector<singaldata>::iterator j = i->data_list.begin(); j != i->data_list.end(); j++) {
                    delete j->time.date_time_;
                }
            }
            //释放data_list
            i->data_list.clear();

            //释放data_pool_的对应节点
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
bool ChartsNext::AddDataAuto(const QString &point_name, double data) {
    if (chart_time_type_ == PROGRAM_TIME) {
        return AddDataWithProgramTime(point_name,
                                      data,
                                      double(QDateTime::currentMSecsSinceEpoch() / (long double) 1000
                                                 - program_begin_time_));//上限精度
    } else {
        return AddDataWithDateTime(point_name, data, new QDateTime(QDateTime::currentDateTime()));
    }
}

bool ChartsNext::AddDataWithProgramTime(const QString &point_name,
                                        double data,
                                        const QDateTime &time) {
    return AddDataWithProgramTime(point_name,
                                  data,
                                  double(time.toMSecsSinceEpoch() / (long double) 1000
                                             - program_begin_time_));//上限精度
}

bool ChartsNext::AddDataWithProgramTime(const QString &point_name, double data, double program_time) {
//    qDebug("启动时间%.02f,更新时间%.02f", program_begin_time_, program_time);
//    qDebug("时间差%.02f", (double) program_time - program_begin_time_);
    if (data_pool_index_.contains(point_name)) {
        struct singaldata tmp;
        tmp.data = data;
        tmp.time.program_time_ = (double) program_time;//降低上限精度

        struct DataNodeIndex index_tmp = data_pool_index_.value(point_name);
        index_tmp.data_list->append(tmp);
        *(index_tmp.is_update) = true;
        qDebug("添加数据%.02f,时间%.02f", data, (double) program_time);
        return false;
    } else {
        qDebug() << "AddDataWithProgramTime: find point fail！" << endl;
        return false;
    }
    return false;

}

bool ChartsNext::AddDataWithDateTime(const QString &point_name, double data, QDateTime *date_time) {
    if (data_pool_index_.contains(point_name)) {
        struct singaldata tmp;
        tmp.data = data;
        tmp.time.date_time_ = date_time;
        struct DataNodeIndex index_tmp = data_pool_index_.value(point_name);
        index_tmp.data_list->append(tmp);
        qDebug("添加数据%.02f,时间%s", data, qPrintable(date_time->toString("dd.MM.yyyy h:m:s ap")));
        return true;
    } else {
        qDebug() << "AddDataWithDateTime: find point fail！" << endl;
        return false;
    }
}

void ChartsNext::UpdateDataPoolIndex() {
    data_pool_index_.clear();//清除索引
    QList<DataNode>::iterator i;
    for (i = data_pool_.begin(); i != data_pool_.end(); ++i) {
        struct DataNodeIndex tmp = {
            .data_list =  &(i->data_list),
            .is_update = &(i->is_update),
            .last_draw_index = &(i->last_draw_index)
        };
        data_pool_index_.insert(i->data_name, tmp);//添加到索引
    }
}
/*****
 * IsDataPointRegistter
 * 检测是否注册过的接口
 * 成功找到返回1，失败0
*****/
[[maybe_unused]] bool ChartsNext::IsDataPointRegistter(const QString &addname) {
    if (data_pool_index_.contains(addname)) {
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


/// 加载右边的信息框
/// 变量名可以手动改？

void ChartsNext::LoadInfo(){

    ui_chart_->line_table->setRowCount(data_pool_.size());
    ui_chart_->line_table->setColumnCount(3);

    /// 动态创建控件
    for(int i=0;i<data_pool_.size();i++)
    {
        struct ChartsList node;
        node.choose_color->setText("颜色选择");
        node.check_visible->setCheckState(Qt::Checked);
        line_info_.append(node);
        disconnect(line_info_[i].choose_color,0,0,0);
        disconnect(line_info_[i].check_visible,0,0,0);
        connect(line_info_[i].check_visible,&QCheckBox::stateChanged,this,[=](int state){
            VisibleChanged(state,i);
        });
        connect(line_info_[i].choose_color,&QPushButton::clicked,this,[&,i]{
            SelectColor(i);
        });
    }

    /// 渲染右侧信息
    int count = 0;
    for (QList<DataNode>::iterator i = data_pool_.begin(); i != data_pool_.end(); ++i,count++) {
        ui_chart_->line_table->setItem(count,0,new QTableWidgetItem(data_pool_.at(count).data_name));
        ui_chart_->line_table->setCellWidget(count,1,line_info_[count].choose_color);
        ui_chart_->line_table->setCellWidget(count,2,line_info_[count].check_visible);
//        line_info_[0].check_visible->setCheckState(Qt::Checked);
    }
}


/// 颜色选择窗口
/// TODO:
///     1. 小bug：颜色选择黑色之后，要在左侧选择框点才能成功切换，不过颜色采集可以用。

void ChartsNext::SelectColor(int i){
    /// 这里的初始化颜色，改为data_pool_里面的

    QPalette plet = line_info_[i].choose_color->palette();
    QColor currentColor = plet.color(QPalette::Button);    // 当前颜色
    QColor color = QColorDialog::getColor(currentColor, this,("颜色选择"));

    if (color.isValid()){
        plet.setColor (QPalette::Button, color);
        line_info_[i].choose_color->setPalette (plet);
        data_pool_[i].line_color = color;

        // 改变图像属性
        QPen pen;
        pen.setWidth(data_pool_.at(i).line_width);//设置线宽,默认是2
        pen.setColor(data_pool_.at(i).line_color);//设置线条红色
        ui_chart_->widget->graph(i)->setPen(pen);


    }



//    LoadInfo();

    qDebug() << "button:" << i;
    ///更改后要重新渲染
    qDebug() << "choose color:" << color << endl;
}


/// 是否可见选择
/// TODO: 1. 关闭数据流之后，点checkbox会闪退

void ChartsNext::VisibleChanged(int state,int location) {
    if (state==Qt::Checked){
//        qDebug() << "changed visible" << endl;
        ui_chart_->widget->graph(location)->setVisible(true);     // 不需要重新渲染其实也可以改变

    }else if(state==Qt::Unchecked){
        ui_chart_->widget->graph(location)->setVisible(false);
//        qDebug() << "changed unvisible" << endl;
    }
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
void ChartsNext::SetProgramTime() {
    program_begin_time_ = (QDateTime::currentMSecsSinceEpoch() / (long double) 1000);
}
bool ChartsNext::AntiRegisterAllDataPoint() {
    qDebug() << "反注册全部变量 ";
    for (QList<DataNode>::iterator i = data_pool_.begin(); i != data_pool_.end(); ++i) {

        if (chart_time_type_ == DATA_TIME) {//释放DataTime里的对象（如果使用了DATA_TIME）
            for (QVector<singaldata>::iterator j = i->data_list.begin(); j != i->data_list.end(); ++j) {
                delete j->time.date_time_;
            }
        }
        //释放data_list
        i->data_list.clear();
        //释放data_pool_的对应节点
        i = data_pool_.erase(i);
        i--;//让迭代器去指向下一个元素，这样for循环才不会出错
    }
    custom_plot_->clearGraphs();
    return true;
}
