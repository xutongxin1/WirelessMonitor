//
// Created by xtx on 2022/11/18.
//

// You may need to build the project (run Qt uic code generator) to get "ui_DataCirculation.h" resolved

#include <QInputDialog>
#include "datacirculation.h"
#include "ui_DataCirculation.h"
#include <QDebug>
#include <QMessageBox>

DataCirculation::DataCirculation(int DeviceNum, int winNum, QSettings *cfg, ToNewWidget *parentInfo, QWidget *parent) :
        RepeaterWidget(parent), ui(new Ui::DataCirculation) {
    ui->setupUi(this);

    this->cfg = cfg;
    this->GroupName = "Win" + QString::number(winNum);
    this->DeviceNum = DeviceNum;
    this->parentInfo = parentInfo;

    this->TCPCommandHandle = (*(parentInfo->DevicesInfo))[DeviceNum].TCPCommandHandler;//结构体这样用
    this->TCPInfoHandler[1] = (*(parentInfo->DevicesInfo))[DeviceNum].TCPInfoHandler[1];
    this->TCPInfoHandler[2] = (*(parentInfo->DevicesInfo))[DeviceNum].TCPInfoHandler[2];
    this->TCPInfoHandler[3] = (*(parentInfo->DevicesInfo))[DeviceNum].TCPInfoHandler[3];

    DataCirculation::GetConstructConfig();
    ui->comProcessMode->setCurrentIndex(processMode);
    ui->comCirculationMode->setCurrentIndex(circulationMode);
    ui->comDateFlowMode->setCurrentIndex(dateFlowMode);
    ui->comOutputMode->setCurrentIndex(outputMode);
    RefreshBox();

    //绑定测试按钮
    connect(ui->btnTest, &QPushButton::clicked, this, [&] {
        TestCirculationMode();
    });
    //设置表格列平分
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    //combox更改逻辑
    void (QComboBox::*fp)(int) =&QComboBox::currentIndexChanged;
    connect(ui->comProcessMode, fp, this, [&](int num) {
        processMode = ProcessMode(num);
        SaveConstructConfig();
    });
    connect(ui->comCirculationMode, fp, this, [&](int num) {
        circulationMode = CirculationMode(num);
        SaveConstructConfig();
    });
    connect(ui->comDateFlowMode, fp, this, [&](int num) {
        dateFlowMode = DateFlowMode(num);
        SaveConstructConfig();
    });
    connect(ui->comOutputMode, fp, this, [&](int num) {
        outputMode = OutputMode(num);
        SaveConstructConfig();
    });

    connect(ui->btnStart, &QPushButton::clicked, this, [&] {
        StartCirculation();
    });

    connect(ui->btnTestFlow, &QPushButton::clicked, this, [&] {

//        QString path = QFileDialog::getOpenFileName(this, "打开文件", "D:\\OneDriveFile\\bird\\OneDrive - xutongxin\\Competition\\xmbDebugTools\\QT\\thirdPartyTool");
        QFile file(
                "D:\\OneDriveFile\\bird\\OneDrive - xutongxin\\Competition\\xmbDebugTools\\QT\\thirdPartyTool\\result.txt");
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            return;
        }
        int i = 0;
        while (!file.atEnd()) {
            QByteArray line = file.readLine();
            this->DoCirculation(line, chartWindow->startedTime.addSecs(i++));
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
    delete ui;
}

///数据过滤测试按钮
void DataCirculation::TestCirculationMode() {
    bool bOk = false;
    QString TestData = QInputDialog::getMultiLineText(this,
                                                      "QInputDialog_Intro",
                                                      "请输入测试数据",
                                                      "填写一项测试数据,不需要以\\r\\n结尾",
                                                      &bOk, Qt::MSWindowsFixedSizeDialogHint
    );
    if (bOk && !TestData.isEmpty()) {
        if (TestData.right(2) == "\r\n")TestData.remove(TestData.length() - 2, 2);
        if (TestData.right(1) == "\n")TestData.remove(TestData.length() - 1, 1);
        qDebug() << "测试内容:" << TestData << " 模式:" << circulationMode;
        switch (circulationMode) {
            case CirculationMode_Direction:
                if (TestData == "0") { ui->tableWidget->setItem(0, 0, new QTableWidgetItem(0)); }
                else {
                    if (TestData.toDouble() != 0) {
                        ui->tableWidget->setItem(0, 1, new QTableWidgetItem(
                                QString::number(TestData.toDouble())));
                    }
                    else {
                        qCritical("%s 解析失败", qPrintable(TestData));
                        QMessageBox::critical(this, tr("错误"), tr("解析错误"));
                    }
                }
                break;
            case CirculationMode_CommaSeparated:
                break;
            case CirculationMode_KeyValue:
                break;
            case CirculationMode_Scanf:
                break;
            case CirculationMode_Regularity:
                break;
            case CirculationMode_Python:
                break;
        }

    }

}

///读取配置文件
void DataCirculation::GetConstructConfig() {
    qDebug("读取DataCirculation配置文件");
    cfg->beginGroup(GroupName);
    circulationMode = CirculationMode(cfg->value("circulationMode", circulationMode).toInt());
    processMode = ProcessMode(cfg->value("processMode", processMode).toInt());
    dateFlowMode = DateFlowMode(cfg->value("dateFlowMode", dateFlowMode).toInt());
    outputMode = OutputMode(cfg->value("outputMode", outputMode).toInt());
    cfg->endGroup();
}

///保存配置文件
void DataCirculation::SaveConstructConfig() {
    qDebug("写入DataCirculation配置文件");
    cfg->beginGroup(GroupName);
    cfg->setValue("circulationMode", circulationMode);
    cfg->setValue("processMode", processMode);
    cfg->setValue("dateFlowMode", dateFlowMode);
    cfg->setValue("outputMode", outputMode);
    cfg->endGroup();
    RefreshBox();
}

///刷新ui选项
void DataCirculation::RefreshBox() {
    bool tmpBool = true;//为false就是想隐藏的
    //模式为无时隐藏选项
    if (processMode == ProcessMode_None)tmpBool = false;
//    ui->labelCirculationMode->setVisible(tmpBool);
//    ui->comCirculationMode->setVisible(tmpBool);
//    ui->comDateFlowMode->setVisible(tmpBool);
//    ui->labelDateFlowMode->setVisible(tmpBool);
    ui->comOutputMode->setVisible(tmpBool);
    ui->labelOutputMode->setVisible(tmpBool);
    ui->btnStart->setEnabled(tmpBool);
    tmpBool = true;
    //当直接外部输出时隐藏选项
    if (processMode == ProcessMode_Output || processMode == ProcessMode_None)tmpBool = false;
    ui->labelCirculationMode->setVisible(tmpBool);
    ui->comCirculationMode->setVisible(tmpBool);
    ui->comDateFlowMode->setVisible(tmpBool);
    ui->labelDateFlowMode->setVisible(tmpBool);
    tmpBool = true;
    if (dateFlowMode != DateFlowMode_Output && processMode != ProcessMode_Output)tmpBool = false;
    //当不需要输出模式时关闭选项
    ui->comOutputMode->setVisible(tmpBool);
    ui->labelOutputMode->setVisible(tmpBool);

}

///启动数据流过滤，绑定通道
void DataCirculation::StartCirculation() {
    //检查界面是否存在
    if (!(*(parentInfo->DevicesInfo))[DeviceNum].hasChart) {
        qCritical("不存在绘图界面");
    }

    qDebug() << "开始绑定数据流";
    chartWindow = (*(parentInfo->DevicesInfo))[DeviceNum].ChartsWindows;

    ui->btnStart->setEnabled(false);
    //检查变量组

    for (auto &value: values) {
        chartWindow->antiRegisterData(value.name);
    }
    values.clear();
    int row = ui->tableWidget->rowCount();
    for (int i = 0; i < row; i++) {
        struct value tmpValue{ui->tableWidget->item(i, 0)->text(), ""};
        values.emplace_back(tmpValue);
        chartWindow->registerData(tmpValue.name);
    }

    //绑定数据进入过滤
    if (TCPInfoHandler[1]->TCPMode == TCPInfoHandle::TCPInfoMode_IN && TCPInfoHandler[1]->isConnected) {
        qDebug() << "绑定一号通道进入解析";
        connect(TCPInfoHandler[1], &TCPInfoHandle::RecNewData, this,
                [&](const QByteArray &data, const QString &ip, int port, QTime time) {
                    this->DoCirculation(data);
                });
    }
    if (TCPInfoHandler[2]->TCPMode == TCPInfoHandle::TCPInfoMode_IN && TCPInfoHandler[2]->isConnected) {
        qDebug() << "绑定二号通道进入解析";
        connect(TCPInfoHandler[2], &TCPInfoHandle::RecNewData, this,
                [&](const QByteArray &data, const QString &ip, int port, QTime time) {
                    this->DoCirculation(data);
                });
    }

    //完成绑定
    qDebug() << "完成数据流绑定";
    ui->btnStart->setText("停止数据流处理");
    ui->btnStart->setEnabled(true);
}

///对目标数据进行过滤
/// \param data 过滤目标数据
void DataCirculation::DoCirculation(const QByteArray &data, QTime dataTime) {
    QString strtmp = data;
    qDebug("准备解析数据%s,时间%s", qPrintable(strtmp), qPrintable(dataTime.toString("h:m:s")));
    QStringList buffer;
    if (strtmp.indexOf("\r\n") != -1) {
        buffer = strtmp.split("\r\n");
    }
    else if (strtmp.indexOf("\n") != -1) {
        buffer = strtmp.split("\n");
    }
    else {
        qCritical("解析数据时出错，没有找到\n符号");
        return;
    }
    for (int i = 0; i < buffer.size(); i++) {
        QString circulationStr = buffer[i];
        if (circulationStr == "")continue;
        switch (circulationMode) {
            case CirculationMode_Direction: {
                bool ok;
                double num = circulationStr.toDouble(&ok);
                if (ok) {
                    qDebug("解析成功 %f", num);
                    chartWindow->updateData(ui->tableWidget->item(0, 0)->text(), dataTime, num);
                }
                else {
                    qCritical("%s 解析失败", qPrintable(circulationStr));
                    QMessageBox::critical(this, tr("错误"), tr("解析错误"));
                }
                break;
            }
            case CirculationMode_CommaSeparated:
                break;
            case CirculationMode_KeyValue:
                break;
            case CirculationMode_Scanf:
                break;
            case CirculationMode_Regularity:
                break;
            case CirculationMode_Python:
                break;
        }
    }

}
