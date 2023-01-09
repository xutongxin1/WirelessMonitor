//
// Created by xtx on 2022/9/16.
//

// You may need to build the project (run Qt uic code generator) to get "ui_TCPCom.h" resolved

#include "Tcpcom.h"
#include "ui_tcpcom.h"
#include "quihelper.h"
#include "quihelperdata.h"

#define _DEBUG 1

/*
 * TODO:shell语法高亮，使用正则表达式https://c.runoob.com_/front-end/
 * TODO:以回车分隔
 * TODO:ui修改
 */
TcpCom::TcpCom(int device_num, int win_num, QSettings *cfg, ToNewWidget *parent_info, QWidget *parent) :
    RepeaterWidget(parent), ui_(new Ui::Tcpcom) {
    this->cfg_ = cfg;
    this->config_file_path_ = "./config/Device" + QString::number(device_num) + ".ini";

    this->group_name_ = "Win" + QString::number(win_num);
    QuiHelper::InitAll();
    TcpCom::GetConstructConfig();

    AppData::ReadSendData();
    AppData::ReadDeviceData();

    ui_->setupUi(this);

    this->InitForm();
//    this->InitConfig();
    QuiHelper::SetFormInCenter(this);

    this->TCPCommandHandle = (*(parent_info->devices_info))[device_num].tcp_command_handler;//结构体这样用
    this->TCPInfoHandler[1] = (*(parent_info->devices_info))[device_num].tcp_info_handler[1];
    this->TCPInfoHandler[2] = (*(parent_info->devices_info))[device_num].tcp_info_handler[2];
    this->TCPInfoHandler[3] = (*(parent_info->devices_info))[device_num].tcp_info_handler[3];

    this->ip_ = TCPCommandHandle->IP;

    connect(TCPCommandHandle, &TCPCommandHandle::startInfoConnection, this, [&] {
//        disconnect(tcp_command_handle_, &tcp_command_handle_::startInfoConnection, 0, 0);

      //选项栏绑定
      if (TCPInfoHandler[2]->TCPMode == TCPInfoHandle::TCPInfoMode_OUT) {
          ui_->channelToSend->setItemData(1, -1, Qt::UserRole - 1);
      } else {
          ui_->channelToSend->setItemData(1, 0, Qt::UserRole - 1);
      }
      if (TCPInfoHandler[3]->TCPMode == TCPInfoHandle::TCPInfoMode_OUT) {
          ui_->channelToSend->setItemData(2, -1, Qt::UserRole - 1);
      } else {
          ui_->channelToSend->setItemData(2, 0, Qt::UserRole - 1);
      }

      //数据接收绑定
      if (TCPInfoHandler[1]->TCPMode == TCPInfoHandle::TCPInfoMode_IN) {
          connect(TCPInfoHandler[1], &TCPInfoHandle::RecNewData, this,
                  [&](const QByteArray &data, const QString &ip, int port, QTime time) {
                    this->GetData(data, port);
                  });
      }
      if (TCPInfoHandler[2]->TCPMode == TCPInfoHandle::TCPInfoMode_IN) {
          connect(TCPInfoHandler[2], &TCPInfoHandle::RecNewData, this,
                  [&](const QByteArray &data, const QString &ip, int port, QTime time) {
                    this->GetData(data, port);
                  });
      }
    });

    connect(ui_->btnSend, &QPushButton::clicked, this, [&] {
      this->SendData();
    });

    connect(ui_->btnPaste, &QPushButton::clicked, this, [&] {
      QClipboard *clipboard = QApplication::clipboard();
      QString text = clipboard->text();
      ui_->SendDataEdit->setPlainText(text);
      this->SendData();
    });

#if _DEBUG
    //测试按钮绑定
    ui_->btnStartTest->setHidden(false);
    connect(ui_->btnStartTest, &QPushButton::clicked, this, [&] {
      qDebug("工作在测试模式，发送通道的comboBox设置错误为正常现象");
      this->ip_ = "127.0.0.1";
      if (!TCPInfoHandler[1]->isConnected) {
          TCPInfoHandler[1]->connectToHost(ip_, 1921, QAbstractSocket::ReadWrite, QAbstractSocket::AnyIPProtocol);
      }
      if (!TCPInfoHandler[2]->isConnected) {
          TCPInfoHandler[2]->connectToHost(ip_, 1922, QAbstractSocket::ReadWrite, QAbstractSocket::AnyIPProtocol);
      }
      if (!TCPInfoHandler[3]->isConnected) {
          TCPInfoHandler[3]->connectToHost(ip_, 1923, QAbstractSocket::ReadWrite, QAbstractSocket::AnyIPProtocol);
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
                      this->GetData(data, port);
                    });
        }
        if (TCPInfoHandler[2]->TCPMode == TCPInfoHandle::TCPInfoMode_IN) {
            connect(TCPInfoHandler[2], &TCPInfoHandle::RecNewData, this,
                    [&](const QByteArray &data, const QString &ip, int port, QTime time) {
                      this->GetData(data, port);
                    });
        }
      });

    });
#else
    ui_->btnStartTest->setHidden(true);
#endif
}

TcpCom::~TcpCom() {
    delete ui_;
}
///初始化统计
void TcpCom::InitForm() {
    sleep_time_rec = 10;
    receive_count_ = 0;
    send_count_ = 0;
    is_show_ = true;

    ui_->tabWidget->setCurrentIndex(0);
    ChangeEnable(false);

#ifdef __arm__
    ui_->widgetRight->setFixedWidth(280);
#endif
}

void TcpCom::ChangeEnable(bool b) {

}

/// 往日志区添加数据
/// \param type 数据类型
/// \param data 数据
/// \param clear 是否清空
void TcpCom::Append(int type, const QString &data, bool clear) {
    static int currentCount = 0;
    static int maxCount = 81920;

    if (clear) {
        ui_->txtMain->clear();
        currentCount = 0;
        return;
    }

    if (currentCount >= maxCount) {
        ui_->txtMain->clear();
        currentCount = 0;
    }

    if (!is_show_) {
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
        ui_->txtMain->setTextColor(QColor("dodgerblue"));
    }
    else if (type == 1) {
        strType = "二通道接收 <<";
        ui_->txtMain->setTextColor(QColor("black"));
    }
    else if (type == 2) {
        strType = "二通道发送 >>";
        ui_->txtMain->setTextColor(QColor("gray"));
    }
    else if (type == 3) {
        strType = "三通道发送 >>";
        ui_->txtMain->setTextColor(QColor("green"));
    }
    else if (type == 4) {
        strType = "提示信息 >>";
        ui_->txtMain->setTextColor(QColor(100, 184, 255));
    }

    strData = QString("时间[%1] %2 %3").arg(TIMEMS, strType, strData);
    ui_->txtMain->append(strData);
    currentCount++;
}

/// 数据收入处理
/// \param data 数据
/// \param port 端口
void TcpCom::GetData(const QByteArray &data, int port) {

    QString buffer;
    if (ui_->ckHexReceive->isChecked()) {
        buffer = QUIHelperData::byteArrayToHexStr(data);
    } else {
        buffer = QString::fromLocal8Bit(data);
    }

    if (port == 1) {
        Append(0, buffer);
    } else if (port == 2) {
        Append(1, buffer);
    }

    receive_count_ = receive_count_ + data.size();
    ui_->btnReceiveCount->setText(QString("接收 : %1 字节").arg(receive_count_));
}

///发送发送栏里的数据
void TcpCom::SendData() {
    QString data = ui_->SendDataEdit->toPlainText();
    if (data.isEmpty()) {
        ui_->SendDataEdit->setFocus();
        return;
    }

    QByteArray buffer;
    if (ui_->ckHexSend->isChecked()) {
        buffer = QUIHelperData::hexStrToByteArray(data);
    } else {
        buffer = QUIHelperData::asciiStrToByteArray(data);
    }

    if (ui_->channelToSend->currentIndex() == 0) {
        if (TCPInfoHandler[2]->TCPMode == TCPInfoHandle::TCPInfoMode_OUT) {
            Append(2, data);
            TCPInfoHandler[2]->write(buffer);
        }
        if (TCPInfoHandler[3]->TCPMode == TCPInfoHandle::TCPInfoMode_OUT) {
            Append(3, data);
            TCPInfoHandler[3]->write(buffer);
        }
    } else if (ui_->channelToSend->currentIndex() == 1) {
        Append(2, data);
        TCPInfoHandler[2]->write(buffer);
    } else if (ui_->channelToSend->currentIndex() == 2) {
        Append(3, data);
        TCPInfoHandler[3]->write(buffer);
    }

    send_count_ = send_count_ + buffer.size();
    ui_->btnSendCount->setText(QString("发送 : %1 字节").arg(send_count_));
}

void TcpCom::SaveData() {
    QString tempData = ui_->txtMain->toPlainText();
    if (tempData.isEmpty()) {
        return;
    }

    QDateTime now = QDateTime::currentDateTime();
    QString name = now.toString("yyyy-MM-dd-HH-mm-ss");
    QString fileName = QString("%1/%2.txt").arg(QuiHelper::AppPath(), name);

    QFile file(fileName);
    file.open(QFile::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    out << tempData;
    file.close();

    on_btnClear_clicked();
}

void TcpCom::on_btnData_clicked() {
    QString fileName = QString("%1/%2").arg(QuiHelper::AppPath()).arg("send.txt");
    QFile file(fileName);
    if (!file.exists()) {
        return;
    }

    if (ui_->btnData->text() == "管理数据") {
        ui_->txtMain->setReadOnly(false);
        ui_->txtMain->clear();
        file.open(QFile::ReadOnly | QIODevice::Text);
        QTextStream in(&file);
        ui_->txtMain->setText(in.readAll());
        file.close();
        ui_->btnData->setText("保存数据");
    } else {
        ui_->txtMain->setReadOnly(true);
        file.open(QFile::WriteOnly | QIODevice::Text);
        QTextStream out(&file);
        out << ui_->txtMain->toPlainText();
        file.close();
        ui_->txtMain->clear();
        ui_->btnData->setText("管理数据");
        AppData::ReadSendData();
    }
}

void TcpCom::on_btnClear_clicked() {
    Append(0, "", true);
}

void TcpCom::GetConstructConfig() {
    cfg_->beginGroup(group_name_);

    cfg_->endGroup();

}

void TcpCom::SaveConstructConfig() {
    cfg_->beginGroup(group_name_);

    cfg_->endGroup();
}

void TcpCom::ReadErrorNet() {

}