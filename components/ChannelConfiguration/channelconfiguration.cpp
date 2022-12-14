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
ChannelConfiguration::ChannelConfiguration(int DeviceNum, QSettings *MainCFG, QSettings *DeviceCFG,
                                           ToNewWidget *parentInfo, RepeaterWidget *parent)
    : RepeaterWidget(parent), ui(new Ui::ChannelConfiguration) {
  ui->setupUi(this);

  this->cfg = DeviceCFG;
  this->MainCFG = MainCFG;

  ui->ESPButton->setChecked(true);
  ui->Disconnect->setEnabled(false);
  ui->progressBar->hide();
  ui->connectionTip->hide();
  this->DeviceNum=DeviceNum;
  this->parentInfo = parentInfo;
  this->DeviceGroup = "Device " + QString::number(DeviceNum);
  this->TCPCommandHandle = (*(parentInfo->DevicesInfo))[DeviceNum].TCPCommandHandler;  // 结构体这样用
  this->GroupName = "Win1";
  ChannelConfiguration::GetConstructConfig();

  //    ui->note->setText(MainCfg->GetMainCfg(cfgText + "note"));
  //
  //    ui->IP->setText(MainCfg->GetMainCfg(cfgText + "IP"));

  connect(ui->ESPButton, &QAbstractButton::toggled, this, [&] {
    ChannelConfiguration::SaveConstructConfig();
    reflashUi(true);
  });
  connect(ui->COMButton, &QAbstractButton::toggled, this, [&] {
    ChannelConfiguration::SaveConstructConfig();
    reflashUi(false);
  });

  connect(ui->note, &QLineEdit::editingFinished, this, [&] { ChannelConfiguration::SaveConstructConfig(); });

  connect(ui->IP, &QLineEdit::editingFinished, this, [&] {
    ChannelConfiguration::SaveConstructConfig();

    // 恢复样式为正常
    ui->IP->setStyleSheet("QLineEdit{border:1px solid gray border-radius:1px}");
  });

  connect(ui->Connect, &QPushButton::clicked, this, &ChannelConfiguration::onConnect);
  connect(ui->Disconnect, &QPushButton::clicked, this, [&] {
    TCPCommandHandle->disconnectFromHost();
    ChannelConfiguration::onDisconnect();
  });

  // IP地址正则表达式并应用
  QRegExp rx(
      R"((25[0-5]|2[0-4]\d|[0-1]\d{2}|[1-9]?\d)\.(25[0-5]|2[0-4]\d|[0-1]\d{2}|[1-9]?\d)\.(25[0-5]|2[0-4]\d|[0-1]\d{2}|[1-9]?\d)\.(25[0-5]|2[0-4]\d|[0-1]\d{2}|[1-9]?\d))");
  auto *pReg = new QRegExpValidator(rx, this);
  ui->IP->setValidator(pReg);
}

ChannelConfiguration::~ChannelConfiguration() {
  delete ui;
}

/**
 * @description: 获取配置文件
 * @return {*}
 */
void ChannelConfiguration::GetConstructConfig() {
  MainCFG->beginGroup(DeviceGroup);
  if (MainCFG->value("WayToConnect") == 1) {
    ui->ESPButton->setChecked(true);
  } else {
    ui->COMButton->setChecked(true);
  }
  ui->IP->setText(MainCFG->value("IP").toString());
  ui->note->setText(MainCFG->value("note").toString());
  MainCFG->endGroup();

  cfg->beginGroup(GroupName);
  ui->FuncitonComboBox->setCurrentIndex(cfg->value("mode").toInt());
  cfg->endGroup();
}

/**
 * @description: 保存配置文件
 * @return {*}
 */
void ChannelConfiguration::SaveConstructConfig() {
  MainCFG->beginGroup(DeviceGroup);
  if (ui->ESPButton->isChecked()) {
    MainCFG->setValue("WayToConnect", 1);
  } else {
    MainCFG->setValue("WayToConnect", 2);
  }
  MainCFG->setValue("note", ui->note->text());
  MainCFG->setValue("IP", ui->IP->text());
  MainCFG->endGroup();

  cfg->beginGroup(GroupName);
  cfg->setValue("mode", ui->FuncitonComboBox->currentIndex());
  cfg->endGroup();
}

/**
 * @description: 连接按钮按下事件
 * @return {*}
 */
void ChannelConfiguration::onConnect() {
  if (!(ui->IP->hasAcceptableInput())) {
    QToolTip::showText(ui->IP->mapToGlobal(QPoint(100, 0)), "IP地址输入有误");
    // 设置LineEdit变为红色
    ui->IP->setStyleSheet("QLineEdit{border:1px solid red }");
    return;
  }

  qDebug("开始启动连接");

  ui->IP->setEnabled(false);
  ui->FuncitonComboBox->setEnabled(false);
  ui->ESPButton->setEnabled(false);
  ui->COMButton->setEnabled(false);

  ui->progressBar->show();
  ui->progressBar->setValue(5);

  ui->connectionTip->show();
  ui->connectionTip->setText("正在连接到调试器");

  ip = ui->IP->text();
  //    QStringList list = ui->IP->text().split(":");
  //    ip = list[0];
  //    port = list[1].toInt();

  // 断开可能的连接逻辑
  disconnect(TCPCommandHandle, &TCPCommandHandle::hasDisconnected, 0, 0);
  disconnect(TCPCommandHandle, &TCPCommandHandle::receiveFirstHeart, 0, 0);
  disconnect(TCPCommandHandle, &TCPCommandHandle::readyReboot, 0, 0);
  disconnect(TCPCommandHandle, &TCPCommandHandle::ModeChangeSuccess, 0, 0);
  disconnect(TCPCommandHandle, &TCPCommandHandle::hasDisconnected, 0, 0);
  disconnect(TCPCommandHandle, &TCPCommandHandle::heartError, 0, 0);
  disconnect(TCPCommandHandle, &TCPCommandHandle::setModeError, 0, 0);
  disconnect(TCPCommandHandle, &TCPCommandHandle::sendCommandSuccess, 0, 0);
  disconnect(TCPCommandHandle, &TCPCommandHandle::sendCommandError, 0, 0);

  // 是否已经终止连接
  hasGiveUp = false;

  // 启动连接流程
  connect(TCPCommandHandle, &TCPCommandHandle::hasDisconnected, this, [&] {
    //        disconnect(TCPCommandHandle, &TCPCommandHandle::hasDisconnected,
    //        0, 0);
    ChannelConfiguration::onDisconnect();
    hasGiveUp = true;
  });
  // 心跳错误
  connect(TCPCommandHandle, &TCPCommandHandle::heartError, this, [&] {
    hasGiveUp = true;
    QMessageBox::critical(this, tr("错误"), tr("心跳包返回失败"));
    ChannelConfiguration::onDisconnect();
  });
  // 模式设置时错误
  connect(TCPCommandHandle, &TCPCommandHandle::setModeError, this, [&] {
    hasGiveUp = true;
    QMessageBox::critical(this, tr("错误"), tr("设置模式失败"));
    ChannelConfiguration::onDisconnect();
  });

  // 如果连接上服务器
  connect(TCPCommandHandle, &TCPCommandHandle::hasConnected, this, [&] {
    disconnect(TCPCommandHandle, &TCPCommandHandle::hasConnected, 0, 0);
    ui->progressBar->setValue(25);
    ui->connectionTip->setText("正在验证调试器状态");
    TCPCommandHandle->SendHeart();
    qInfo("等待第一个心跳包");
  });
  // 开始TCP连接
  TCPCommandHandle->connectToHost(ip, 1920, QAbstractSocket::ReadWrite, QAbstractSocket::AnyIPProtocol);
  ui->Connect->setEnabled(false);
  // 如果收到第一个心跳包
  connect(TCPCommandHandle, &TCPCommandHandle::receiveFirstHeart, this, [&] {
    // 第一次收到是检查模块状态
    disconnect(TCPCommandHandle, &TCPCommandHandle::receiveFirstHeart, 0, 0);
    ui->progressBar->setValue(45);
    ui->connectionTip->setText("正在设置调试器模式");

    connect(TCPCommandHandle, &TCPCommandHandle::receiveFirstHeart, this,
            [&] {  // 第二次收到，证明模式切换全部完成
              ui->progressBar->setValue(100);
              ui->connectionTip->setText("调试器模式设置完成，请进行下一步配置");
              disconnect(TCPCommandHandle, &TCPCommandHandle::receiveFirstHeart, 0, 0);
              (*(parentInfo->DevicesInfo))[DeviceNum].configStep = 2;
              ui->Disconnect->setEnabled(true);

            });
    TCPCommandHandle->setMode(ui->FuncitonComboBox->currentIndex());
  });
  // 设备重启
  connect(TCPCommandHandle, &TCPCommandHandle::readyReboot, this, [&] {
    ui->progressBar->setValue(65);
    ui->connectionTip->setText("设置完成，等待重启");
    qInfo("等待RF返回包");
    disconnect(TCPCommandHandle, &TCPCommandHandle::readyReboot, 0, 0);
  });
  // 模式设置成功
  connect(TCPCommandHandle, &TCPCommandHandle::ModeChangeSuccess, this, [&] {
    ui->progressBar->setValue(85);
    ui->connectionTip->setText("模式设置成功，检查模块状态");
    qInfo("等待重启后的心跳包应答");
    disconnect(TCPCommandHandle, &TCPCommandHandle::ModeChangeSuccess, 0, 0);
  });

  // 超时管理
  QTimer::singleShot(45000, this, [&] {
    if (ui->progressBar->value() != 100 && !hasGiveUp) {
      TCPCommandHandle->disconnectFromHost();
      QMessageBox::critical(this, tr("错误"), tr("连接流程超时"));
      //            ui->connectionTip->setText(ui->connectionTip->text() +
      //            "\n错误:操作超时");
      ChannelConfiguration::onDisconnect();
      qCritical("连接超时");
    }
  });
}
// /**
//  * @description:
//  * @return {*}
//  */
// void ChannelConfiguration::SendModePackage() {
//     TCPCommandHandle->write("COM\r\n");
//     char *buffer;
//     connect(TCPCommandHandle, &QTcpSocket::readyRead, this, [=]() {
//         TCPCommandHandle->read(buffer, 1024);
//         if (strlen(buffer) == 6 && strncmp(buffer, " OK!\r\n", 6) == 0) {
//             ConnectStep = 2;
//             TCPCommandHandle->disconnect(SIGNAL(readyRead()));

//         }
//     });
// }

/**
 * @description: 断开按钮按下事件
 * @return {*}
 */
void ChannelConfiguration::onDisconnect() {
  qDebug("关闭了连接");
  ui->progressBar->setValue(0);
  ui->connectionTip->setText("");
  ui->IP->setEnabled(true);
  ui->FuncitonComboBox->setEnabled(true);
  ui->ESPButton->setEnabled(true);
  ui->COMButton->setEnabled(true);
  ui->Connect->setEnabled(true);
  (*(parentInfo->DevicesInfo))[DeviceNum].configStep = 1;
}

/// Ui刷新
/// \param isXMB isXMB是否是小卖部调试器模式
void ChannelConfiguration::reflashUi(bool isXMB) {
  ui->label_3->setVisible(isXMB);
  ui->FuncitonComboBox->setVisible(isXMB);
  ui->label_2->setVisible(isXMB);
  ui->IP->setVisible(isXMB);
  ui->Connect->setVisible(isXMB);
  ui->Disconnect->setVisible(isXMB);
}
