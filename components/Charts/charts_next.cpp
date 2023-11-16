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
bool rolling_flag = true;
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
    ui_chart_->widget->setOpenGl(true);
    qDebug() << "opengle=" << ui_chart_->widget->openGl();
//    ui_chart_->widget->setNoAntialiasingOnDrag(true);
//    ui_chart_->widget->setNotAntialiasedElements(
//        QCP::aeAxes | QCP::aeGrid | QCP::aeSubGrid | QCP::aeLegend | QCP::aeLegendItems | QCP::aeZeroLine
//            | QCP::aeOther);
//    ui_chart_->widget->setAntialiasedElements(QCP::aeItems);

    ui_chart_->widget->xAxis->setLabel("Time/秒");
    ui_chart_->widget->yAxis->setLabel("ADC");
    ui_chart_->widget->xAxis->setRange(0, 100);     // 设置默认范围
    ui_chart_->widget->yAxis->setRange(0, 100);
    ui_chart_->widget->legend->setVisible(true);           // 是否显示图例，legend是管理图例的类

    ui_chart_->widget->xAxis2->setVisible(true);            // 上边轴
    ui_chart_->widget->xAxis2->setTickLabels(false);        // 不显示刻度
    ui_chart_->widget->yAxis2->setVisible(true);
    ui_chart_->widget->yAxis2->setTickLabels(false);
    connect(ui_chart_->widget->xAxis,
            SIGNAL(rangeChanged(QCPRange)),
            ui_chart_->widget->xAxis2,
            SLOT(setRange(QCPRange)));
    connect(ui_chart_->widget->yAxis,
            SIGNAL(rangeChanged(QCPRange)),
            ui_chart_->widget->yAxis2,
            SLOT(setRange(QCPRange)));                  // 将上边右边轴与左边下边轴同步变化

    //设置基本坐标轴（左侧Y轴和下方X轴）可拖动、可缩放、曲线可选、legend可选、设置伸缩比例，使所有图例可见
    ui_chart_->widget->setInteractions(
            QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes | QCP::iSelectLegend | QCP::iSelectPlottables);

    connect(ui_chart_->widget, SIGNAL(mouseMove(QMouseEvent * )), this, SLOT(myMoveEvent(QMouseEvent * )));

    //设置legend只能选择图例
    ui_chart_->widget->legend->setSelectableParts(QCPLegend::spItems);
    connect(ui_chart_->widget, &QCustomPlot::selectionChangedByUser, this, [&] {
        // 这个信号貌似有值
        SelectionChanged();
    });
    custom_plot_ = ui_chart_->widget;
    custom_plot_->axisRect()->insetLayout()->setInsetAlignment(0,
                                                               Qt::AlignTop
                                                               |
                                                               Qt::AlignLeft);       // 设置图例在左上，这句不能放上去，要等整张图像画出来，才能设置位置
    UpdateLine();

    // 重写滚轮事件
    connect(ui_chart_->widget, &QCustomPlotWithMouse::RwheelEvent, this, [&] {
//        double x = data_pool_[i].data_list.last().time.program_time_;       // 获取最新的X值
//        double y = data_pool_[i].data_list.last().data;                     // 获取最新的y值
//        double y_lower = ui_chart_->widget->yAxis->range().lower;           // 获取坐标轴当前最小的y值
//        double y_low = ui_chart_->widget->yAxis->range().lower;
//        double y_high = ui_chart_->widget->yAxis->range().upper;
//        double dis_y = y_high-y_low;
//        double x_low = ui_chart_->widget->xAxis->range().lower;
//        double x_high = ui_chart_->widget->xAxis->range().upper;
//        double dis_x = x_high-x_low;
//        ui_chart_->widget->xAxis->setRange(x_low + dis_x, x_high+dis_x);          // 刷新x的范围
//        ui_chart_->widget->yAxis->setRange(y_low + dis_y, y_high+dis_y);    // 刷新y的范围  这些加减都是随便设的
//        double y_low = ui_chart_->widget->yAxis->range().lower;
        double y_high = ui_chart_->widget->yAxis->range().upper;    // 获取y轴的最大值
        double y = data_pool_[0].data_list.last().data;
        // newdata+distance
        distance_y = y_high - y;
        qDebug() << "distance" << distance_y;
    });

    // 重写鼠标释放事件
    connect(ui_chart_->widget, &QCustomPlotWithMouse::RmouseReleaseEvent, this, [&] {
        double y_low = ui_chart_->widget->yAxis->range().lower;
        double y_high = ui_chart_->widget->yAxis->range().upper;
//        qDebug() << y_low << " " << y_high;
    });

    // 图表右键菜单
    ui_chart_->widget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui_chart_->widget, &QWidget::customContextMenuRequested, this, [&](const QPoint pos) {

        QMenu *menu = new QMenu(ui_chart_->widget);

        QCheckBox *control = new QCheckBox("是否滚动", ui_chart_->widget);
        rolling_flag ? control->setCheckState(Qt::Checked) : control->setCheckState(
                Qt::Unchecked);         // 设置选择框的默认状态
        QWidgetAction *choice = new QWidgetAction(ui_chart_->widget);
        choice->setDefaultWidget(control);                                       // 在Action对象中添加控件

        QAction *restore = new QAction("自动", ui_chart_->widget);
        connect(control, &QCheckBox::stateChanged, this, [&, this](int state) {
            state == Qt::Checked ? rolling_flag = true : rolling_flag = false;        // 设置是否滚动
        });

        connect(restore, &QAction::triggered, this, [&] {
            //默认是不滚动的
//            qDebug() << "auto()";
            for (int i = 0; i < data_pool_.size(); ++i) {
                custom_plot_->graph(i)->rescaleAxes();              // 自动调整图像，首尾点都显示出来
            }
            custom_plot_->replot();
        });
        menu->addAction(choice);
        menu->addAction(restore);
        menu->move(cursor().pos());           // 将菜单窗口移动到鼠标的坐标
        menu->show();

    });

}

ChartsNext::~ChartsNext() {
    delete ui_chart_;
}

// 绘制图线
/// 触发条件为时间，所以函数一直都会运行。当关闭数据流过滤的时候，data_pool_size()=0，循环就不能执行，相当于函数不运行了。
void ChartsNext::UpdateLine() {
    bool has_update = false;
    for (int i = 0; i < data_pool_.size(); i++) {
        if (data_pool_.at(i).last_draw_index == -1)//首次绘制(或修改颜色后首次)
        {
            QPen pen;
            pen.setWidth(data_pool_.at(i).line_width);//设置线宽,默认是2
            pen.setColor(data_pool_.at(i).line_color);//设置线条红色
            custom_plot_->addGraph();
            custom_plot_->graph()->setAntialiased(true);//抗锯齿
            custom_plot_->graph(i)->setPen(pen);
            custom_plot_->graph(i)->setName(data_pool_.at(i).data_name);

            // 散点测试
            custom_plot_->graph(i)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 6));
//            custom_plot_->graph(i)->setData(latVector, lonVector);
//            custom_plot_->graph(i)->setLineStyle(QCPGraph::lsNone);     // 散点图

            if (data_pool_.at(i).is_visible) {
                custom_plot_->graph(i)->setVisible(true);
            } else {
                custom_plot_->graph(i)->setVisible(false);
            }
            data_pool_[i].last_draw_index = 0;

        }
        // 应该对大概出问题了，超过1000个点卡住了
        if (data_pool_.at(i).is_update) {
            has_update = true;
            this->data_pool_[i].is_update = false;

            for (int j = data_pool_.at(i).last_draw_index; j < data_pool_.at(i).data_list.size(); j++) {
                switch (chart_time_type_) {

                    case PROGRAM_TIME: {
                        custom_plot_->graph(i)->addData(
                                data_pool_[i].data_list.at(j).time.program_time_,
                                data_pool_[i].data_list.at(j).data);
                        break;
                    }
                    case DATE_TIME: {
                        custom_plot_->graph(i)->addData(
                                data_pool_[i].data_list.at(j).time.date_time_->toMSecsSinceEpoch(),
                                data_pool_[i].data_list.at(j).data);
                        break;
                    }
                    case DATA_TIME: {
                        custom_plot_->graph(i)->addData(
                                double(data_pool_[i].data_list.at(j).time.data_time_),
                                data_pool_[i].data_list.at(j).data);
                        break;
                    }
                }
            }
            /// TODO:   改变坐标轴, 刷轴，为什么改变任何图像属性，并不会触发重绘了？？？
            if (rolling_flag) {
                double x = data_pool_[i].data_list.last().time.program_time_;       // 获取最新的X值
                double y = data_pool_[i].data_list.last().data;                     // 获取最新的y值

                double y_low = ui_chart_->widget->yAxis->range().lower;
                double y_high = ui_chart_->widget->yAxis->range().upper;
                distance_y = y_high - y_low;
                double x_low = ui_chart_->widget->xAxis->range().lower;
                double x_high = ui_chart_->widget->xAxis->range().upper;
                distance_x = x_high - x_low;
//                ui_chart_->widget->xAxis->setRange(x_low + distance_x, x_high + distance_x);          // 刷新x的范围

                ui_chart_->widget->yAxis->setRange(y_low + distance_y, y_high + distance_y);    // 刷新y的范围  这些加减都是随便设的
                qDebug() << ui_chart_->widget->yAxis->range().upper;
                //                qDebug() << distance_y << " " << distance_x;

//                double y_lower = ui_chart_->widget->yAxis->range().lower;           // 获取坐标轴当前最小的y值
//                ui_chart_->widget->xAxis->setRange(x * 0.75, x);          // 刷新x的范围
//                ui_chart_->widget->yAxis->setRange(y_lower, abs(y) * 2);    // 刷新y的范围  这些加减都是随便设的
            }
            data_pool_[i].last_draw_index = data_pool_[i].data_list.size();

        }
    }
    if (has_update) {
        custom_plot_->replot(QCustomPlot::rpQueuedReplot);
    }
}

void ChartsNext::myMoveEvent(QMouseEvent *event) {
    //获取鼠标坐标，相对父窗体坐标
    int c_flag = 0;                     // 应该是一个是否选中线条的标志
    int x_pos = event->pos().x();       // 获取x坐标
    int y_pos = event->pos().y();       // 获取y坐标
//    qDebug() << "event->pos()" << event->pos();

    //鼠标坐标转化为CustomPlot内部坐标
    float x_val = ui_chart_->widget->xAxis->pixelToCoord(x_pos);    // 转换为坐标轴对应的值
    float y_val = ui_chart_->widget->yAxis->pixelToCoord(y_pos);    // 转换为坐标轴对应的值
    static float line_y_val = 0;
    //获得x轴坐标位置对应的曲线上y的值
    for (int i = 0; i < ui_chart_->widget->graphCount(); ++i) {
        QCPGraph *graph = ui_chart_->widget->graph(i);
        if (graph->selected()) {
            line_y_val = ui_chart_->widget->graph(i)->data()->at(x_val)->value;     // 获取选中点的y值
            c_flag = 1;         // 图线选中
        }
    }

    //曲线的上点坐标位置，用来显示QToolTip提示框
    float out_x = ui_chart_->widget->xAxis->coordToPixel(x_val);
    float out_y = ui_chart_->widget->yAxis->coordToPixel(y_val);
    float out_value = ui_chart_->widget->yAxis->coordToPixel(line_y_val);       // 这个值有问题
    QString str, strToolTip;
    str = QString::number(x_val, 10, 3);
    strToolTip += "Time: ";
    strToolTip += str;
    strToolTip += "\n";

    /// 显示提示框
    if (c_flag) {
        /// 这部分是选中逻辑
        str = QString::number(y_val, 10, 3);
        strToolTip += "ADC: ";
        strToolTip += str;
        strToolTip += "\n";
        /// 这里这么写是有什么用吗，选中之后提示有什么作用？
        QToolTip::showText(mapToGlobal(QPoint(out_x, out_y)), strToolTip, ui_chart_->widget);

    }

//    else {
//        /// 这部分是默认逻辑
//        str = QString::number(y_val, 10, 3);
//        strToolTip += "ADC: ";
//        strToolTip += str;
//        strToolTip += "\n";
//        QToolTip::showText(mapToGlobal(QPoint(out_x, out_y)), strToolTip, ui_chart_->widget);
//    }
}

//void ChartsNext::on_pushButton_clicked() {
//    if (checked == 0) {
//        //开启，不可以放缩和移动
//        //qDebug() << "开始画图 \r\n";
//        ui_chart_->start_stop->setText("暂停显示");
//        //timerChart->start();//每200ms重绘一次折线图
//        //ui_chart_->widget->setInteractions(QCP::iNone);
//        ui_chart_->widget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
//        hide_flag = 1;
//        checked = 1;
//    } else {
//        //关闭，可以放缩和移动
//        ui_chart_->start_stop->setText("开始显示");
//        //timerChart->stop();
//        ui_chart_->widget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
//        hide_flag = 0;
//        checked = 0;
//    }
//}

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

    qDebug() << "RegisterDataPoint:register success!\r\n";
    qDebug() << (data_pool_.size()) << "\r\n";
    return true;

}

//void ChartsNext::on_pushButton_yincang_clicked() {
////    //点击鼠标然后隐藏
////    for (int i = 0; i < ui_chart_->widget->graphCount(); ++i) {
////        QCPGraph *graph = ui_chart_->widget->graph(i);
////        if (graph->selected()) {
////            DataPairs[i].flag = 2;
////            ui_chart_->widget->graph(i)->setVisible(false);//隐藏
////        }
////    }
////    ui_chart_->widget->replot();//重绘图形
//}

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
            if (chart_time_type_ == DATE_TIME) {//释放DataTime里的对象（如果使用了DATA_TIME）
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

    qCritical() << "AntiRegisterDataPoint: fail！\r\n";
    return false;
}

/*****
 * updateData
 * 中介是Data_pools
 * 成功返回1，失败0
*****/
//bool ChartsNext::AddDataAuto(const QString &point_name, double data) {
//    if (chart_time_type_ == PROGRAM_TIME) {
//        return AddDataWithProgramTime(point_name,
//                                      data,
//                                      double(QDateTime::currentMSecsSinceEpoch() / (long double) 1000
//                                                 - program_begin_time_));//上限精度
//    } else {
//        return AddDataWithDateTime(point_name, data, new QDateTime(QDateTime::currentDateTime()));
//    }
//}

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
        qDebug() << "AddDataWithProgramTime: find point fail！\r\n";
        return false;
    }
    return false;

}

bool ChartsNext::AddDataWithDataTime(const QString &point_name, double data, int data_time) {
    if (data_pool_index_.contains(point_name)) {
        struct singaldata tmp;
        tmp.data = data;
        tmp.time.data_time_ = data_time;
        struct DataNodeIndex index_tmp = data_pool_index_.value(point_name);
        index_tmp.data_list->append(tmp);
        *(index_tmp.is_update) = true;
        qDebug("添加数据%.02f,时间%d", data, data_time);
        return true;
    } else {
        qDebug() << "AddDataWithDateTime: find point fail！\r\n";
        return false;
    }
}

bool ChartsNext::AddDataWithDateTime(const QString &point_name, double data, const QDateTime &date_time) {
    if (data_pool_index_.contains(point_name)) {
        struct singaldata tmp;
        tmp.data = data;
        tmp.time.date_time_ = &date_time;
        struct DataNodeIndex index_tmp = data_pool_index_.value(point_name);
        index_tmp.data_list->append(tmp);
        qDebug("添加数据%.02f,时间%s", data, qPrintable(date_time.toString("dd.MM.yyyy h:m:s ap")));
        return true;
    } else {
        qDebug() << "AddDataWithDateTime: find point fail！\r\n";
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
bool ChartsNext::IsDataPointRegistter(const QString &addname) {
    if (data_pool_index_.contains(addname)) {
//        qDebug() << "check: find！\r\n";
        return true;
    } else {
//        qDebug() << "check: find fail！\r\n";
        return false;
    }
}

// 随机设置颜色
bool ChartsNext::SetColor() {
    int r = rand() % 256;
    int g = rand() % 256;
    int b = rand() % 256;
    QColor data_color = QColor(r, g, b);
    for (int i = 0; i < data_pool_.size(); ++i) {
        if (data_color == data_pool_.at(i).line_color) {
            return true;            // 有重复的，再次执行
        }
    }
    data_pool_.last().line_color = data_color;      // 新增的应该是在最后吧
    qDebug() << data_pool_.last().data_name << " " << data_pool_.last().line_color;
    return false;
}

void ChartsNext::test(const QVector<double> &addDate) {
    double *temp;
    int size = addDate.size();
    temp = new double[size];

    for (int i = 0; i < addDate.size(); i++) {
        temp[i] = addDate.at(i);
    }
    qDebug() << temp[3] << "\r\n";
}

/// 加载右边的信息框
/// 变量名可以手动改？
void ChartsNext::LoadInfo() {
    ui_chart_->line_table->setRowCount(data_pool_.size());
    ui_chart_->line_table->setColumnCount(3);
    line_info_.clear();         // 清空上一次的line_info_，防止重复渲染叠加
    /// 动态创建控件
    for (int i = 0; i < data_pool_.size(); i++) {
        ChartsList node;
        // 设置按钮颜色
        QPalette plet = node.choose_color->palette();
        plet.setColor(QPalette::Button, data_pool_[i].line_color);
        node.choose_color->setPalette(plet);
        node.choose_color->setText("颜色选择");

        // 设置默认选择框属性
        data_pool_[i].is_visible ? node.check_visible->setCheckState(Qt::Checked)
                                 : node.check_visible->setCheckState(Qt::Unchecked);

        line_info_.append(node);
        disconnect(line_info_[i].choose_color, 0, 0, 0);            // 防止重复connect
        disconnect(line_info_[i].check_visible, 0, 0, 0);

        connect(line_info_[i].check_visible, &QCheckBox::stateChanged, this, [=, this](int state) {
            VisibleChanged(state, i);
        });
        connect(line_info_[i].choose_color, &QPushButton::clicked, this, [&, i] {
            SelectColor(i);
        });
    }

    /// 渲染右侧信息
    for (int i = 0; i < data_pool_.size(); ++i) {
        // QTable添加文字，控件
        ui_chart_->line_table->setItem(i, 0, new QTableWidgetItem(data_pool_.at(i).data_name));     // 添加变量名
        ui_chart_->line_table->item(i, 0)->setTextAlignment(Qt::AlignCenter);                        // 文字居中
        ui_chart_->line_table->setCellWidget(i, 1, line_info_[i].choose_color);                         // 颜色选择控件
        ui_chart_->line_table->setCellWidget(i, 2, line_info_[i].check_visible);                        // 显示选择框

    }
}

/// 删除表格中的控件
/// 先删除控件，再删除行，控件是动态创建的，要delete
void ChartsNext::DeleteWidget() {
    for (int i = 0; i < data_pool_.size(); ++i) {
        ui_chart_->line_table->removeRow(0);
    }
    line_info_.clear();
    qDebug() << "DeleteWidget\n";
}
/// 颜色选择窗口
/// TODO:
///     1. 小bug：颜色选择黑色之后，要在左侧选择框点才能成功切换，不过颜色采集可以用。

void ChartsNext::SelectColor(int i) {

    QPalette plet = line_info_[i].choose_color->palette();
    QColor current_color = plet.color(QPalette::Button);    // 当前颜色
    QColor color = QColorDialog::getColor(current_color, this, ("颜色选择"));

    if (color.isValid()) {
        // 改变按钮颜色
        plet.setColor(QPalette::Button, color);
        line_info_[i].choose_color->setPalette(plet);
        data_pool_[i].line_color = color;

        // 改变图像属性
        QPen pen;
        pen.setWidth(data_pool_.at(i).line_width);//设置线宽,默认是2
        pen.setColor(data_pool_.at(i).line_color);//设置线条红色
        ui_chart_->widget->graph(i)->setPen(pen);
    }
    ui_chart_->widget->replot();

}

/// 是否可见选择

void ChartsNext::VisibleChanged(int state, int location) {
    if (state == Qt::Checked) {
        data_pool_[location].is_visible = true;
        ui_chart_->widget->graph(location)->setVisible(true);     // 不需要重新渲染其实也可以改变

    } else if (state == Qt::Unchecked) {
        data_pool_[location].is_visible = false;
        ui_chart_->widget->graph(location)->setVisible(false);
    }
    ui_chart_->widget->replot();
}

void ChartsNext::SelectionChanged() {
    // 好像失效了
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

        if (chart_time_type_ == DATE_TIME) {//释放DataTime里的对象（如果使用了DATA_TIME）
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
    custom_plot_->clearGraphs();            // 清除图像
    custom_plot_->replot();
    return true;
}

/// 保存图像配置
/// notes: groupname不能写中文

void ChartsNext::SaveConstructConfig() {
    qDebug("写入Charts配置文件");

    for (int i = 0; i < data_pool_.size(); i++) {
        cfg_->beginGroup(group_name_);
        cfg_->setValue("Value" + QString::number(i + 1), data_pool_.at(i).data_name);     /// 变量名
        cfg_->setValue("Color" + QString::number(i + 1), data_pool_.at(i).line_color);      /// 颜色
        cfg_->setValue("Visible" + QString::number(i + 1), data_pool_.at(i).is_visible);   /// 是否可见
        cfg_->setValue("Width" + QString::number(i + 1), data_pool_.at(i).line_width);   /// 宽度
        cfg_->endGroup();
    }

    qDebug() << "saveconfig success\n";

}

/// 读取图像配置
void ChartsNext::GetConstructConfig() {
    qDebug("Get charts config");

    for (int i = 0; i < data_pool_.size(); i++) {
        cfg_->beginGroup(group_name_);
        QVariant value = cfg_->value("Color" + QString::number(i + 1));        // 读出来的是color的hex值
        QColor color = value.value<QColor>();           // 这是一个QColor(ARGB 1, 1, 0, 0)
        data_pool_[i].line_color = color;                                                               /// 颜色
        data_pool_[i].is_visible = cfg_->value("Visible" + QString::number(i + 1)).toBool();          /// 是否可见
        data_pool_[i].line_width = cfg_->value("Width" + QString::number(i + 1)).toInt();             /// 宽度
        cfg_->endGroup();
    }

    qDebug() << "getconfig success\n";
}

