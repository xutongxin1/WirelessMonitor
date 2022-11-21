//
// Created by xtx on 2022/11/18.
//

// You may need to build the project (run Qt uic code generator) to get "ui_DataCirculation.h" resolved

#include <QInputDialog>
#include "datacirculation.h"
#include "ui_DataCirculation.h"
#include <QDebug>
#include <QMessageBox>

DataCirculation::DataCirculation(QWidget *parent) :
        RepeaterWidget(parent), ui(new Ui::DataCirculation) {
    ui->setupUi(this);

    //绑定测试按钮
    connect(ui->btnTest, &QPushButton::clicked, this, [&] {
        TestCirculationMode();
    });
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

}

DataCirculation::~DataCirculation() {
    delete ui;
}

void DataCirculation::TestCirculationMode() {
    CirculationMode Mode = CirculationMode(ui->comCirculateMode->currentIndex());
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
        qDebug() <<"测试内容:"<<TestData<<" 模式:"<<Mode;
        switch (Mode) {
            case CirculationMode_Direction:
                if (TestData == "0") { ui->tableWidget->setItem(0, 0, new QTableWidgetItem(0)); }
                else {
                    if (TestData.toDouble() != 0) {
                        ui->tableWidget->setItem(0, 1, new QTableWidgetItem(
                                QString::number(TestData.toDouble())));
                    }
                    else
                    {
                        qCritical("%s 解析失败",qPrintable(TestData));
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
