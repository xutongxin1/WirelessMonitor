//
// Created by xtx on 2022/9/21.
//

// You may need to build the project (run Qt uic code generator) to get "ui_TCPBridgeConfiguration.h" resolved

#include "tcpbridgeconfiguration.h"

#include <QJsonObject>
#include <QLineEdit>
#include <QListView>
#include <QMessageBox>
#include <QCheckBox>

#include "ui_TCPBridgeConfiguration.h"

TCPBridgeConfiguration::TCPBridgeConfiguration(int device_num, int win_num, QSettings *cfg, ToNewWidget *parent_info,
                                               QWidget *parent)
    : RepeaterWidget(parent), ui_(new Ui::TCPBridgeConfiguration) {
    ui_->setupUi(this);

    this->cfg_ = cfg;
    this->group_name_ = "Win" + QString::number(win_num);
    TCPBridgeConfiguration::GetConstructConfig();
    this->parent_info_ = parent_info;
    this->device_num_ = device_num;
    this->tcp_command_handle_ = (*(parent_info->devices_info))[device_num].tcp_command_handler;  // 结构体这样用
    this->tcp_info_handler_[1] = (*(parent_info->devices_info))[device_num].tcp_info_handler[1];
    this->tcp_info_handler_[2] = (*(parent_info->devices_info))[device_num].tcp_info_handler[2];
    this->tcp_info_handler_[3] = (*(parent_info->devices_info))[device_num].tcp_info_handler[3];

    // 通道选择变更逻辑
    void (QComboBox::*fp)(int) = &QComboBox::currentIndexChanged;
    connect(ui_->enable12CheckBox, &QCheckBox::toggled, this, [&](int num) {
      RefreshBox();
    });

    connect(ui_->enable34CheckBox, &QCheckBox::toggled, this, [&](int num) {
      RefreshBox();
    });

//    ChangeMode();  // 初始化模式选择器

    // 初始化波特率表
    QStringList baud_list;
    baud_list << QString::number(TCPBridgeConfiguration::baud_rate_1_)
              << QString::number(TCPBridgeConfiguration::baud_rate_2_)
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

    ui_->BaudRate1->addItems(baud_list);
    ui_->BaudRate2->addItems(baud_list);
    //    connect(ui_->baud_rate_1_->lineEdit(), &QLineEdit::editingFinished, this, [=] {
    //        baud_rate_1_ = ui_->baud_rate_1_->lineEdit()->text().toInt();
    //        ChangeMode();
    //    });
    //
    //    connect(ui_->baud_rate_2_->lineEdit(), &QLineEdit::editingFinished, this, [=] {
    //        baud_rate_2_ = ui_->baud_rate_2_->lineEdit()->text().toInt();
    //        ChangeMode();
    //    });

    connect(ui_->BaudRate1, fp, this, [&](int num) {
      baud_rate_1_ = ui_->BaudRate1->currentText().toInt();
      RefreshBox();
    });
    connect(ui_->BaudRate2, fp, this, [&](int num) {
      baud_rate_2_ = ui_->BaudRate2->currentText().toInt();
      RefreshBox();
    });

    // 初始化数据位表
    QStringList data_bits_list;
    data_bits_list << "5"
                   << "6"
                   << "7"
                   << "8";

    ui_->DataBit1->addItems(data_bits_list);
    ui_->DataBit2->addItems(data_bits_list);
    connect(ui_->DataBit1, fp, this, [&](int num) {
      data_bit_1_ = ui_->DataBit1->currentText().toInt();
      RefreshBox();
    });
    connect(ui_->DataBit2, fp, this, [&](int num) {
      data_bit_2_ = ui_->DataBit2->currentText().toInt();
      RefreshBox();
    });

    // 初始化校验位表
    QStringList parity_list;
    parity_list << "无"
                << "奇"
                << "偶";
    ui_->Parity1->addItems(parity_list);
    ui_->Parity2->addItems(parity_list);
    connect(ui_->Parity1, fp, this, [&](int num) {
      parity_1_ = ui_->Parity1->currentIndex();
      RefreshBox();
    });
    connect(ui_->Parity2, fp, this, [&](int num) {
      parity_2_ = ui_->Parity2->currentIndex();
      RefreshBox();
    });

    // 初始化停止位表
    QStringList stop_bits_list;
    stop_bits_list << "1";
#ifdef Q_OS_WIN
    stop_bits_list << "1.5";
#endif
    stop_bits_list << "2";

    ui_->StopBit1->addItems(stop_bits_list);
    ui_->StopBit2->addItems(stop_bits_list);
    connect(ui_->StopBit1, fp, this, [&](int num) {
      stop_bit_1_ = ui_->StopBit1->currentText().toDouble();
      RefreshBox();
    });
    connect(ui_->StopBit2, fp, this, [&](int num) {
      stop_bit_2_ = ui_->StopBit2->currentText().toDouble();
      RefreshBox();
    });

    //    connect(ui_->)

    // 刷新选项
    RefreshBox();

    // 应用配置
    connect(ui_->save, &QPushButton::clicked, this, [&] { this->SetUart(); });
    connect(this, &RepeaterWidget::UseHistory, this, [&] { this->SetUart(); });
}
/**
 * @description: 获取配置文件
 * @return {*}
 */
void TCPBridgeConfiguration::GetConstructConfig() {
    cfg_->beginGroup(group_name_);
    TCPBridgeConfiguration::mode_1_ = IOMode(cfg_->value("mode_1_", TCPBridgeConfiguration::mode_1_).toInt());
    TCPBridgeConfiguration::mode_2_ = IOMode(cfg_->value("mode_2_", TCPBridgeConfiguration::mode_2_).toInt());
    TCPBridgeConfiguration::baud_rate_1_ = cfg_->value("baud_rate_1_", TCPBridgeConfiguration::baud_rate_1_).toInt();
    TCPBridgeConfiguration::data_bit_1_ = cfg_->value("data_bit_1_", TCPBridgeConfiguration::data_bit_1_).toInt();
    TCPBridgeConfiguration::parity_1_ = cfg_->value("parity_1_", TCPBridgeConfiguration::parity_1_).toInt();
    TCPBridgeConfiguration::stop_bit_1_ = cfg_->value("stop_bit_1_", TCPBridgeConfiguration::stop_bit_1_).toDouble();
    TCPBridgeConfiguration::baud_rate_2_ = cfg_->value("baud_rate_2_", TCPBridgeConfiguration::baud_rate_2_).toInt();
    TCPBridgeConfiguration::data_bit_2_ = cfg_->value("data_bit_2_", TCPBridgeConfiguration::data_bit_2_).toInt();
    TCPBridgeConfiguration::parity_2_ = cfg_->value("parity_2_", TCPBridgeConfiguration::parity_2_).toInt();
    TCPBridgeConfiguration::stop_bit_2_ = cfg_->value("stop_bit_2_", TCPBridgeConfiguration::stop_bit_2_).toDouble();
    cfg_->endGroup();

    if (mode_1_ != IO_MODE_CLOSED) { ui_->enable12CheckBox->setChecked(true); }
    if (mode_2_ != IO_MODE_CLOSED) { ui_->enable34CheckBox->setChecked(true); }

    if (mode_1_ != IO_MODE_TRANSMIT_ON) { ui_->transmit12->setChecked(true); }
    if (mode_2_ != IO_MODE_TRANSMIT_ON) { ui_->transmit34->setChecked(true); }
}

/**
 * @description: 保存配置文件
 * @return {*}
 */
void TCPBridgeConfiguration::SaveConstructConfig() {
    cfg_->beginGroup(group_name_);
    cfg_->setValue("mode_1_", TCPBridgeConfiguration::mode_1_);
    cfg_->setValue("mode_2_", TCPBridgeConfiguration::mode_2_);
    cfg_->setValue("baud_rate_1_", TCPBridgeConfiguration::baud_rate_1_);
    cfg_->setValue("data_bit_1_", TCPBridgeConfiguration::data_bit_1_);
    cfg_->setValue("parity_1_", TCPBridgeConfiguration::parity_1_);
    cfg_->setValue("stop_bit_1_", TCPBridgeConfiguration::stop_bit_1_);
    cfg_->setValue("baud_rate_2_", TCPBridgeConfiguration::baud_rate_2_);
    cfg_->setValue("data_bit_2_", TCPBridgeConfiguration::data_bit_2_);
    cfg_->setValue("parity_2_", TCPBridgeConfiguration::parity_2_);
    cfg_->setValue("stop_bit_2_", TCPBridgeConfiguration::stop_bit_2_);
    cfg_->endGroup();
}

TCPBridgeConfiguration::~TCPBridgeConfiguration() {
    delete ui_;
}

/**
 * @description: 刷新Ui通道选项
 * @return {*}
 */
void TCPBridgeConfiguration::RefreshBox() {
    bool tmp;
    if (ui_->enable12CheckBox->isChecked()) {
        tmp = true;
        if (ui_->transmit12->isChecked()) {
            mode_1_ = IO_MODE_TRANSMIT_ON;
        } else {
            mode_1_ = IO_MODE_ON;
        }
    } else {
        tmp = false;
        mode_1_ = IO_MODE_CLOSED;
    }
    ui_->BaudRate1->setEnabled(tmp);
    ui_->DataBit1->setEnabled(tmp);
    ui_->Parity1->setEnabled(tmp);
    ui_->StopBit1->setEnabled(tmp);
    ui_->transmit12->setEnabled(tmp);

    if (!tmp && !ui_->enable34CheckBox->isChecked()) {
        ui_->save->setEnabled(false);
    } else {
        ui_->save->setEnabled(true);
    }
    if (ui_->enable34CheckBox->isChecked()) {
        tmp = true;
        if (ui_->transmit12->isChecked()) {
            mode_2_ = IO_MODE_TRANSMIT_ON;
        } else {
            mode_2_ = IO_MODE_ON;
        }
    } else {
        tmp = false;
        mode_2_ = IO_MODE_CLOSED;
    }
    ui_->BaudRate2->setEnabled(tmp);
    ui_->DataBit2->setEnabled(tmp);
    ui_->Parity2->setEnabled(tmp);
    ui_->StopBit2->setEnabled(tmp);
    ui_->transmit34->setEnabled(tmp);

    ui_->BaudRate2->setCurrentIndex(ui_->BaudRate2->findText(QString::number(TCPBridgeConfiguration::baud_rate_2_)));
    ui_->StopBit1->setCurrentIndex(ui_->StopBit1->findText(QString::number(TCPBridgeConfiguration::stop_bit_1_)));
    ui_->StopBit2->setCurrentIndex(ui_->StopBit2->findText(QString::number(TCPBridgeConfiguration::stop_bit_2_)));
    ui_->Parity1->setCurrentIndex(parity_1_);
    ui_->Parity2->setCurrentIndex(parity_2_);
    ui_->DataBit1->setCurrentIndex(ui_->DataBit1->findText(QString::number(TCPBridgeConfiguration::data_bit_1_)));
    ui_->DataBit2->setCurrentIndex(ui_->DataBit2->findText(QString::number(TCPBridgeConfiguration::data_bit_2_)));

    SaveConstructConfig();
}

/**
 * @description: 应用设置
 * @return {*}
 */
void TCPBridgeConfiguration::SetUart() {
    //TODO: 当只有一个串口打开时仍然连接两个服务器?
    if (!tcp_command_handle_->GetConnectionState()) {
        qDebug() << "No connection found";
        return;
    }
    this->ip_ = tcp_command_handle_->ip_;

    ui_->save->setEnabled(false);
    ui_->save->setText("正在设置");
    StopAllInfoTCP();//关闭之前的信号通道
    // 构造配置文件
    QJsonObject u_1;
    if (mode_1_ == IO_MODE_CLOSED) {
        tcp_info_handler_[1]->ChangeTCPInfoMode(TCPInfoHandle::TCP_INFO_MODE_NONE);
        u_1.insert("mode", 0);
    } else {
        tcp_info_handler_[1]->ChangeTCPInfoMode(TCPInfoHandle::TCP_INFO_MODE_BOTH);
        u_1.insert("mode", int(mode_1_));

        u_1.insert("band", baud_rate_1_);
        u_1.insert("stop", stop_bit_1_);
        u_1.insert("parity", parity_1_);
        u_1.insert("data", data_bit_1_);
    }
    QJsonObject u_2;
    if (mode_2_ == IO_MODE_CLOSED) {
        u_2.insert("mode", 0);
        tcp_info_handler_[2]->ChangeTCPInfoMode(TCPInfoHandle::TCP_INFO_MODE_NONE);
    } else {
        tcp_info_handler_[2]->ChangeTCPInfoMode(TCPInfoHandle::TCP_INFO_MODE_BOTH);

        u_2.insert("mode", int(mode_2_));

        u_2.insert("band", baud_rate_2_);
        u_2.insert("stop", stop_bit_2_);
        u_2.insert("parity", parity_2_);
        u_2.insert("data", data_bit_2_);
    }
    QJsonObject attach;

    attach.insert("u1", u_1);
    attach.insert("u2", u_2);

    QJsonObject command;

    command.insert("command", 220);
    command.insert("attach", attach);

    // 连接信号服务器
    if (mode_1_ != IO_MODE_CLOSED && !tcp_info_handler_[1]->is_connected_) {
        tcp_info_handler_[1]->connectToHost(ip_, 1921, QAbstractSocket::ReadWrite, QAbstractSocket::AnyIPProtocol);
    }
    if (mode_2_ != IO_MODE_CLOSED && !tcp_info_handler_[2]->is_connected_) {
        tcp_info_handler_[2]->connectToHost(ip_, 1922, QAbstractSocket::ReadWrite, QAbstractSocket::AnyIPProtocol);
    }

    // 检查消息线路有没有正确连接，指令执行超时由SendCommand内处理，发出SendCommandError信号
    QTimer::singleShot(3000, this, [&] {
      disconnect(tcp_command_handle_, &TCPCommandHandle::SendCommandSuccess, nullptr, nullptr);
      disconnect(tcp_command_handle_, &TCPCommandHandle::SendCommandError, nullptr, nullptr);
      if ((!tcp_info_handler_[1]->is_connected_ && mode_1_ != IO_MODE_CLOSED)
          || (!tcp_info_handler_[2]->is_connected_ && mode_2_ != IO_MODE_CLOSED)) {
          StopAllInfoTCP();
          QMessageBox::critical(this, tr("错误"), tr("连接消息服务器失败"));
          ui_->save->setEnabled(true);
          ui_->save->setText("保存并应用");
          (*(parent_info_->devices_info))[device_num_].config_step = 2;
      } else if (get_command_reply_) {
          //解决收到OK后但INFO通道还没连上的时序问题
//          QMessageBox::information(this, tr("(*^▽^*)"), tr("设置串口完成，进入串口监视界面"), QMessageBox::Ok,
//                                   QMessageBox::Ok);
          emit(OrderShowSnackbar("(*^▽^*)\n设置串口完成，进入串口监视界面"));
          ui_->save->setEnabled(true);
          ui_->save->setText("再次保存并应用");
          (*(parent_info_->devices_info))[device_num_].config_step = 4;
          emit(OrderExchangeWindow(device_num_, 3));
          get_command_reply_ = false;
      }
    });

    connect(tcp_command_handle_, &TCPCommandHandle::SendCommandError, this, [&] {
      disconnect(tcp_command_handle_, &TCPCommandHandle::SendCommandSuccess, nullptr, nullptr);
      disconnect(tcp_command_handle_, &TCPCommandHandle::SendCommandError, nullptr, nullptr);
      StopAllInfoTCP();
      QMessageBox::critical(this, tr("错误"), tr("应用串口设置失败"));
      ui_->save->setEnabled(true);
      ui_->save->setText("保存并应用");
    });
    connect(tcp_command_handle_, &TCPCommandHandle::SendCommandSuccess, this, [&] {
      disconnect(tcp_command_handle_, &TCPCommandHandle::SendCommandSuccess, nullptr, nullptr);
      disconnect(tcp_command_handle_, &TCPCommandHandle::SendCommandError, nullptr, nullptr);
      get_command_reply_ = true;
      if ((tcp_info_handler_[1]->is_connected_ || mode_1_ == IO_MODE_CLOSED)
          && (tcp_info_handler_[2]->is_connected_ || mode_2_ == IO_MODE_CLOSED)) {
//          QMessageBox::information(this, tr("(*^▽^*)"), tr("设置串口完成，进入串口监视界面"), QMessageBox::Ok,
//                                   QMessageBox::Ok);

          ui_->save->setEnabled(true);
          ui_->save->setText("再次保存并应用");
          (*(parent_info_->devices_info))[device_num_].config_step = 4;
          emit(OrderExchangeWindow(device_num_, 3));
          get_command_reply_ = false;
      }
    });

    tcp_command_handle_->SendCommand(command, "OK!\r\n");//超时处理在这里

}