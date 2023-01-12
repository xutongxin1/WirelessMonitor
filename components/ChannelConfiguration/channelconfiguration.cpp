//
// Created by xtx on 2022/9/8.
//

// You may need to build the project (run Qt uic code generator) to get
// "ui_ChannelConfiguration.h" resolved

#include "channelconfiguration.h"

#include <QMessageBox>
#include <QToolTip>

#include "ui_ChannelConfiguration.h"

//TODO:多次连接时可能出现上个连接超时的bug(需要验证)
//TODO:设置模式失败后没有断开连接
ChannelConfiguration::ChannelConfiguration(int device_num, QSettings *main_cfg, QSettings *device_cfg,
                                           ToNewWidget *parent_info, RepeaterWidget *parent)
    : RepeaterWidget(parent), ui_(new Ui::ChannelConfiguration) {
    ui_->setupUi(this);

    this->cfg_ = device_cfg;
    this->main_cfg_ = main_cfg;

    ui_->ESPButton->setChecked(true);
    ui_->Disconnect->setEnabled(false);
    ui_->progressBar->hide();
    ui_->connectionTip->hide();
    this->device_num_ = device_num;
    this->parent_info_ = parent_info;
    this->device_group_ = "Device " + QString::number(device_num);
    this->tcp_command_handle_ = (*(parent_info->devices_info))[device_num].tcp_command_handler;  // 结构体这样用
    this->group_name_ = "Win1";
    ChannelConfiguration::GetConstructConfig();

    //    ui_->note->setText(MainCfg->GetMainCfg(cfgText + "note"));
    //
    //    ui_->ip_->setText(MainCfg->GetMainCfg(cfgText + "ip_"));

    connect(ui_->ESPButton, &QAbstractButton::toggled, this, [&] {
      ChannelConfiguration::SaveConstructConfig();
      ReflashUi(true);
    });
    connect(ui_->COMButton, &QAbstractButton::toggled, this, [&] {
      ChannelConfiguration::SaveConstructConfig();
      ReflashUi(false);
    });

    connect(ui_->note, &QLineEdit::editingFinished, this, [&] { ChannelConfiguration::SaveConstructConfig(); });

    connect(ui_->IP, &QLineEdit::editingFinished, this, [&] {
      ChannelConfiguration::SaveConstructConfig();

      // 恢复样式为正常
      ui_->IP->setStyleSheet("QLineEdit{border:1px solid gray border-radius:1px}");
    });

    connect(ui_->Connect, &QPushButton::clicked, this, &ChannelConfiguration::OnConnect);
    connect(ui_->Disconnect, &QPushButton::clicked, this, [&] {
      tcp_command_handle_->disconnectFromHost();
      ChannelConfiguration::OnDisconnect();
    });

    // IP地址正则表达式并应用
    QRegExp rx(
        R"((25[0-5]|2[0-4]\d|[0-1]\d{2}|[1-9]?\d)\.(25[0-5]|2[0-4]\d|[0-1]\d{2}|[1-9]?\d)\.(25[0-5]|2[0-4]\d|[0-1]\d{2}|[1-9]?\d)\.(25[0-5]|2[0-4]\d|[0-1]\d{2}|[1-9]?\d))");
    auto *p_reg = new QRegExpValidator(rx, this);
    ui_->IP->setValidator(p_reg);
}

ChannelConfiguration::~ChannelConfiguration() {
    delete ui_;
}

/**
 * @description: 获取配置文件
 * @return {*}
 */
void ChannelConfiguration::GetConstructConfig() {
    main_cfg_->beginGroup(device_group_);
    if (main_cfg_->value("WayToConnect") == 1) {
        ui_->ESPButton->setChecked(true);
    } else {
        ui_->COMButton->setChecked(true);
    }
    ui_->IP->setText(main_cfg_->value("ip_").toString());
    ui_->note->setText(main_cfg_->value("note").toString());
    main_cfg_->endGroup();

    cfg_->beginGroup(group_name_);
    ui_->FuncitonComboBox->setCurrentIndex(cfg_->value("mode").toInt());
    cfg_->endGroup();
}

/**
 * @description: 保存配置文件
 * @return {*}
 */
void ChannelConfiguration::SaveConstructConfig() {
    main_cfg_->beginGroup(device_group_);
    if (ui_->ESPButton->isChecked()) {
        main_cfg_->setValue("WayToConnect", 1);
    } else {
        main_cfg_->setValue("WayToConnect", 2);
    }
    main_cfg_->setValue("note", ui_->note->text());
    main_cfg_->setValue("ip_", ui_->IP->text());
    main_cfg_->endGroup();

    cfg_->beginGroup(group_name_);
    cfg_->setValue("mode", ui_->FuncitonComboBox->currentIndex());
    cfg_->endGroup();
}

/**
 * @description: 连接按钮按下事件
 * @return {*}
 */
void ChannelConfiguration::OnConnect() {
    if (!(ui_->IP->hasAcceptableInput())) {
        QToolTip::showText(ui_->IP->mapToGlobal(QPoint(100, 0)), "IP地址输入有误");
        // 设置LineEdit变为红色
        ui_->IP->setStyleSheet("QLineEdit{border:1px solid red }");
        return;
    }

    qDebug("开始启动连接");

    ui_->IP->setEnabled(false);
    ui_->FuncitonComboBox->setEnabled(false);
    ui_->ESPButton->setEnabled(false);
    ui_->COMButton->setEnabled(false);

    ui_->progressBar->show();
    ui_->progressBar->setValue(5);

    ui_->connectionTip->show();
    ui_->connectionTip->setText("正在连接到调试器");

    ip_ = ui_->IP->text();
    //    QStringList list = ui_->ip_->text().split(":");
    //    ip_ = list[0];
    //    port_ = list[1].toInt();

    // 断开可能的连接逻辑
    disconnect(tcp_command_handle_, &TCPCommandHandle::HasDisconnected, 0, 0);
    disconnect(tcp_command_handle_, &TCPCommandHandle::ReceiveFirstHeart, 0, 0);
    disconnect(tcp_command_handle_, &TCPCommandHandle::ReadyReboot, 0, 0);
    disconnect(tcp_command_handle_, &TCPCommandHandle::ModeChangeSuccess, 0, 0);
    disconnect(tcp_command_handle_, &TCPCommandHandle::HasDisconnected, 0, 0);
    disconnect(tcp_command_handle_, &TCPCommandHandle::HeartError, 0, 0);
    disconnect(tcp_command_handle_, &TCPCommandHandle::SetModeError, 0, 0);
    disconnect(tcp_command_handle_, &TCPCommandHandle::SendCommandSuccess, 0, 0);
    disconnect(tcp_command_handle_, &TCPCommandHandle::SendCommandError, 0, 0);

    // 是否已经终止连接
    has_give_up_ = false;

    // 启动连接流程
    connect(tcp_command_handle_, &TCPCommandHandle::HasDisconnected, this, [&] {
      //        disconnect(tcp_command_handle_, &tcp_command_handle_::HasDisconnected,
      //        0, 0);
      ChannelConfiguration::OnDisconnect();
      has_give_up_ = true;
    });
    // 心跳错误
    connect(tcp_command_handle_, &TCPCommandHandle::HeartError, this, [&] {
      has_give_up_ = true;
      QMessageBox::critical(this, tr("错误"), tr("心跳包返回失败"));
      ChannelConfiguration::OnDisconnect();
    });
    // 模式设置时错误
    connect(tcp_command_handle_, &TCPCommandHandle::SetModeError, this, [&] {
      has_give_up_ = true;
      QMessageBox::critical(this, tr("错误"), tr("设置模式失败"));
      ChannelConfiguration::OnDisconnect();
    });

    // 如果连接上服务器
    connect(tcp_command_handle_, &TCPCommandHandle::HasConnected, this, [&] {
      disconnect(tcp_command_handle_, &TCPCommandHandle::HasConnected, 0, 0);
      ui_->progressBar->setValue(25);
      ui_->connectionTip->setText("正在验证调试器状态");
      tcp_command_handle_->SendHeart();
      qInfo("等待第一个心跳包");
    });
    // 开始TCP连接
    tcp_command_handle_->connectToHost(ip_, 1920, QAbstractSocket::ReadWrite, QAbstractSocket::AnyIPProtocol);
    ui_->Connect->setEnabled(false);
    // 如果收到第一个心跳包
    connect(tcp_command_handle_, &TCPCommandHandle::ReceiveFirstHeart, this, [&] {
      // 第一次收到是检查模块状态
      disconnect(tcp_command_handle_, &TCPCommandHandle::ReceiveFirstHeart, 0, 0);
      ui_->progressBar->setValue(45);
      ui_->connectionTip->setText("正在设置调试器模式");

      connect(tcp_command_handle_, &TCPCommandHandle::ReceiveFirstHeart, this,
              [&] {  // 第二次收到，证明模式切换全部完成
                ui_->progressBar->setValue(100);
                ui_->connectionTip->setText("调试器模式设置完成，请进行下一步配置");
                disconnect(tcp_command_handle_, &TCPCommandHandle::ReceiveFirstHeart, 0, 0);
                (*(parent_info_->devices_info))[device_num_].config_step = 2;
                ui_->Disconnect->setEnabled(true);

              });
      tcp_command_handle_->SetMode(ui_->FuncitonComboBox->currentIndex());
    });
  // 设备重启
  connect(tcp_command_handle_, &TCPCommandHandle::ReadyReboot, this, [&] {
    ui_->progressBar->setValue(65);
    ui_->connectionTip->setText("设置完成，等待重启");
    qInfo("等待RF返回包");
    disconnect(tcp_command_handle_, &TCPCommandHandle::ReadyReboot, 0, 0);
  });
  // 模式设置成功
  connect(tcp_command_handle_, &TCPCommandHandle::ModeChangeSuccess, this, [&] {
    ui_->progressBar->setValue(85);
    ui_->connectionTip->setText("模式设置成功，检查模块状态");
    qInfo("等待重启后的心跳包应答");
    disconnect(tcp_command_handle_, &TCPCommandHandle::ModeChangeSuccess, 0, 0);
  });

  // 超时管理
  QTimer::singleShot(45000, this, [&] {
    if (ui_->progressBar->value() != 100 && !has_give_up_) {
        tcp_command_handle_->disconnectFromHost();
        QMessageBox::critical(this, tr("错误"), tr("连接流程超时"));
        //            ui_->connectionTip->setText(ui_->connectionTip->text() +
        //            "\n错误:操作超时");
        ChannelConfiguration::OnDisconnect();
        qCritical("连接超时");
    }
  });
}
// /**
//  * @description:
//  * @return {*}
//  */
// void ChannelConfiguration::SendModePackage() {
//     tcp_command_handle_->write("COM\r\n");
//     char *buffer;
//     connect(tcp_command_handle_, &QTcpSocket::readyRead, this, [=]() {
//         tcp_command_handle_->read(buffer, 1024);
//         if (strlen(buffer) == 6 && strncmp(buffer, " OK!\r\n", 6) == 0) {
//             connect_step_ = 2;
//             tcp_command_handle_->disconnect(SIGNAL(readyRead()));

//         }
//     });
// }

/**
 * @description: 断开按钮按下事件
 * @return {*}
 */
void ChannelConfiguration::OnDisconnect() {
    qDebug("关闭了连接");
    ui_->progressBar->setValue(0);
    ui_->connectionTip->setText("");
    ui_->IP->setEnabled(true);
    ui_->FuncitonComboBox->setEnabled(true);
    ui_->ESPButton->setEnabled(true);
    ui_->COMButton->setEnabled(true);
    ui_->Connect->setEnabled(true);
    (*(parent_info_->devices_info))[device_num_].config_step = 1;
}

/// Ui刷新
/// \param is_xmb isXMB是否是小卖部调试器模式
void ChannelConfiguration::ReflashUi(bool is_xmb) {
    ui_->label_3->setVisible(is_xmb);
    ui_->FuncitonComboBox->setVisible(is_xmb);
    ui_->label_2->setVisible(is_xmb);
    ui_->IP->setVisible(is_xmb);
    ui_->Connect->setVisible(is_xmb);
    ui_->Disconnect->setVisible(is_xmb);
}
