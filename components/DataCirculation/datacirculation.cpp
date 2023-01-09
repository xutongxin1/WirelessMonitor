//
// Created by xtx on 2022/11/18.
//

// You may need to build the project (run Qt uic code generator) to get "ui_DataCirculation.h" resolved


//TODO:添加数据流关闭按钮
#include "datacirculation.h"

#include <QDebug>
#include <QInputDialog>
#include <QMessageBox>

#include "ui_DataCirculation.h"

DataCirculation::DataCirculation(int device_num, int win_num, QSettings *cfg, ToNewWidget *parent_info, QWidget *parent)
    : RepeaterWidget(parent), ui_(new Ui::DataCirculation) {
    ui_->setupUi(this);

    this->cfg = cfg;
    this->GroupName = "Win" + QString::number(win_num);
    this->DeviceNum = device_num;
    this->parentInfo = parent_info;

    this->TCPCommandHandle = (*(parent_info->devices_info))[device_num].tcp_command_handler;  // 结构体这样用
    this->TCPInfoHandler[1] = (*(parent_info->devices_info))[device_num].tcp_info_handler[1];
    this->TCPInfoHandler[2] = (*(parent_info->devices_info))[device_num].tcp_info_handler[2];
    this->TCPInfoHandler[3] = (*(parent_info->devices_info))[device_num].tcp_info_handler[3];

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
          this->DoCirculation(line, chart_window_->startedTime.addSecs(i++));
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
    bool bOk = false;
    QString TestData =
        QInputDialog::getMultiLineText(this, "QInputDialog_Intro", "请输入测试数据",
                                       "填写一项测试数据,不需要以\\r\\n结尾", &bOk, Qt::MSWindowsFixedSizeDialogHint);
    if (bOk && !TestData.isEmpty()) {
        if (TestData.right(2) == "\r\n") {
            TestData.remove(TestData.length() - 2, 2);
        }
        if (TestData.right(1) == "\n") {
            TestData.remove(TestData.length() - 1, 1);
        }
        qDebug() << "测试内容:" << TestData << " 模式:" << circulation_mode_;
        switch (circulation_mode_) {
            case CIRCULATION_MODE_DIRECTION:
                if (TestData == "0") {
                    ui_->tableWidget->setItem(0, 0, new QTableWidgetItem(0));
                } else {
                    if (TestData.toDouble() != 0) {
                        ui_->tableWidget->setItem(0, 1, new QTableWidgetItem(QString::number(TestData.toDouble())));
                    } else {
                        qCritical("%s 解析失败", qPrintable(TestData));
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
    cfg->beginGroup(GroupName);
    circulation_mode_ = CirculationMode(cfg->value("circulation_mode_", circulation_mode_).toInt());
    process_mode_ = ProcessMode(cfg->value("process_mode_", process_mode_).toInt());
    date_flow_mode_ = DateFlowMode(cfg->value("date_flow_mode_", date_flow_mode_).toInt());
    output_mode_ = OutputMode(cfg->value("output_mode_", output_mode_).toInt());
    cfg->endGroup();
}

/// 保存配置文件
void DataCirculation::SaveConstructConfig() {
    qDebug("写入DataCirculation配置文件");
    cfg->beginGroup(GroupName);
    cfg->setValue("circulation_mode_", circulation_mode_);
    cfg->setValue("process_mode_", process_mode_);
    cfg->setValue("date_flow_mode_", date_flow_mode_);
    cfg->setValue("output_mode_", output_mode_);
    cfg->endGroup();
    RefreshBox();
}

/// 刷新ui选项
void DataCirculation::RefreshBox() {
    bool tmpBool = true;  // 为false就是想隐藏的
    // 模式为无时隐藏选项
    if (process_mode_ == PROCESS_MODE_NONE) {
        tmpBool = false;
    }
    //    ui_->labelCirculationMode->setVisible(tmpBool);
    //    ui_->comCirculationMode->setVisible(tmpBool);
    //    ui_->comDateFlowMode->setVisible(tmpBool);
    //    ui_->labelDateFlowMode->setVisible(tmpBool);
    ui_->comOutputMode->setVisible(tmpBool);
    ui_->labelOutputMode->setVisible(tmpBool);
    ui_->btnStart->setEnabled(tmpBool);
    tmpBool = true;
    // 当直接外部输出时隐藏选项
    if (process_mode_ == PROCESS_MODE_OUTPUT || process_mode_ == PROCESS_MODE_NONE) {
        tmpBool = false;
    }
    ui_->labelCirculationMode->setVisible(tmpBool);
    ui_->comCirculationMode->setVisible(tmpBool);
    ui_->comDateFlowMode->setVisible(tmpBool);
    ui_->labelDateFlowMode->setVisible(tmpBool);
    tmpBool = true;
    if (date_flow_mode_ != DATE_FLOW_MODE_OUTPUT && process_mode_ != PROCESS_MODE_OUTPUT) {
        tmpBool = false;
    }
    // 当不需要输出模式时关闭选项
    ui_->comOutputMode->setVisible(tmpBool);
    ui_->labelOutputMode->setVisible(tmpBool);
}

/// 启动数据流过滤，绑定通道
void DataCirculation::StartCirculation()
{
    // 检查界面是否存在
    if (!(*(parentInfo->devices_info))[DeviceNum].has_chart) {
        qCritical("不存在绘图界面");
    }
    (*(parentInfo->devices_info))[DeviceNum].config_step = 4;

    qDebug() << "开始绑定数据流";
    chart_window_ = (*(parentInfo->devices_info))[DeviceNum].charts_windows;

    ui_->btnStart->setEnabled(false);
    // 检查变量组

    for (auto &value : values_) {
        chart_window_->antiRegisterData(value.name);
    }
    values_.clear();
    int row = ui_->tableWidget->rowCount();
    for (int i = 0; i < row; i++) {
        struct value tmpValue
            {
                ui_->tableWidget->item(i, 0)->text(), ""
            };
        values_.emplace_back(tmpValue);
        chart_window_->registerData(tmpValue.name);
    }

    // 绑定数据进入过滤
    if (TCPInfoHandler[1]->TCPMode == TCPInfoHandle::TCPInfoMode_IN && TCPInfoHandler[1]->isConnected) {
        qDebug() << "绑定一号通道进入解析";
        connect(TCPInfoHandler[1], &TCPInfoHandle::RecNewData, this,
                [&](const QByteArray &data, const QString &ip, int port, QTime time) { this->DoCirculation(data); });
    }
    if (TCPInfoHandler[2]->TCPMode == TCPInfoHandle::TCPInfoMode_IN && TCPInfoHandler[2]->isConnected) {
        qDebug() << "绑定二号通道进入解析";
        connect(TCPInfoHandler[2], &TCPInfoHandle::RecNewData, this,
                [&](const QByteArray &data, const QString &ip, int port, QTime time) { this->DoCirculation(data); });
    }

    // 完成绑定
    qDebug() << "完成数据流绑定";
    (*(parentInfo->devices_info))[DeviceNum].config_step = 5;
    ui_->btnStart->setText("停止数据流处理");
    ui_->btnStart->setEnabled(true);
}

/// 对目标数据进行过滤
///  \param data 过滤目标数据
void DataCirculation::DoCirculation(const QByteArray &data, QTime data_time) {
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
        QString circulationStr = buffer[i];
        if (circulationStr == "") {
            continue;
        }
        switch (circulation_mode_) {
            case CIRCULATION_MODE_DIRECTION: {
                bool ok;
                double num = circulationStr.toDouble(&ok);
                if (ok) {
                    qDebug("解析成功 %f", num);
                    chart_window_->updateData(ui_->tableWidget->item(0, 0)->text(), data_time, num);
                } else {
                    qCritical("%s 解析失败", qPrintable(circulationStr));
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
