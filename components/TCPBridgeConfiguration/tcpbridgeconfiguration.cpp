//
// Created by xtx on 2022/9/21.
//

// You may need to build the project (run Qt uic code generator) to get "ui_TCPBridgeConfiguration.h" resolved

#include "tcpbridgeconfiguration.h"

#include <QJsonObject>
#include <QLineEdit>
#include <QListView>
#include <QMessageBox>

#include "ui_TCPBridgeConfiguration.h"

TcpBridgeConfiguration::TcpBridgeConfiguration(int device_num, int win_num, QSettings *cfg, ToNewWidget *parent_info,
                                               QWidget *parent)
    : RepeaterWidget(parent), ui_(new Ui::TcpBridgeConfiguration) {
    ui_->setupUi(this);

    this->cfg = cfg;
    this->GroupName = "Win" + QString::number(win_num);
    TcpBridgeConfiguration::GetConstructConfig();
    this->parentInfo = parent_info;
    this->DeviceNum = device_num;
    this->tcp_command_handle_ = (*(parent_info->devices_info))[device_num].tcp_command_handler;  // 结构体这样用
    this->tcp_info_handler_[1] = (*(parent_info->devices_info))[device_num].tcp_info_handler[1];
    this->tcp_info_handler_[2] = (*(parent_info->devices_info))[device_num].tcp_info_handler[2];
    this->tcp_info_handler_[3] = (*(parent_info->devices_info))[device_num].tcp_info_handler[3];

    // 通道选择变更逻辑
    void (QComboBox::*fp)(int) = &QComboBox::currentIndexChanged;
    connect(ui_->mode1, fp, this, [&](int num) {
      switch (ui_->mode1->currentIndex()) {
          case 1:TcpBridgeConfiguration::mode_1_ = INPUT;
              break;
          case 2:TcpBridgeConfiguration::mode_1_ = SINGLE_INPUT;
              break;
          default:TcpBridgeConfiguration::mode_1_ = CLOSED;
      }
      ChangeMode();
    });

    connect(ui_->mode2, fp, this, [&](int num) {
      switch (ui_->mode2->currentIndex()) {
          case 1:TcpBridgeConfiguration::mode_2_ = FOLLOW_1_OUTPUT;
              break;
          case 2:TcpBridgeConfiguration::mode_2_ = FOLLOW_3_INPUT;
              break;
          default:TcpBridgeConfiguration::mode_2_ = CLOSED;
      }
      ChangeMode();
    });

    connect(ui_->mode3, fp, this, [&](int num) {
      switch (ui_->mode3->currentIndex()) {
          case 1:TcpBridgeConfiguration::mode_3_ = OUTPUT;
              break;
          case 2:TcpBridgeConfiguration::mode_3_ = SINGLE_OUTPUT;
              break;
          default:TcpBridgeConfiguration::mode_3_ = CLOSED;
      }
      ChangeMode();
    });

    ChangeMode();  // 初始化模式选择器

    // 初始化波特率表
    QStringList baudList;
    baudList << QString::number(TcpBridgeConfiguration::baud_rate_1_)
             << QString::number(TcpBridgeConfiguration::baud_rate_3_)
             << "600"
             << "1200"
             << "1800"
             << "2400"
             << "4800"
             << "9600"
             << "14400"
             << "19200"
             << "38400"
             << "56000"
             << "57600"
             << "76800"
             << "115200"
             << "128000"
             << "256000";

    ui_->BaudRate1->addItems(baudList);
    ui_->BaudRate3->addItems(baudList);
    //    connect(ui_->baud_rate_1_->lineEdit(), &QLineEdit::editingFinished, this, [=] {
    //        baud_rate_1_ = ui_->baud_rate_1_->lineEdit()->text().toInt();
    //        ChangeMode();
    //    });
    //
    //    connect(ui_->baud_rate_3_->lineEdit(), &QLineEdit::editingFinished, this, [=] {
    //        baud_rate_3_ = ui_->baud_rate_3_->lineEdit()->text().toInt();
    //        ChangeMode();
    //    });

    connect(ui_->BaudRate1, fp, this, [&](int num) {
      baud_rate_1_ = ui_->BaudRate1->currentText().toInt();
      ChangeMode();
    });
    connect(ui_->BaudRate3, fp, this, [&](int num) {
      baud_rate_3_ = ui_->BaudRate3->currentText().toInt();
      ChangeMode();
    });

    // 初始化数据位表
    QStringList dataBitsList;
    dataBitsList << "5"
                 << "6"
                 << "7"
                 << "8";

    ui_->DataBit1->addItems(dataBitsList);
    ui_->DataBit3->addItems(dataBitsList);
    connect(ui_->DataBit1, fp, this, [&](int num) {
      data_bit_1_ = ui_->DataBit1->currentText().toInt();
      ChangeMode();
    });
    connect(ui_->DataBit3, fp, this, [&](int num) {
      data_bit_3_ = ui_->DataBit3->currentText().toInt();
      ChangeMode();
    });

    // 初始化校验位表
    QStringList parityList;
    parityList << "无"
               << "奇"
               << "偶";
    ui_->Parity1->addItems(parityList);
    ui_->Parity3->addItems(parityList);
    connect(ui_->Parity1, fp, this, [&](int num) {
      parity_1_ = ui_->Parity1->currentIndex();
      ChangeMode();
    });
    connect(ui_->Parity3, fp, this, [&](int num) {
      parity_3_ = ui_->Parity3->currentIndex();
      ChangeMode();
    });

    // 初始化停止位表
    QStringList stopBitsList;
    stopBitsList << "1";
#ifdef Q_OS_WIN
    stopBitsList << "1.5";
#endif
    stopBitsList << "2";

    ui_->StopBit1->addItems(stopBitsList);
    ui_->StopBit3->addItems(stopBitsList);
    connect(ui_->StopBit1, fp, this, [&](int num) {
      stop_bit_1_ = ui_->StopBit1->currentText().toDouble();
      ChangeMode();
    });
    connect(ui_->StopBit3, fp, this, [&](int num) {
      stop_bit_3_ = ui_->StopBit3->currentText().toDouble();
      ChangeMode();
    });

    //    connect(ui_->)

    // 刷新选项
    RefreshBox();

    // 应用配置
    connect(ui_->save, &QPushButton::clicked, this, [&] { this->SetUart(); });
}
/**
 * @description: 获取配置文件
 * @return {*}
 */
void TcpBridgeConfiguration::GetConstructConfig() {
    cfg->beginGroup(GroupName);
    TcpBridgeConfiguration::mode_1_ = IoMode(cfg->value("mode_1_", TcpBridgeConfiguration::mode_1_).toInt());
    TcpBridgeConfiguration::mode_2_ = IoMode(cfg->value("mode_2_", TcpBridgeConfiguration::mode_2_).toInt());
    TcpBridgeConfiguration::mode_3_ = IoMode(cfg->value("mode_3_", TcpBridgeConfiguration::mode_3_).toInt());
    TcpBridgeConfiguration::baud_rate_1_ = cfg->value("baud_rate_1_", TcpBridgeConfiguration::baud_rate_1_).toInt();
    TcpBridgeConfiguration::data_bit_1_ = cfg->value("data_bit_1_", TcpBridgeConfiguration::data_bit_1_).toInt();
    TcpBridgeConfiguration::parity_1_ = cfg->value("parity_1_", TcpBridgeConfiguration::parity_1_).toInt();
    TcpBridgeConfiguration::stop_bit_1_ = cfg->value("stop_bit_1_", TcpBridgeConfiguration::stop_bit_1_).toDouble();
    TcpBridgeConfiguration::baud_rate_3_ = cfg->value("baud_rate_3_", TcpBridgeConfiguration::baud_rate_3_).toInt();
    TcpBridgeConfiguration::data_bit_3_ = cfg->value("data_bit_3_", TcpBridgeConfiguration::data_bit_3_).toInt();
    TcpBridgeConfiguration::parity_3_ = cfg->value("parity_3_", TcpBridgeConfiguration::parity_3_).toInt();
    TcpBridgeConfiguration::stop_bit_3_ = cfg->value("stop_bit_3_", TcpBridgeConfiguration::stop_bit_3_).toDouble();
    cfg->endGroup();
}

/**
 * @description: 保存配置文件
 * @return {*}
 */
void TcpBridgeConfiguration::SaveConstructConfig() {
    cfg->beginGroup(GroupName);
    cfg->setValue("mode_1_", TcpBridgeConfiguration::mode_1_);
    cfg->setValue("mode_2_", TcpBridgeConfiguration::mode_2_);
    cfg->setValue("mode_3_", TcpBridgeConfiguration::mode_3_);
    cfg->setValue("baud_rate_1_", TcpBridgeConfiguration::baud_rate_1_);
    cfg->setValue("data_bit_1_", TcpBridgeConfiguration::data_bit_1_);
    cfg->setValue("parity_1_", TcpBridgeConfiguration::parity_1_);
    cfg->setValue("stop_bit_1_", TcpBridgeConfiguration::stop_bit_1_);
    cfg->setValue("baud_rate_3_", TcpBridgeConfiguration::baud_rate_3_);
    cfg->setValue("data_bit_3_", TcpBridgeConfiguration::data_bit_3_);
    cfg->setValue("parity_3_", TcpBridgeConfiguration::parity_3_);
    cfg->setValue("stop_bit_3_", TcpBridgeConfiguration::stop_bit_3_);
    cfg->endGroup();
}

TcpBridgeConfiguration::~TcpBridgeConfiguration()
{
    delete ui_;
}

/**
 * @description: 切换通道模式的显示处理（不是应用）
 * @return {*}
 */
void TcpBridgeConfiguration::ChangeMode() {
    // 独占模式对中间选项的处理
    if (mode_1_ == SINGLE_INPUT || mode_3_ == SINGLE_OUTPUT) {
        ui_->mode2->setItemData(1, 0, Qt::UserRole - 1);
        ui_->mode2->setItemData(2, 0, Qt::UserRole - 1);
        mode_2_ = CLOSED;

        ui_->BaudRate2->setText("");
        ui_->DataBit2->setText("");
        ui_->Parity2->setText("");
        ui_->StopBit2->setText("");

        ui_->mode1->setItemData(1, 0, Qt::UserRole - 1);
        ui_->mode3->setItemData(1, 0, Qt::UserRole - 1);
        if (mode_1_ == INPUT) {
            mode_1_ = CLOSED;
        }
        if (mode_3_ == OUTPUT) {
            mode_3_ = CLOSED;
        }
    }
    else {
        ui_->mode2->setItemData(1, -1, Qt::UserRole - 1);
        ui_->mode2->setItemData(2, -1, Qt::UserRole - 1);
        ui_->mode1->setItemData(1, -1, Qt::UserRole - 1);
        ui_->mode3->setItemData(1, -1, Qt::UserRole - 1);
    }

    if (mode_1_ == SINGLE_INPUT && mode_3_ == SINGLE_OUTPUT)  // 转发模式逻辑
    {
        ui_->transmit->setEnabled(true);
    } else {
        ui_->transmit->setEnabled(false);
        ui_->transmit->setChecked(false);
    }

    // 当模式为None时禁用下面的选项
    bool m;
    m = mode_1_ != CLOSED;
    ui_->BaudRate1->setDisabled(true);
    ui_->Parity1->setEnabled(m);
    ui_->DataBit1->setEnabled(m);
    ui_->StopBit1->setEnabled(m);
    m = mode_2_ != CLOSED;
    ui_->BaudRate2->setEnabled(m);
    ui_->Parity2->setEnabled(m);
    ui_->DataBit2->setEnabled(m);
    ui_->StopBit2->setEnabled(m);
    m = mode_3_ != CLOSED;
    ui_->BaudRate3->setEnabled(m);
    ui_->Parity3->setEnabled(m);
    ui_->DataBit3->setEnabled(m);
    ui_->StopBit3->setEnabled(m);

    // 独占模式对相互的波特率选项影响
    if (mode_1_ == SINGLE_INPUT && mode_3_ == SINGLE_OUTPUT) {
        ui_->BaudRate3->setEnabled(false);
        ui_->Parity3->setEnabled(false);
        ui_->DataBit3->setEnabled(false);
        ui_->StopBit3->setEnabled(false);

        ui_->BaudRate1->setEnabled(true);
        ui_->Parity1->setEnabled(true);
        ui_->DataBit1->setEnabled(true);
        ui_->StopBit1->setEnabled(true);

        ui_->BaudRate3->setCurrentText(QString::number(baud_rate_1_));
        ui_->DataBit3->setCurrentText(QString::number(data_bit_1_));
        ui_->Parity3->setCurrentIndex(parity_3_);
        ui_->StopBit3->setCurrentText(QString::number(stop_bit_1_));
    } else if (mode_3_ == SINGLE_OUTPUT) {
        ui_->BaudRate3->setEnabled(true);
        ui_->Parity3->setEnabled(true);
        ui_->DataBit3->setEnabled(true);
        ui_->StopBit3->setEnabled(true);

        ui_->BaudRate1->setEnabled(false);
        ui_->Parity1->setEnabled(false);
        ui_->DataBit1->setEnabled(false);
        ui_->StopBit1->setEnabled(false);
    } else if (mode_1_ != CLOSED) {
        ui_->BaudRate1->setEnabled(true);
        ui_->Parity1->setEnabled(true);
        ui_->DataBit1->setEnabled(true);
        ui_->StopBit1->setEnabled(true);
    } else if (mode_3_ != CLOSED) {
        ui_->BaudRate3->setEnabled(true);
        ui_->Parity3->setEnabled(true);
        ui_->DataBit3->setEnabled(true);
        ui_->StopBit3->setEnabled(true);
    }

    // 中间模式的逻辑
    if (mode_1_ == INPUT) {
        ui_->mode2->setItemData(1, -1, Qt::UserRole - 1);
    } else {
        ui_->mode2->setItemData(1, 0, Qt::UserRole - 1);
        if (mode_2_ == FOLLOW_1_OUTPUT) {
            mode_2_ = CLOSED;
        }
    }
    if (mode_3_ == OUTPUT) {
        ui_->mode2->setItemData(2, -1, Qt::UserRole - 1);
    } else {
        ui_->mode2->setItemData(2, 0, Qt::UserRole - 1);
        if (mode_2_ == FOLLOW_3_INPUT) {
            mode_2_ = CLOSED;
        }
    }

    // 中间选项波特率的设计
    switch (mode_2_) {
        case CLOSED:ui_->BaudRate2->setText("");
            ui_->DataBit2->setText("");
            ui_->Parity2->setText("");
            ui_->StopBit2->setText("");
            break;
        case FOLLOW_1_OUTPUT:ui_->BaudRate2->setText(QString::number(baud_rate_1_));
            ui_->DataBit2->setText(QString::number(data_bit_1_));
            ui_->Parity2->setText(ui_->Parity1->currentText());
            ui_->StopBit2->setText(QString::number(stop_bit_1_));
            break;
        case FOLLOW_3_INPUT:ui_->BaudRate2->setText(QString::number(baud_rate_3_));
            ui_->DataBit2->setText(QString::number(data_bit_3_));
            ui_->Parity2->setText(ui_->Parity3->currentText());
            ui_->StopBit2->setText(QString::number(stop_bit_3_));
            break;
        default:break;
    }

    SaveConstructConfig();
    RefreshBox();
}
/**
 * @description: 刷新Ui通道选项
 * @return {*}
 */
void TcpBridgeConfiguration::RefreshBox() {
    switch (mode_1_) {
        case INPUT:ui_->mode1->setCurrentIndex(1);
            break;
        case SINGLE_INPUT:ui_->mode1->setCurrentIndex(2);
            break;
        default:ui_->mode1->setCurrentIndex(0);
    }
    switch (mode_2_) {
        case FOLLOW_1_OUTPUT:ui_->mode2->setCurrentIndex(1);
            break;
        case FOLLOW_3_INPUT:ui_->mode2->setCurrentIndex(2);
            break;
        default:ui_->mode2->setCurrentIndex(0);
    }
    switch (mode_3_) {
        case OUTPUT:ui_->mode3->setCurrentIndex(1);
            break;
        case SINGLE_OUTPUT:ui_->mode3->setCurrentIndex(2);
            break;
        default:ui_->mode3->setCurrentIndex(0);
    }
    ui_->BaudRate1->setCurrentIndex(ui_->BaudRate1->findText(QString::number(TcpBridgeConfiguration::baud_rate_1_)));
    ui_->BaudRate3->setCurrentIndex(ui_->BaudRate3->findText(QString::number(TcpBridgeConfiguration::baud_rate_3_)));
    ui_->StopBit1->setCurrentIndex(ui_->StopBit1->findText(QString::number(TcpBridgeConfiguration::stop_bit_1_)));
    ui_->StopBit3->setCurrentIndex(ui_->StopBit3->findText(QString::number(TcpBridgeConfiguration::stop_bit_3_)));
    ui_->Parity1->setCurrentIndex(parity_1_);
    ui_->Parity3->setCurrentIndex(parity_3_);
    ui_->DataBit1->setCurrentIndex(ui_->DataBit1->findText(QString::number(TcpBridgeConfiguration::data_bit_1_)));
    ui_->DataBit3->setCurrentIndex(ui_->DataBit3->findText(QString::number(TcpBridgeConfiguration::data_bit_3_)));
}

/**
 * @description: 应用设置
 * @return {*}
 */
void TcpBridgeConfiguration::SetUart() {
    if (!tcp_command_handle_->getConnectionState()) {
        qDebug() << "No connection found";
        return;
    }
    this->ip_ = tcp_command_handle_->IP;

    // 连接信号服务器
    if (!tcp_info_handler_[1]->isConnected) {
        tcp_info_handler_[1]->connectToHost(ip_, 1921, QAbstractSocket::ReadWrite, QAbstractSocket::AnyIPProtocol);
    }
    if (!tcp_info_handler_[2]->isConnected) {
        tcp_info_handler_[2]->connectToHost(ip_, 1922, QAbstractSocket::ReadWrite, QAbstractSocket::AnyIPProtocol);
    }
    if (!tcp_info_handler_[3]->isConnected) {
        tcp_info_handler_[3]->connectToHost(ip_, 1923, QAbstractSocket::ReadWrite, QAbstractSocket::AnyIPProtocol);
    }

    // 构造配置文件
    QJsonObject c1;
    if (mode_1_ == CLOSED) {
        tcp_info_handler_[1]->changeTCPInfoMode(TCPInfoHandle::TCPInfoMode_None);
        c1.insert("mode", 0);
    } else {
        tcp_info_handler_[1]->changeTCPInfoMode(TCPInfoHandle::TCPInfoMode_IN);
        if (mode_1_ == INPUT) {
            c1.insert("mode", 1);
        } else {
            c1.insert("mode", 3);
        }
        c1.insert("band", QString::number(baud_rate_1_));
        c1.insert("stop", stop_bit_1_);
        c1.insert("parity", parity_1_);
        c1.insert("data", data_bit_1_);
    }
    QJsonObject c2;
    if (mode_2_ == CLOSED) {
        c2.insert("mode", 0);
        tcp_info_handler_[2]->changeTCPInfoMode(TCPInfoHandle::TCPInfoMode_None);
    } else {
        if (mode_2_ == FOLLOW_1_OUTPUT) {
            c2.insert("mode", 5);
            tcp_info_handler_[2]->changeTCPInfoMode(TCPInfoHandle::TCPInfoMode_OUT);
        } else {
            c2.insert("mode", 6);
            tcp_info_handler_[2]->changeTCPInfoMode(TCPInfoHandle::TCPInfoMode_IN);
        }
    }
    QJsonObject c3;
    if (mode_3_ == CLOSED) {
        c3.insert("mode", 0);
        tcp_info_handler_[3]->changeTCPInfoMode(TCPInfoHandle::TCPInfoMode_None);
    } else {
        tcp_info_handler_[3]->changeTCPInfoMode(TCPInfoHandle::TCPInfoMode_OUT);
        if (mode_3_ == OUTPUT) {
            c3.insert("mode", 2);
        } else {
            c3.insert("mode", 4);
        }
        c3.insert("band", QString::number(baud_rate_3_));
        c3.insert("stop", stop_bit_3_);
        c3.insert("parity", parity_3_);
        c3.insert("data", data_bit_3_);
    }
    QJsonObject all;

    all.insert("c1", c1);
    all.insert("c2", c2);
    all.insert("c3", c3);

    ui_->save->setEnabled(false);
    ui_->save->setText("正在设置");

    connect(tcp_command_handle_, &TCPCommandHandle::sendCommandError, this, [=] {
      disconnect(tcp_command_handle_, &TCPCommandHandle::sendCommandSuccess, 0, 0);
      disconnect(tcp_command_handle_, &TCPCommandHandle::sendCommandError, 0, 0);
      QMessageBox::critical(this, tr("错误"), tr("设置串口失败"));
      ui_->save->setEnabled(true);
      ui_->save->setText("保存并应用");
    });
    connect(tcp_command_handle_, &TCPCommandHandle::sendCommandSuccess, this, [=] {
      disconnect(tcp_command_handle_, &TCPCommandHandle::sendCommandSuccess, 0, 0);
      disconnect(tcp_command_handle_, &TCPCommandHandle::sendCommandError, 0, 0);
      QMessageBox::information(this, tr("(*^▽^*)"), tr("设置串口完成，进入串口监视界面"), QMessageBox::Ok,
                               QMessageBox::Ok);
      ui_->save->setEnabled(true);
      ui_->save->setText("保存并应用");
      (*(parentInfo->devices_info))[DeviceNum].config_step = 4;
    });

    tcp_command_handle_->SendCommand(all, "OK!\r\n");

    // 超时设置
    QTimer::singleShot(1000, this, [&] {
      if (!(tcp_info_handler_[1]->isConnected && tcp_info_handler_[2]->isConnected
          && tcp_info_handler_[3]->isConnected)) {
          QMessageBox::critical(this, tr("错误"), tr("设置通信链路失败"));
          (*(parentInfo->devices_info))[DeviceNum].config_step = 2;
      }
    });
}
