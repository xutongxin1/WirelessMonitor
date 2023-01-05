//
// Created by xtx on 2022/9/16.
//

// You may need to build the project (run Qt uic code generator) to get "ui_TCPCom.h" resolved

#include "tcpcom.h"
#include "ui_tcpcom.h"
#include "quihelper.h"
#include "quihelperdata.h"

#define _DEBUG 1

/*
 * TODO:shell语法高亮，使用正则表达式https://c.runoob.com/front-end/
 * TODO:以回车分隔
 * TODO:ui修改
 */
TCPCom::TCPCom(int DeviceNum, int winNum, QSettings *cfg, ToNewWidget *parentInfo, QWidget *parent) :
        RepeaterWidget(parent), ui(new Ui::tcpcom) {
    this->cfg = cfg;
    this->ConfigFilePath = "./config/Device" + QString::number(DeviceNum) + ".ini";

    this->GroupName = "Win" + QString::number(winNum);
    QUIHelper::initAll();
    TCPCom::GetConstructConfig();

    AppData::readSendData();
    AppData::readDeviceData();

    ui->setupUi(this);

    this->initForm();
//    this->initConfig();
    QUIHelper::setFormInCenter(this);

    this->TCPCommandHandle = (*(parentInfo->DevicesInfo))[DeviceNum].TCPCommandHandler;//结构体这样用
    this->TCPInfoHandler[1] = (*(parentInfo->DevicesInfo))[DeviceNum].TCPInfoHandler[1];
    this->TCPInfoHandler[2] = (*(parentInfo->DevicesInfo))[DeviceNum].TCPInfoHandler[2];
    this->TCPInfoHandler[3] = (*(parentInfo->DevicesInfo))[DeviceNum].TCPInfoHandler[3];

    this->IP = TCPCommandHandle->IP;

    connect(TCPCommandHandle, &TCPCommandHandle::startInfoConnection, this, [&] {
//        disconnect(TCPCommandHandle, &TCPCommandHandle::startInfoConnection, 0, 0);

        //选项栏绑定
        if (TCPInfoHandler[2]->TCPMode == TCPInfoHandle::TCPInfoMode_OUT) {
            ui->channelToSend->setItemData(1, -1, Qt::UserRole - 1);
        }
        else {
            ui->channelToSend->setItemData(1, 0, Qt::UserRole - 1);
        }
        if (TCPInfoHandler[3]->TCPMode == TCPInfoHandle::TCPInfoMode_OUT) {
            ui->channelToSend->setItemData(2, -1, Qt::UserRole - 1);
        }
        else {
            ui->channelToSend->setItemData(2, 0, Qt::UserRole - 1);
        }

        //数据接收绑定
        if (TCPInfoHandler[1]->TCPMode == TCPInfoHandle::TCPInfoMode_IN) {
            connect(TCPInfoHandler[1], &TCPInfoHandle::RecNewData, this,
                    [&](const QByteArray &data, const QString &ip, int port, QTime time) {
                        this->getData(data, port);
                    });
        }
        if (TCPInfoHandler[2]->TCPMode == TCPInfoHandle::TCPInfoMode_IN) {
            connect(TCPInfoHandler[2], &TCPInfoHandle::RecNewData, this,
                    [&](const QByteArray &data, const QString &ip, int port, QTime time) {
                        this->getData(data, port);
                    });
        }
    });

    connect(ui->btnSend, &QPushButton::clicked, this, [&] {
        this->sendData();
    });

    connect(ui->btnPaste, &QPushButton::clicked, this, [&] {
        QClipboard *clipboard = QApplication::clipboard();
        QString text = clipboard->text();
        ui->SendDataEdit->setPlainText(text);
        this->sendData();
    });


#if _DEBUG
    //测试按钮绑定
    ui->btnStartTest->setHidden(false);
    connect(ui->btnStartTest, &QPushButton::clicked, this, [&] {
        qDebug("工作在测试模式，发送通道的comboBox设置错误为正常现象");
        this->IP = "127.0.0.1";
        if (!TCPInfoHandler[1]->isConnected) {
            TCPInfoHandler[1]->connectToHost(IP, 1921, QAbstractSocket::ReadWrite, QAbstractSocket::AnyIPProtocol);
        }
        if (!TCPInfoHandler[2]->isConnected) {
            TCPInfoHandler[2]->connectToHost(IP, 1922, QAbstractSocket::ReadWrite, QAbstractSocket::AnyIPProtocol);
        }
        if (!TCPInfoHandler[3]->isConnected) {
            TCPInfoHandler[3]->connectToHost(IP, 1923, QAbstractSocket::ReadWrite, QAbstractSocket::AnyIPProtocol);
        }

        //默认设置模式
        connect(TCPInfoHandler[3], &QTcpSocket::connected, this, [&] {
            TCPInfoHandler[1]->changeTCPInfoMode(TCPInfoHandle::TCPInfoMode_IN);
            TCPInfoHandler[2]->changeTCPInfoMode(TCPInfoHandle::TCPInfoMode_IN);
            TCPInfoHandler[3]->changeTCPInfoMode(TCPInfoHandle::TCPInfoMode_OUT);
            //数据接收绑定
            if (TCPInfoHandler[1]->TCPMode == TCPInfoHandle::TCPInfoMode_IN) {
                connect(TCPInfoHandler[1], &TCPInfoHandle::RecNewData, this,
                        [&](const QByteArray &data, const QString &ip, int port, QTime time) {
                            this->getData(data, port);
                        });
            }
            if (TCPInfoHandler[2]->TCPMode == TCPInfoHandle::TCPInfoMode_IN) {
                connect(TCPInfoHandler[2], &TCPInfoHandle::RecNewData, this,
                        [&](const QByteArray &data, const QString &ip, int port, QTime time) {
                            this->getData(data, port);
                        });
            }
        });


    });
#else
    ui->btnStartTest->setHidden(true);
#endif
}

TCPCom::~TCPCom() {
    delete ui;
}
///初始化统计
void TCPCom::initForm() {
    sleepTime = 10;
    receiveCount = 0;
    sendCount = 0;
    isShow = true;

    ui->tabWidget->setCurrentIndex(0);
    changeEnable(false);


#ifdef __arm__
    ui->widgetRight->setFixedWidth(280);
#endif
}

void TCPCom::changeEnable(bool b) {

}

/// 往日志区添加数据
/// \param type 数据类型
/// \param data 数据
/// \param clear 是否清空
void TCPCom::append(int type, const QString &data, bool clear) {
    static int currentCount = 0;
    static int maxCount = 81920;

    if (clear) {
        ui->txtMain->clear();
        currentCount = 0;
        return;
    }

    if (currentCount >= maxCount) {
        ui->txtMain->clear();
        currentCount = 0;
    }

    if (!isShow) {
        return;
    }

//    //过滤回车换行符
    QString strData = data;
//    strData = strData.replace("\r", "");
//    strData = strData.replace("\n", "");

    //不同类型不同颜色显示
    QString strType;
    if (type == 0) {
        strType = "一通道接收 <<";
        ui->txtMain->setTextColor(QColor("dodgerblue"));
    }
    else if (type == 1) {
        strType = "二通道接收 <<";
        ui->txtMain->setTextColor(QColor("black"));
    }
    else if (type == 2) {
        strType = "二通道发送 >>";
        ui->txtMain->setTextColor(QColor("gray"));
    }
    else if (type == 3) {
        strType = "三通道发送 >>";
        ui->txtMain->setTextColor(QColor("green"));
    }
    else if (type == 4) {
        strType = "提示信息 >>";
        ui->txtMain->setTextColor(QColor(100, 184, 255));
    }

    strData = QString("时间[%1] %2 %3").arg(TIMEMS, strType, strData);
    ui->txtMain->append(strData);
    currentCount++;
}

/// 数据收入处理
/// \param data 数据
/// \param port 端口
void TCPCom::getData(const QByteArray &data, int port) {

    QString buffer;
    if (ui->ckHexReceive->isChecked()) {
        buffer = QUIHelperData::byteArrayToHexStr(data);
    }
    else {
        buffer = QString::fromLocal8Bit(data);
    }

    if (port == 1) {
        append(0, buffer);
    }
    else if (port == 2) {
        append(1, buffer);
    }

    receiveCount = receiveCount + data.size();
    ui->btnReceiveCount->setText(QString("接收 : %1 字节").arg(receiveCount));
}

///发送发送栏里的数据
void TCPCom::sendData() {
    QString data = ui->SendDataEdit->toPlainText();
    if (data.isEmpty()) {
        ui->SendDataEdit->setFocus();
        return;
    }

    QByteArray buffer;
    if (ui->ckHexSend->isChecked()) {
        buffer = QUIHelperData::hexStrToByteArray(data);
    }
    else {
        buffer = QUIHelperData::asciiStrToByteArray(data);
    }

    if (ui->channelToSend->currentIndex() == 0) {
        if (TCPInfoHandler[2]->TCPMode == TCPInfoHandle::TCPInfoMode_OUT) {
            append(2, data);
            TCPInfoHandler[2]->write(buffer);
        }
        if (TCPInfoHandler[3]->TCPMode == TCPInfoHandle::TCPInfoMode_OUT) {
            append(3, data);
            TCPInfoHandler[3]->write(buffer);
        }
    }
    else if (ui->channelToSend->currentIndex() == 1) {
        append(2, data);
        TCPInfoHandler[2]->write(buffer);
    }
    else if (ui->channelToSend->currentIndex() == 2) {
        append(3, data);
        TCPInfoHandler[3]->write(buffer);
    }

    sendCount = sendCount + buffer.size();
    ui->btnSendCount->setText(QString("发送 : %1 字节").arg(sendCount));
}

void TCPCom::saveData() {
    QString tempData = ui->txtMain->toPlainText();
    if (tempData.isEmpty()) {
        return;
    }

    QDateTime now = QDateTime::currentDateTime();
    QString name = now.toString("yyyy-MM-dd-HH-mm-ss");
    QString fileName = QString("%1/%2.txt").arg(QUIHelper::appPath(), name);

    QFile file(fileName);
    file.open(QFile::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    out << tempData;
    file.close();

    on_btnClear_clicked();
}

void TCPCom::on_btnData_clicked() {
    QString fileName = QString("%1/%2").arg(QUIHelper::appPath()).arg("send.txt");
    QFile file(fileName);
    if (!file.exists()) {
        return;
    }

    if (ui->btnData->text() == "管理数据") {
        ui->txtMain->setReadOnly(false);
        ui->txtMain->clear();
        file.open(QFile::ReadOnly | QIODevice::Text);
        QTextStream in(&file);
        ui->txtMain->setText(in.readAll());
        file.close();
        ui->btnData->setText("保存数据");
    }
    else {
        ui->txtMain->setReadOnly(true);
        file.open(QFile::WriteOnly | QIODevice::Text);
        QTextStream out(&file);
        out << ui->txtMain->toPlainText();
        file.close();
        ui->txtMain->clear();
        ui->btnData->setText("管理数据");
        AppData::readSendData();
    }
}

void TCPCom::on_btnClear_clicked() {
    append(0, "", true);
}

void TCPCom::GetConstructConfig() {
    cfg->beginGroup(GroupName);

    cfg->endGroup();

}

void TCPCom::SaveConstructConfig() {
    cfg->beginGroup(GroupName);


    cfg->endGroup();
}


void TCPCom::readErrorNet() {

}