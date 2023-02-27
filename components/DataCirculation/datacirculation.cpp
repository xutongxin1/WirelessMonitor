//
// Created by xtx on 2022/11/18.
//

// You may need to build the project (run Qt uic code generator) to get "ui_DataCirculation.h" resolved


//TODO:添加数据流关闭按钮
#include "datacirculation.h"

#include <QInputDialog>
#include <QMessageBox>
#include <QFile>

#include "ui_DataCirculation.h"

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

    connect(ui_->btnStart, &QPushButton::clicked, this, [&] { StartCirculation(); });

    connect(ui_->btnTestFlow, &QPushButton::clicked, this, [&] {
      //        QString path = QFileDialog::GetOpenFileName(this, "打开文件", "D:\\OneDriveFile\\bird\\OneDrive -
      //        xutongxin\\Competition\\xmbDebugTools\\QT\\thirdPartyTool");
      QFile file(
          "D:\\OneDriveFile\\bird\\OneDrive - xutongxin\\Competition\\xmbDebugTools\\QT\\thirdPartyTool\\result.txt");
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

DataCirculation::~DataCirculation()
{
    delete ui_;
}

/// 数据过滤测试按钮
void DataCirculation::TestCirculationMode()
{
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
}

/// 启动数据流过滤，绑定通道
void DataCirculation::StartCirculation()
{
    // 检查界面是否存在
    if (!(*(parent_info_->devices_info))[device_num_].has_chart) {
        qCritical("不存在绘图界面");
    }
    (*(parent_info_->devices_info))[device_num_].config_step = 4;

    qDebug() << "开始绑定数据流";
    chart_window_ = (*(parent_info_->devices_info))[device_num_].charts_windows;

    ui_->btnStart->setEnabled(false);
    // 检查变量组

//    for (auto &value : values_) {
//        chart_window_->AntiRegisterDataPoint(value.name);
//    }//依次反注册数据点

    chart_window_->AntiRegisterAllDataPoint();//反注册所有数据点
    values_.clear();
    int row = ui_->tableWidget->rowCount();
    for (int i = 0; i < row; i++) {
        struct value tmp_value
            {
                ui_->tableWidget->item(i, 0)->text(), ""
            };
        values_.emplace_back(tmp_value);
        chart_window_->RegisterDataPoint(tmp_value.name);
    }

    chart_window_->UpdateDataPoolIndex();
    chart_window_->SetProgramTime();
    chart_window_->paint_timer_->start();

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

    // 完成绑定
    qDebug() << "完成数据流绑定";
    (*(parent_info_->devices_info))[device_num_].config_step = 5;
    ui_->btnStart->setText("停止数据流处理");
    ui_->btnStart->setEnabled(true);
}

/// 对目标数据进行过滤
///  \param data 过滤目标数据
void DataCirculation::DoCirculation(const QByteArray &data, const QDateTime &data_time) {
    QString strtmp = data;
    qDebug("准备解析数据%s,时间%s", qPrintable(strtmp), qPrintable(data_time.toString("h:m:s")));
    QStringList buffer;
    if (strtmp.indexOf("\r\n") != -1) {
        buffer = strtmp.split("\r\n");
    } else if (strtmp.indexOf("\n") != -1) {
        buffer = strtmp.split("\n");
    } else {
        qCritical("解析数据时出错，没有找到\n符号");
        return;
    }
    for (int i = 0; i < buffer.size(); i++) {
        QString circulation_str = buffer[i];
        if (circulation_str == "") {
            continue;
        }
        switch (circulation_mode_) {
            case CIRCULATION_MODE_DIRECTION: {
                bool ok;
                double num = circulation_str.toDouble(&ok);
                if (ok) {
                    qDebug("解析成功 %f", num);
                    chart_window_->AddDataWithProgramTime(ui_->tableWidget->item(0, 0)->text(), num, data_time);
                } else {
                    qCritical("%s 解析失败", qPrintable(circulation_str));
                    QMessageBox::critical(this, tr("错误"), tr("解析错误"));
                }
                break;
            }
            case CIRCULATION_MODE_COMMA_SEPARATED:break;
            case CIRCULATION_MODE_KEY_VALUE:break;
            case CIRCULATION_MODE_SCANF:break;
            case CIRCULATION_MODE_REGULARITY:break;
            case CIRCULATION_MODE_PYTHON:break;
        }
    }
}
