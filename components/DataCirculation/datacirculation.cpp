//
// Created by xtx on 2022/11/18.
//

// You may need to build the project (run Qt uic code generator) to get "ui_DataCirculation.h" resolved

#include "datacirculation.h"

#include <QInputDialog>
#include <QMessageBox>
#include <QFile>
#include "ComTool/Comtool.h"
#include "ui_DataCirculation.h"
#include "dataengineinterface.h"

DataCirculation::DataCirculation(int device_num, int win_num, QSettings *cfg, ToNewWidget *parent_info, QWidget *parent)
    : RepeaterWidget(parent), ui_(new Ui::DataCirculation) {
    ui_->setupUi(this);

    this->cfg_ = cfg;
    this->group_name_ = "Win" + QString::number(win_num);
    this->device_num_ = device_num;
    this->parent_info_ = parent_info;

    this->tcp_command_handle_ = (*(parent_info->devices_info))[device_num].tcp_command_handler;  // 结构体这样用
    this->tcp_info_handler_[1] = (*(parent_info->devices_info))[device_num].tcp_info_handler[1];
    this->tcp_info_handler_[2] = (*(parent_info->devices_info))[device_num].tcp_info_handler[2];
    this->tcp_info_handler_[3] = (*(parent_info->devices_info))[device_num].tcp_info_handler[3];
    this->connect_mode = (*(parent_info->devices_info))[device_num].connect_mode;

    DataCirculation::GetConstructConfig();
    ui_->comProcessMode->setCurrentIndex(process_mode_);
    ui_->comCirculationMode->setCurrentIndex(circulation_mode_);
    ui_->comDateFlowMode->setCurrentIndex(date_flow_mode_);
    ui_->comOutputMode->setCurrentIndex(output_mode_);
    RefreshBox();

    // 绑定测试按钮
    connect(ui_->btnTest, &QPushButton::clicked, this, [&] { TestCirculationMode(); });

    // 设置表格列平分
    ui_->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // combox更改逻辑
    void (QComboBox::*fp)(int) = &QComboBox::currentIndexChanged;
    connect(ui_->comProcessMode, fp, this, [&](int num) {
      process_mode_ = ProcessMode(num);
      SaveConstructConfig();
    });
    connect(ui_->comCirculationMode, fp, this, [&](int num) {
      circulation_mode_ = CirculationMode(num);
      SaveConstructConfig();
    });
    connect(ui_->comDateFlowMode, fp, this, [&](int num) {
      date_flow_mode_ = DateFlowMode(num);
      SaveConstructConfig();
    });
    connect(ui_->comOutputMode, fp, this, [&](int num) {
      output_mode_ = OutputMode(num);
      SaveConstructConfig();
    });
    connect(ui_->comTimeMode, fp, this, [&](int num) {
      time_type_ = TimeType(num);
      SaveConstructConfig();
    });

    connect(ui_->btnStart, &QPushButton::clicked, this, [&] {
      if (ui_->btnStart->text() == "停止数据流处理") {
          StopCirculation();
      } else {
          StartCirculation();
      }
    });

    connect(ui_->btnTestFlow, &QPushButton::clicked, this, [&] {
      //        QString path = QFileDialog::GetOpenFileName(this, "打开文件", "D:\\OneDriveFile\\bird\\OneDrive -
      //        xutongxin\\Competition\\xmbDebugTools\\QT\\thirdPartyTool");
      QFile file(
          "..\\thirdPartyTool\\result.txt");
      if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
          return;
      }
      int i = 0;
      while (!file.atEnd()) {
          QByteArray line = file.readLine();
          this->DoCirculation(line, QDateTime::currentDateTime());
      }
      //        QByteArray allArray = file.readAll();
      //        QString allStr = QString(allArray);
      file.close();
      //
      //        qDebug("准备把以下数据注入文件 %s", qPrintable(allStr));
      //        this->DoCirculation(allArray);
    });
}

DataCirculation::~DataCirculation() {
    delete ui_;
}

/// 数据过滤测试按钮
void DataCirculation::TestCirculationMode() {
    bool b_ok = false;
    QString test_data =
        QInputDialog::getMultiLineText(this, "QInputDialog_Intro", "请输入测试数据",
                                       "填写一项测试数据,不需要以\\r\\n结尾", &b_ok, Qt::MSWindowsFixedSizeDialogHint);
    if (b_ok && !test_data.isEmpty()) {
        if (test_data.right(2) == "\r\n") {
            test_data.remove(test_data.length() - 2, 2);
        }
        if (test_data.right(1) == "\n") {
            test_data.remove(test_data.length() - 1, 1);
        }
        qDebug() << "测试内容:" << test_data << " 模式:" << circulation_mode_;
        switch (circulation_mode_) {
            case CIRCULATION_MODE_DIRECTION:
                if (test_data == "0") {
                    ui_->tableWidget->setItem(0, 0, new QTableWidgetItem(0));
                } else {
                    if (test_data.toDouble() != 0) {
                        ui_->tableWidget->setItem(0, 1, new QTableWidgetItem(QString::number(test_data.toDouble())));
                    } else {
                        qCritical("%s 解析失败", qPrintable(test_data));
                        QMessageBox::critical(this, tr("错误"), tr("解析错误"));
                    }
                }
                break;
            case CIRCULATION_MODE_COMMA_SEPARATED:break;
            case CIRCULATION_MODE_KEY_VALUE:break;
            case CIRCULATION_MODE_JUST_FLOAT:break;
            case CIRCULATION_MODE_SCANF:break;
            case CIRCULATION_MODE_REGULARITY:break;
            case CIRCULATION_MODE_PYTHON:break;
        }
    }
}

/// 读取配置文件
void DataCirculation::GetConstructConfig() {
    qDebug("读取DataCirculation配置文件");
    cfg_->beginGroup(group_name_);
    circulation_mode_ = CirculationMode(cfg_->value("circulation_mode_", circulation_mode_).toInt());
    process_mode_ = ProcessMode(cfg_->value("process_mode_", process_mode_).toInt());
    date_flow_mode_ = DateFlowMode(cfg_->value("date_flow_mode_", date_flow_mode_).toInt());
    output_mode_ = OutputMode(cfg_->value("output_mode_", output_mode_).toInt());
    cfg_->endGroup();

}

/// 保存配置文件
void DataCirculation::SaveConstructConfig() {
    qDebug("写入DataCirculation配置文件");
    cfg_->beginGroup(group_name_);
    cfg_->setValue("circulation_mode_", circulation_mode_);
    cfg_->setValue("process_mode_", process_mode_);
    cfg_->setValue("date_flow_mode_", date_flow_mode_);
    cfg_->setValue("output_mode_", output_mode_);
    cfg_->endGroup();
    RefreshBox();
}

/// 刷新ui选项
void DataCirculation::RefreshBox() {
    bool tmp_bool = true;  // 为false就是想隐藏的
    // 模式为无时隐藏选项
    if (process_mode_ == PROCESS_MODE_NONE) {
        tmp_bool = false;
    }
    //    ui_->labelCirculationMode->setVisible(tmp_bool);
    //    ui_->comCirculationMode->setVisible(tmp_bool);
    //    ui_->comDateFlowMode->setVisible(tmp_bool);
    //    ui_->labelDateFlowMode->setVisible(tmp_bool);
    ui_->comOutputMode->setVisible(tmp_bool);
    ui_->labelOutputMode->setVisible(tmp_bool);
    ui_->btnStart->setEnabled(tmp_bool);
    tmp_bool = true;
    // 当直接外部输出时隐藏选项
    if (process_mode_ == PROCESS_MODE_OUTPUT || process_mode_ == PROCESS_MODE_NONE) {
        tmp_bool = false;
    }
    ui_->labelCirculationMode->setVisible(tmp_bool);
    ui_->comCirculationMode->setVisible(tmp_bool);
    ui_->comDateFlowMode->setVisible(tmp_bool);
    ui_->labelDateFlowMode->setVisible(tmp_bool);
    tmp_bool = true;
    if (date_flow_mode_ != DATE_FLOW_MODE_OUTPUT && process_mode_ != PROCESS_MODE_OUTPUT) {
        tmp_bool = false;
    }
    // 当不需要输出模式时关闭选项
    ui_->comOutputMode->setVisible(tmp_bool);
    ui_->labelOutputMode->setVisible(tmp_bool);

    if (circulation_mode_ == CIRCULATION_MODE_DIRECTION) {           // 判断是否为直出模式
        ui_->tableWidget->setRowHidden(0, false);
        for (int i = 1; i < 16; i++) {
            ui_->tableWidget->setRowHidden(i, true); // 隐藏后十五行
        }
    } else if ((circulation_mode_ == CIRCULATION_MODE_KEY_VALUE)) {
        for (int i = 0; i < 16; ++i) {
            ui_->tableWidget->setRowHidden(i, true);
        }
    } else {
        for (int i = 0; i < 16; i++) {
            ui_->tableWidget->setRowHidden(i, false); // 显示后十五行
        }
    }
}

/// 启动数据流过滤，绑定通道（开启数据流处理）

void DataCirculation::StartCirculation() {
    // 检查界面是否存在
    if (!(*(parent_info_->devices_info))[device_num_].has_chart) {
        qCritical("不存在绘图界面");
    }
    (*(parent_info_->devices_info))[device_num_].config_step = 4;

    qDebug() << "开始绑定数据流";
    chart_window_ = (*(parent_info_->devices_info))[device_num_].charts_windows;

    ui_->btnStart->setEnabled(false);   // 这个是按键使能的标志吗？

    // 检查变量组

//    for (auto &value : values_) {
//        chart_window_->AntiRegisterDataPoint(value.name);
//    }//依次反注册数据点

    chart_window_->AntiRegisterAllDataPoint();//反注册所有数据点
    values_.clear();
    if (circulation_mode_ != CIRCULATION_MODE_KEY_VALUE) {
        int row;        // 直接获取table的行数，来注册变量
        circulation_mode_ ? row = ui_->tableWidget->rowCount() : row = 1;       // 根据模式而调整了行数，实际上获取到的还是16行。
        for (int i = 0; i < row; i++) {
            struct value tmp_value
                {
                    ui_->tableWidget->item(i, 0)->text(), ""
                };
            values_.emplace_back(tmp_value);            // 这里直接读取qtable里面的变量名。
            chart_window_->RegisterDataPoint(tmp_value.name);           // 注册的只带变量名的数据点
        }
        chart_window_->UpdateDataPoolIndex();
    }

    chart_window_->SetProgramTime();
    chart_window_->paint_timer_->start();

    /// 加载变量到charts
    chart_window_->GetConstructConfig();           // 读配置文件
    chart_window_->LoadInfo();
    chart_window_->chart_time_type_=time_type_;

    if (ui_->missError->isChecked()) {
        disconnect(this,
                   &RepeaterWidget::OrderShowSnackbar,
                   nullptr,
                   nullptr);//不再绑定弹出提示的有关事件，即不提示错误
    }
    else
    {
        connect(this,
                &RepeaterWidget::OrderShowSnackbar,
                parent_info_->main_window,
                &MainWindow::ReceiveOrderShowSnackbar);//不再绑定弹出提示的有关事件，即不提示错误
    }


    // 绑定数据进入过滤
    if (tcp_info_handler_[1]->tcp_mode_ == TCPInfoHandle::TCP_INFO_MODE_BOTH && tcp_info_handler_[1]->is_connected_) {
        qDebug() << "绑定一号通道进入解析";
        connect(tcp_info_handler_[1], &TCPInfoHandle::RecNewData, this,
                [&](const QByteArray &data,
                    const QString &ip,
                    int port,
                    const QDateTime &time) { this->DoCirculation(data); });
    }
    if (tcp_info_handler_[2]->tcp_mode_ == TCPInfoHandle::TCP_INFO_MODE_BOTH && tcp_info_handler_[2]->is_connected_) {
        qDebug() << "绑定二号通道进入解析";
        connect(tcp_info_handler_[2], &TCPInfoHandle::RecNewData, this,
                [&](const QByteArray &data,
                    const QString &ip,
                    int port,
                    const QDateTime &time) { this->DoCirculation(data); });
    }

    if (connect_mode == 2) {
        qDebug() << "本地串口解析";
        if ((*(this->parent_info_->devices_info))[this->device_num_].com_tool == nullptr) {
            qFatal("ComTool pointer is null");
        }

        connect((*(this->parent_info_->devices_info))[this->device_num_].com_tool, &ComTool::RecNewData, this,
                [&](const QByteArray &data,
                    const QDateTime &time) { this->DoCirculation(data); });
    }

    // 完成绑定
    qDebug() << "完成数据流绑定";
    (*(parent_info_->devices_info))[device_num_].config_step = 5;
    ui_->btnStart->setText("停止数据流处理");
    ui_->btnStart->setEnabled(true);
}

/// 停止数据流过滤，关闭通道（停止数据流处理）

void DataCirculation::StopCirculation() {
    ui_->btnStart->setEnabled(false);       // 按钮使能状态

    chart_window_->DeleteWidget();          // 删除控件
    disconnect(tcp_info_handler_[1], 0, 0, 0);
    disconnect(tcp_info_handler_[2], 0, 0, 0);
//    disconnect((*(this->parent_info_->devices_info))[this->device_num_].com_tool, 0, 0, 0);

    chart_window_->SaveConstructConfig();   // 保存图像信息

    chart_window_->AntiRegisterAllDataPoint();          // 反注册全部变量
    chart_window_->UpdateDataPoolIndex();               // 关闭数据流的同时，更新data_pool_index_
//    qDebug() << "关闭数据流过滤" << endl;
    ui_->btnStart->setText("启动数据流处理");
    ui_->btnStart->setEnabled(true);
}

/// 对目标数据进行过滤
///  \param data 过滤目标数据
void DataCirculation::DoCirculation(const QByteArray &data, const QDateTime &data_time) {
    QString strtmp = data;
    qDebug("准备解析数据%s,时间%s", qPrintable(strtmp), qPrintable(data_time.toString("h:m:s")));
    QStringList buffer;
    if (strtmp.indexOf("\r\n") != -1) {
        buffer = strtmp.split("\r\n");              // 把换行符过滤掉，将ByteArray转存QStringList
    } else if (strtmp.indexOf("\n") != -1) {
        buffer = strtmp.split("\n");
    } else {
        qWarning("解析数据时出错，没有找到\n符号");
        return;
    }
    bool is_analyzed_success = true;
    for (int i = 0; i < buffer.size(); i++) {
        QString circulation_str = buffer[i];        // 将buffer每一位的数据传给circulation_str，然后进入数据解析
        if (circulation_str == "") {
            continue;
        }
        switch (circulation_mode_) {
            case CIRCULATION_MODE_DIRECTION: {
                bool ok;
                double num = circulation_str.toDouble(&ok);
                if (ok) {
                    qDebug("解析成功 %f", num);
                    GetData(ui_->tableWidget->item(0, 0)->text(), num, data_time);
                } else {
                    qCritical("%s 解析失败", qPrintable(circulation_str));
                    is_analyzed_success = false;
                    emit(OrderShowSnackbar(circulation_str + "解析失败"));
                }
                break;
            }
            case CIRCULATION_MODE_COMMA_SEPARATED: {
                QStringList result = circulation_str.split(",");
                int i = 0;
                    foreach (QString circulation_data, result) {
                        qDebug() << circulation_data;
                        bool ok;
                        double num = circulation_data.toDouble(&ok);
                        if (ok) {
                            qDebug("解析成功 %f", num);
                            GetData(ui_->tableWidget->item(i, 0)->text(), num, data_time);
                        } else {

                            qCritical("%s 解析失败", qPrintable(circulation_str));
                            is_analyzed_success = false;
                            emit(OrderShowSnackbar(circulation_str + "解析失败"));

                        }
                        i++;
                    }
                break;
            }
            case CIRCULATION_MODE_JUST_FLOAT: {
                if (circulation_str.right(4) == "\x00\x00\x80\x7f" && circulation_str.length() % 4 == 0) {
                    JustFloat(circulation_str, data_time);
                } else {
                    is_analyzed_success = false;
                    qCritical("%s 解析失败", qPrintable(circulation_str));
                    emit(OrderShowSnackbar(circulation_str + "解析失败"));
                }
            }
            case CIRCULATION_MODE_KEY_VALUE: {
                QStringList
                    result = circulation_str.split(",");            // 先把每个变量分开，在分离变量，分成"name1:value","name2:value"的形式
                for (int i = 0; i < result.size(); ++i) {
                    QStringList value;
                    value = result.at(i).split(":");            // 分成"name","value"的形式
                    bool is_contain = chart_window_->IsDataPointRegistter(value.at(0));
                    if (!is_contain) {
                        chart_window_->RegisterDataPoint(value.at(0));              // 注册数据点，放在这里有个问题，如果没有新的变量名进来，数据的点就不会被注册
                        while (chart_window_->SetColor());                                              // 设置颜色
                        chart_window_->UpdateDataPoolIndex();                                   // 更新数据池索引

                    }

                    bool ok;
                    double num = value.at(1).toDouble(&ok);
                    if (ok) {
                        qDebug("解析成功 %f", num);
                        chart_window_->AddDataWithProgramTime(value.at(0), num, data_time);
                    } else {
                        qCritical("%s 解析失败", qPrintable(circulation_str));
                        QMessageBox::critical(this, tr("错误"), tr("解析错误"));
                    }
                }
                chart_window_->LoadInfo();
                break;
            }
            case CIRCULATION_MODE_SCANF:break;
            case CIRCULATION_MODE_REGULARITY:break;
            case CIRCULATION_MODE_PYTHON:break;
        }

        if (is_analyzed_success && time_type_ == DATA_TIME) {
            data_time_now_++;
        }
        is_analyzed_success=true;
    }
}

void DataCirculation::JustFloat(const QString &str, const QDateTime &data_time) {
    const char *c_str = str.toLocal8Bit().data();
    float num;
    for (int i = 0; i < (str.length() / 4 - 1); i++) {
        memcpy(&num, c_str + i * 4, 4);
        qDebug("解析成功 %f", num);
        GetData(ui_->tableWidget->item(i, 0)->text(), num, data_time);
    }
}
void DataCirculation::GetData(const QString &point_name, double data, const QDateTime &time) {
    switch (time_type_) {
        case DATE_TIME: {
            chart_window_->AddDataWithDateTime(point_name, data, time);
            break;
        }
        case PROGRAM_TIME: {
            chart_window_->AddDataWithProgramTime(point_name, data, time);
            break;
        }
        case DATA_TIME: {
            //在这里面不负责处理data_time_now_的递增问题
            chart_window_->AddDataWithDataTime(point_name, data, data_time_now_);
            break;
        }

    }
}


//bool DataCirculation::ProcessingFrame(char *data, int count, QVector<float> &dd)
//{
//    if (count <= 0)
//        return false;
//
//    if (count % 4 == 0) {
//        // 只有数据长度是4的倍数，才是合法的浮点数组
//        for (int i = 0; i < count - 4; i += 4) {
//            //            double value = datas[i].trimmed().toDouble();
//            float value;
//            memcpy(&value, data + i, 4);
//            dd.append(value);
//        }
//        return true;
//    }
//    return false;
//}

//// 帧结构：小端浮点数组 0x7f800000
//void DataCirculation::ProcessingDatas(char *data, int count)
//{
//
//    frame_list_.clear();
//
//
//    int begin = 0, end = 0;
//    for (int i = 3; i < count; i++) {
//        char *data_ptr = data + i - 3;
//        int frame_tail_data;
//        bool frame_is_valid = false;
//
//        memcpy(&frame_tail_data, data_ptr, 4);
//        if (frame_tail_data != static_cast<int>(0x7F800000))
//            continue;
//
//        // 已经匹配到帧尾 0x7f800000
//        end = i;
//
//        int image_size = 0;
//        Frame frame;
//
//        if ((i + 4) < count) {
//            int frame_tail_data2;
//            memcpy(&frame_tail_data2, data + i + 1, 4);
//            if (frame_tail_data2 == static_cast<int>(0x7F800000)) {
//                // 匹配到2个连续的0x7f800000，这是个图片前导帧
//                i += 4;
//                if ((i - begin + 1) != 28) {
//                    // 5个图片前导帧参数 + 2个帧尾，共7个整型数据，28byte
//                    // 如果帧长度不等于28byte，说明图片前导帧格式错误
//                    break;
//                }
//
//
//                // 获取图片信息
//                int image_id;
//                int image_width;
//                int image_height;
//                RawImage::Format image_format;
//                memcpy(&image_id, data + i - 27, 4);
//                memcpy(&image_size, data + i - 23, 4);
//                memcpy(&image_width, data + i - 19, 4);
//                memcpy(&image_height, data + i - 15, 4);
//                memcpy(&image_format, data + i - 11, 4);
//                // !获取图片信息
//
//
//                if ((i + image_size) >= count) {
//                    // 图片长度超过缓冲区长度，可能还没接收完，直接返回，下次再来
//                    return;
//                }
//                if (image_id > (image_channels_.length() - 1)) {
//                    // 图片id > 图片通道数量，扩充图片通道
//                    // 在扩充图片通道之前，为过滤异常情况，保证发送了6帧大id的图片之后，再进行扩充
//
//                    image_count_mutation_count_++;
//                    if (image_id < 6 || image_count_mutation_count_ >= 6) {
//                        image_count_mutation_count_ = 0;
//                        while (image_channels_.length() < image_id + 1) {
//                            image_channels_.append(new RawImage());
//                        }
//                    }
//                }
//                if (image_id < image_channels_.length()) {
//                    // 图片id合法，把图片数据放到图片通道中
//                    image_channels_[image_id]->set((uchar*)data + i + 1, image_size,
//                                                   image_width, image_height, image_format);
//                }
//
//                // 把图片数据结尾记录为帧尾，图片前导帧+图片数据，构成了一个图片数据包
//                end = i + image_size;
//                i = end;
//                frame_is_valid = true;  // 至此，可以确定这是一个合法的图片数据包
//            } else {
//                // 解析浮点数组，将其转换为采样数据
//                frame_is_valid = ProcessingFrame(data + begin, (i - begin) + 1, frame.datas_);
//            }
//        } else {
//            // 解析浮点数组，将其转换为采样数据
//            frame_is_valid = ProcessingFrame(data + begin, (i - begin) + 1, frame.datas_);
//        }
//
//        // 记录帧 是否合法，开始位置，结束位置，图片尺寸（如果为0，标识其不是图片数据包）
//        frame.is_valid_ = frame_is_valid;
//        frame.start_index_ = begin;
//        frame.end_index_ = end;
//        frame.image_size_ = image_size;
//        frame_list_.append(frame);
//        // !记录帧
//
//        begin = i+1;
//
//    }
//}
