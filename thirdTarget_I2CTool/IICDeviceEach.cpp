//
// Created by xtx on 2023/11/27.
//

// You may need to build the project (run Qt uic code generator) to get "ui_IICDeviceEach.h" resolved

#include "IICDeviceEach.h"

#include <QDesktopServices>
#include <xlsxdocument.h>
#include <xlsxcellrange.h>
#include <xlsxcell.h>
#include <QScrollBar>
#include "CH339Control.h"
#include "ui_IICDeviceEach.h"

IICDeviceEach::IICDeviceEach(QWidget *parent) : RepeaterWidget(parent), ui_(new Ui::IICDeviceEach) {
    ui_->setupUi(this);

    ui_->LRSplitter->setStretchFactor(0, 1);
    ui_->LRSplitter->setStretchFactor(1, 2);

    //新建消息提示栏
    // sInfoBar = new FluShortInfoBar(FluShortInfoBarType::Info, window());
    // sInfoBar->setFixedWidth(270);

    python_work_ = new PythonWorkWithI2C();
    //test
    connect(ui_->ConfigAutoScan, &QPushButton::clicked, this, [&] {
        qDebug() << python_work_->ReciveDataFromI2C("3B", "FB92FFD01E3E");
        qDebug() << python_work_->ReciveDataFromI2C("43", "07DF0AC0F512");
        // sInfoBar->showInfoBarWithText("Hello");
    });

    EnumDevice();
    connect(ui_->ConnectCH, &QPushButton::clicked, this, &IICDeviceEach::OpenDevice);

    connect(ui_->ADDR_W, &QLineEdit::textChanged, this, [&](const QString &text) {
        //ui_->ADDR_W的值为转换为char存储
        ui_->ADDR_W->setText(text.toUpper());
        if (text.toUInt(nullptr, 16) > 15) {
            ADDR_W = text.toUInt(nullptr, 16);

            if (ui_->isRW1->isChecked()) {
                //读地址为写地址+1
                ADDR_R = ADDR_W + 1;
                ui_->ADDR_R->setText(QString::number(ADDR_W + 1, 16).toUpper());
            }
        }
    });
    connect(ui_->applyI2C, &QPushButton::clicked, this, &IICDeviceEach::CH347InitI2C);

    connect(ui_->ADDR_R, &QLineEdit::textChanged, this, [&](const QString &text) {
        ADDR_R = text.toUInt(nullptr, 16);
    });
    // connect(ui_->)

    ui_->tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded); //水平滚动条
    ui_->tableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded); //垂直滚动条
    Loadxlsx();
    AddRoute();
    // uint8_t tmp[100];
    // int t;
    // convertHexStringToUInt8Array("0xaB 0xfF 0x10",tmp,100,t);
    // qDebug()<<t;

    connect(ui_->openXlsx, &QPushButton::clicked, this, [&] {
        QDesktopServices::openUrl(QUrl::fromLocalFile(ui_->xlsxFile->text()));
    });
    connect(ui_->openPython, &QPushButton::clicked, this, [&] {
        QDesktopServices::openUrl(QUrl::fromLocalFile(ui_->pythonFile->text()));
    });
}

IICDeviceEach::~IICDeviceEach() {
    delete ui_;
}

void IICDeviceEach::onReadButtonClicked(int row, int col) {
    QString val = python_work_->PythonRunUserDef("Read_0x00");
    // sInfoBar->showInfoBarWithText(val);
}

void IICDeviceEach::onWriteButtonClicked(int row, int col) {
}
void IICDeviceEach::onActionButtonClicked(int row, int col) {
    if(row==4&&col==5) {
        uint8_t Data[2] = {0x08, 0x00};
        uint8_t rec[6] = {0};
        CH347ReadI2C(0x71, 0x00, rec, 1);

        CH347WriteI2C(0x70, 0xe1, Data, 2);
        QThread::msleep(500);
        CH347ReadI2C(0x71, 0x00, rec, 1);

        uint8_t Data2[2] = {0x33, 0x00};
        CH347WriteI2C(0x70, 0xac, Data2, 2);
        QThread::msleep(500);
        CH347ReadI2C(0x71, 0x00, rec, 5);

        uint8_t databuff[6];
        databuff[0] = rec[1];
        databuff[1] = rec[2];
        databuff[2] = rec[3];
        databuff[3] = rec[4];
        databuff[4] = 0x00;
        uint32_t SRH = (databuff[0] << 12) + (databuff[1] << 4) + (databuff[2] >> 4);
        uint32_t ST = ((databuff[2] & 0X0f) << 16) + (databuff[3] << 8) + (databuff[4]);
        float humidity = (int) (SRH * 100.0 / 1024 / 1024 + 0.5);
        float temperature = ((int) (ST * 2000.0 / 1024 / 1024 + 0.5)) / 10.0 - 50;
        qDebug() << humidity << temperature;

        QTableWidgetItem* oldItem = ui_->tableWidget->item(row - 1, col - 2);

        // 如果存在，先删除
        if (oldItem != nullptr) {
            oldItem->setText("湿度:" + QString::number(humidity) + "% 温度:" + QString::number(temperature) + "℃");
        }

        // auto *item = new QTableWidgetItem("湿度:" + QString::number(humidity) + "% 温度:" + QString::number(temperature) + "℃");
        // ui_->tableWidget->setItem(row - 1, col - 1, item);
    }
    if(row==3&&col==5) {
        auto* oldItem = ui_->tableWidget->item(row-1, col-2);

        // 如果存在，先删除
        if (oldItem != nullptr) {
            oldItem->setText("校准完成");
        }
        // auto *item = new QTableWidgetItem("校准完成");
        // ui_->tableWidget->setItem(row - 1, col - 1, item);
    }
}
void IICDeviceEach::Loadxlsx() {
    QXlsx::Document xlsx("C:/GitProject/WirelessMonitor/thirdTarget_I2CTool/config/AHT10/AHT10.xlsx");
    if (!xlsx.load()) {
        return;
    }

    int rowCount = xlsx.dimension().rowCount();
    int colCount = xlsx.dimension().columnCount();

    ui_->tableWidget->setRowCount(rowCount);
    ui_->tableWidget->setColumnCount(colCount);

    // 遍历所有单元格
    for (int row = 1; row <= rowCount; ++row) {
        for (int col = 1; col <= colCount; ++col) {
            QXlsx::Cell *cell = xlsx.cellAt(row, col);
            if (cell) {
                QString tmp = cell->value().toString();
                if (tmp == "读写") {
                    QWidget *cellWidget = new QWidget();
                    QVBoxLayout *layout = new QVBoxLayout(cellWidget);
                    layout->setContentsMargins(0, 0, 0, 0);

                    FluPushButton *readButton = new FluPushButton("读", cellWidget);
                    FluPushButton *writeButton = new FluPushButton("写", cellWidget);
                    layout->addWidget(readButton);
                    layout->addWidget(writeButton);
                    cellWidget->setLayout(layout);

                    // 绑定信号
                    connect(readButton, &QPushButton::clicked, this, [this, row, col]() {
                        onReadButtonClicked(row, col);
                    });
                    connect(writeButton, &QPushButton::clicked, this, [this, row, col]() {
                        onWriteButtonClicked(row, col);
                    });

                    ui_->tableWidget->setCellWidget(row - 1, col - 1, cellWidget);
                } else if (tmp == "读" || tmp == "执行") {
                    QWidget *cellWidget = new QWidget();
                    QVBoxLayout *layout = new QVBoxLayout(cellWidget);
                    layout->setContentsMargins(0, 0, 0, 0);
                    FluPushButton *readButton = new FluPushButton(tmp, ui_->tableWidget);
                    layout->addWidget(readButton);
                    cellWidget->setLayout(layout);

                    if (tmp == "读") {
                        // 绑定信号，将行列信息通过 lambda 传递给槽函数
                        connect(readButton, &QPushButton::clicked,
                                [this, row, col]() { onReadButtonClicked(row ,col); });
                    } else {
                        connect(readButton, &QPushButton::clicked,
                                [this, row, col]() { onActionButtonClicked(row, col); });
                    }

                    ui_->tableWidget->setCellWidget(row - 1, col - 1, cellWidget);
                } else {

                    //仅测试用
                    if(row==3&&col==4) {
                        tmp="true";
                    }


                    auto *item = new QTableWidgetItem(tmp);
                    item->setTextAlignment(Qt::AlignCenter);
                    if (cell->format().fontBold()) {
                        QFont font = item->font();
                        font.setBold(true);
                        item->setFont(font);
                    }

                    ui_->tableWidget->setItem(row - 1, col - 1, item);
                }
            }
        }
    }

    // 处理合并单元格
    QList<QXlsx::CellRange> merges = xlsx.currentWorksheet()->mergedCells();
    foreach(QXlsx::CellRange range, merges) {
        ui_->tableWidget->setSpan(range.firstRow() - 1, range.firstColumn() - 1,
                                  range.rowCount(), range.columnCount());
    }

    ui_->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // ui_->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    // ui_->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // ui_->tableWidget->verticalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui_->tableWidget->resizeColumnsToContents();
    ui_->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui_->tableWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel); //设置为像素移动
    QScrollBar *a = ui_->tableWidget->verticalScrollBar(); //获取到tablewidget的滚动条
    a->setSingleStep(5); //设置单步，值越小，下滑越慢
    // ui_->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
}
