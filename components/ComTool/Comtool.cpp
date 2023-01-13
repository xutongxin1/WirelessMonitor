//
// Created by xtx on 2022/9/16.
//

// You may need to build the project (run Qt uic code generator) to get "ui_ComTool.h" resolved

#include "Comtool.h"
#include "ui_comtool.h"
#include "quihelper.h"
#include "quihelperdata.h"

/*
 * TODO:shell语法高亮，使用正则表达式https://c.runoob.com_/front-end/
 * TODO:以回车分隔
 * TODO:ui修改
 */



QString ComTool::port_name_ = "COM1";
int ComTool::baud_rate_ = 9600;
int ComTool::data_bit_ = 8;
QString ComTool::parity_ = QString::fromUtf8("无");
double ComTool::stop_bit_ = 1;

bool ComTool::hex_send_ = false;
bool ComTool::hex_receive_ = false;
bool ComTool::debug_ = false;
bool ComTool::auto_clear_ = false;

bool ComTool::auto_send_ = false;
int ComTool::send_interval_ = 1000;
bool ComTool::auto_save_ = false;
int ComTool::save_interval_ = 5000;

QString ComTool::mode_ = "Tcp_Client";
QString ComTool::server_ip_ = "127.0.0.1";
int ComTool::server_port_ = 6000;
int ComTool::listen_port_ = 6000;
int ComTool::sleep_time_ = 100;
bool ComTool::auto_connect_ = false;

ComTool::ComTool(int device_num, int win_num, QSettings *cfg, ToNewWidget *parent_info, QWidget *parent) :
    RepeaterWidget(parent), ui_(new Ui::Comtool) {
  this->cfg_ = cfg;
  this->config_file_path_ = "./config/Device" + QString::number(device_num) + ".ini";

  this->group_name_ = "Win" + QString::number(win_num);
  QuiHelper::InitAll();
  ComTool::GetConstructConfig();

  AppData::intervals_ << "1" << "10" << "20" << "50" << "100" << "200" << "300" << "500" << "1000" << "1500" << "2000"
                      << "3000" << "5000" << "10000";
  AppData::ReadSendData();
  AppData::ReadDeviceData();

  ui_->setupUi(this);

  this->InitForm();
  this->InitConfig();
  QuiHelper::SetFormInCenter(this);
}

ComTool::~ComTool() {
  delete ui_;
}

void ComTool::InitForm() {
  com_ok_ = false;
  com_ = 0;
  sleep_time_rec_ = 10;
  receive_count_ = 0;
  send_count_ = 0;
  is_show_ = true;

  ui_->cboxSendInterval->addItems(AppData::intervals_);
  ui_->cboxData->addItems(AppData::datas_);

  //读取数据
  timer_read_ = new QTimer(this);
  timer_read_->setInterval(100);
  connect(timer_read_, SIGNAL(timeout()), this, SLOT(ReadData()));

  //发送数据
  timer_send_ = new QTimer(this);
  connect(timer_send_, SIGNAL(timeout()), this, SLOT(SendData()));
  connect(ui_->btnSend, SIGNAL(clicked()), this, SLOT(SendData()));

  //保存数据
  timer_save_ = new QTimer(this);
  connect(timer_save_, SIGNAL(timeout()), this, SLOT(SaveData()));
  connect(ui_->btnSave, SIGNAL(clicked()), this, SLOT(SaveData()));

  ui_->tabWidget->setCurrentIndex(0);
  ChangeEnable(false);

  tcp_ok_ = false;
  socket_ = new QTcpSocket(this);
  socket_->abort();
  connect(socket_, SIGNAL(readyRead()), this, SLOT(ReadDataNet()));
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
  connect(socket_, SIGNAL(errorOccurred(QAbstractSocket::SocketError)), this, SLOT(readErrorNet()));
#else
  connect(socket_, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(ReadErrorNet()));
#endif

  timer_connect_ = new QTimer(this);
  connect(timer_connect_, SIGNAL(timeout()), this, SLOT(ConnectNet()));
  timer_connect_->setInterval(3000);
  timer_connect_->start();

#ifdef __arm__
  ui_->widgetRight->setFixedWidth(280);
#endif
}

void ComTool::InitConfig() {
  QStringList comList;
  for (int i = 1; i <= 20; i++) {
    comList << QString("COM%1").arg(i);
  }

  comList << "ttyUSB0" << "ttyS0" << "ttyS1" << "ttyS2" << "ttyS3" << "ttyS4";
  comList << "ttymxc1" << "ttymxc2" << "ttymxc3" << "ttymxc4";
  comList << "ttySAC1" << "ttySAC2" << "ttySAC3" << "ttySAC4";
  ui_->cboxPortName->addItems(comList);
  ui_->cboxPortName->setCurrentIndex(ui_->cboxPortName->findText(ComTool::port_name_));
  connect(ui_->cboxPortName, SIGNAL(currentIndexChanged(int)), this, SLOT(SaveConfig()));

  QStringList baudList;
  baudList << QString::number(ComTool::baud_rate_) << "600" << "1200"
           << "1800" << "2400" << "4800" << "9600" << "14400" << "19200" << "38400"
           << "56000" << "57600" << "76800" << "115200" << "128000" << "256000";

  ui_->cboxBaudRate->addItems(baudList);
  ui_->cboxBaudRate->setCurrentIndex(ui_->cboxBaudRate->findText(QString::number(ComTool::baud_rate_)));
  connect(ui_->cboxBaudRate, SIGNAL(currentIndexChanged(int)), this, SLOT(SaveConfig()));
//    void (QComboBox::*fp)(int) =&QComboBox::activated;
  connect(ui_->cboxBaudRate->lineEdit(), &QLineEdit::editingFinished, this, [=] {

    ComTool::baud_rate_ = ui_->cboxBaudRate->lineEdit()->text().toInt();
    SaveConfig();

  });

  QStringList dataBitsList;
  dataBitsList << "5" << "6" << "7" << "8";

  ui_->cboxDataBit->addItems(dataBitsList);
  ui_->cboxDataBit->setCurrentIndex(ui_->cboxDataBit->findText(QString::number(ComTool::data_bit_)));
  connect(ui_->cboxDataBit, SIGNAL(currentIndexChanged(int)), this, SLOT(SaveConfig()));

  QStringList parityList;
  parityList << "无" << "奇" << "偶";
#ifdef Q_OS_WIN
  parityList << "标志";
#endif
  parityList << "空格";

  ui_->cboxParity->addItems(parityList);
  ui_->cboxParity->setCurrentIndex(ui_->cboxParity->findText(ComTool::parity_));
  connect(ui_->cboxParity, SIGNAL(currentIndexChanged(int)), this, SLOT(SaveConfig()));

  QStringList stopBitsList;
  stopBitsList << "1";
#ifdef Q_OS_WIN
  stopBitsList << "1.5";
#endif
  stopBitsList << "2";

  ui_->cboxStopBit->addItems(stopBitsList);
  ui_->cboxStopBit->setCurrentIndex(ui_->cboxStopBit->findText(QString::number(ComTool::stop_bit_)));
  connect(ui_->cboxStopBit, SIGNAL(currentIndexChanged(int)), this, SLOT(SaveConfig()));

  ui_->ckHexSend->setChecked(ComTool::hex_send_);
  connect(ui_->ckHexSend, SIGNAL(stateChanged(int)), this, SLOT(SaveConfig()));

  ui_->ckHexReceive->setChecked(ComTool::hex_receive_);
  connect(ui_->ckHexReceive, SIGNAL(stateChanged(int)), this, SLOT(SaveConfig()));

  ui_->ckDebug->setChecked(ComTool::debug_);
  connect(ui_->ckDebug, SIGNAL(stateChanged(int)), this, SLOT(SaveConfig()));

  ui_->ckAutoClear->setChecked(ComTool::auto_clear_);
  connect(ui_->ckAutoClear, SIGNAL(stateChanged(int)), this, SLOT(SaveConfig()));

  ui_->ckAutoSend->setChecked(ComTool::auto_send_);
  connect(ui_->ckAutoSend, SIGNAL(stateChanged(int)), this, SLOT(SaveConfig()));

  ui_->ckAutoSave->setChecked(ComTool::auto_save_);
  connect(ui_->ckAutoSave, SIGNAL(stateChanged(int)), this, SLOT(SaveConfig()));

  QStringList sendInterval;
  QStringList saveInterval;
  sendInterval << "100" << "300" << "500";

  for (int i = 1000; i <= 10000; i = i + 1000) {
    sendInterval << QString::number(i);
    saveInterval << QString::number(i);
  }

  ui_->cboxSendInterval->addItems(sendInterval);
  ui_->cboxSaveInterval->addItems(saveInterval);

  ui_->cboxSendInterval->setCurrentIndex(ui_->cboxSendInterval->findText(QString::number(ComTool::send_interval_)));
  connect(ui_->cboxSendInterval, SIGNAL(currentIndexChanged(int)), this, SLOT(SaveConfig()));
  ui_->cboxSaveInterval->setCurrentIndex(ui_->cboxSaveInterval->findText(QString::number(ComTool::save_interval_)));
  connect(ui_->cboxSaveInterval, SIGNAL(currentIndexChanged(int)), this, SLOT(SaveConfig()));

  timer_send_->setInterval(ComTool::send_interval_);
  timer_save_->setInterval(ComTool::save_interval_);

  if (ComTool::auto_send_) {
    timer_send_->start();
  }

  if (ComTool::auto_save_) {
    timer_save_->start();
  }

  //串口转网络部分
  ui_->cboxMode->setCurrentIndex(ui_->cboxMode->findText(ComTool::mode_));
  connect(ui_->cboxMode, SIGNAL(currentIndexChanged(int)), this, SLOT(SaveConfig()));

  ui_->txtServerIP->setText(ComTool::server_ip_);
  connect(ui_->txtServerIP, SIGNAL(textChanged(QString)), this, SLOT(SaveConfig()));

  ui_->txtServerPort->setText(QString::number(ComTool::server_port_));
  connect(ui_->txtServerPort, SIGNAL(textChanged(QString)), this, SLOT(SaveConfig()));

  ui_->txtListenPort->setText(QString::number(ComTool::listen_port_));
  connect(ui_->txtListenPort, SIGNAL(textChanged(QString)), this, SLOT(SaveConfig()));

  QStringList values;
  values << "0" << "10" << "50";

  for (int i = 100; i < 1000; i = i + 100) {
    values << QString("%1").arg(i);
  }

  ui_->cboxSleepTime->addItems(values);

  ui_->cboxSleepTime->setCurrentIndex(ui_->cboxSleepTime->findText(QString::number(ComTool::sleep_time_)));
  connect(ui_->cboxSleepTime, SIGNAL(currentIndexChanged(int)), this, SLOT(SaveConfig()));

  ui_->ckAutoConnect->setChecked(ComTool::auto_connect_);
  connect(ui_->ckAutoConnect, SIGNAL(stateChanged(int)), this, SLOT(SaveConfig()));
}

void ComTool::SaveConfig() {
  ComTool::port_name_ = ui_->cboxPortName->currentText();
  ComTool::baud_rate_ = ui_->cboxBaudRate->currentText().toInt();
  ComTool::data_bit_ = ui_->cboxDataBit->currentText().toInt();
  ComTool::parity_ = ui_->cboxParity->currentText();
  ComTool::stop_bit_ = ui_->cboxStopBit->currentText().toDouble();

  ComTool::hex_send_ = ui_->ckHexSend->isChecked();
  ComTool::hex_receive_ = ui_->ckHexReceive->isChecked();
  ComTool::debug_ = ui_->ckDebug->isChecked();
  ComTool::auto_clear_ = ui_->ckAutoClear->isChecked();

  ComTool::auto_send_ = ui_->ckAutoSend->isChecked();
  ComTool::auto_save_ = ui_->ckAutoSave->isChecked();

  int sendInterval = ui_->cboxSendInterval->currentText().toInt();
  if (sendInterval != ComTool::send_interval_) {
    ComTool::send_interval_ = sendInterval;
    timer_send_->setInterval(ComTool::send_interval_);
  }

  int saveInterval = ui_->cboxSaveInterval->currentText().toInt();
  if (saveInterval != ComTool::save_interval_) {
    ComTool::save_interval_ = saveInterval;
    timer_save_->setInterval(ComTool::save_interval_);
  }

//    ComTool::mode_ = ui_->cboxMode->currentText();
//    ComTool::server_ip_ = ui_->txtServerIP->text().trimmed();
//    ComTool::server_port_ = ui_->txtServerPort->text().toInt();
//    ComTool::listen_port_ = ui_->txtListenPort->text().toInt();
//    ComTool::sleep_time_ = ui_->cboxSleepTime->currentText().toInt();
//    ComTool::auto_connect_ = ui_->ckAutoConnect->isChecked();

  ComTool::SaveConstructConfig();
}

void ComTool::ChangeEnable(bool b) {
  ui_->cboxBaudRate->setEnabled(!b);
  ui_->cboxDataBit->setEnabled(!b);
  ui_->cboxParity->setEnabled(!b);
  ui_->cboxPortName->setEnabled(!b);
  ui_->cboxStopBit->setEnabled(!b);
  ui_->btnSend->setEnabled(b);
  ui_->ckAutoSend->setEnabled(b);
  ui_->ckAutoSave->setEnabled(b);
}

void ComTool::Append(int type, const QString &data, bool clear) {
  static int currentCount = 0;
  static int maxCount = 100;

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
    strType = "串口发送 >>";
    ui_->txtMain->setTextColor(QColor("dodgerblue"));
  } else if (type == 1) {
    strType = "串口接收 <<";
    ui_->txtMain->setTextColor(QColor("black"));
  } else if (type == 2) {
    strType = "处理延时 >>";
    ui_->txtMain->setTextColor(QColor("gray"));
  } else if (type == 3) {
    strType = "正在校验 >>";
    ui_->txtMain->setTextColor(QColor("green"));
  } else if (type == 4) {
    strType = "网络发送 >>";
    ui_->txtMain->setTextColor(QColor(24, 189, 155));
  } else if (type == 5) {
    strType = "网络接收 <<";
    ui_->txtMain->setTextColor(QColor(255, 107, 107));
  } else if (type == 6) {
    strType = "提示信息 >>";
    ui_->txtMain->setTextColor(QColor(100, 184, 255));
  }

  strData = QString("时间[%1] %2 %3").arg(TIMEMS).arg(strType).arg(strData);
  ui_->txtMain->append(strData);
  currentCount++;
}

void ComTool::ReadData() {
  if (com_->bytesAvailable() <= 0) {
    return;
  }

  QuiHelper::Sleep(sleep_time_rec_);
  QByteArray data = com_->readAll();
  int dataLen = data.length();
  if (dataLen <= 0) {
    return;
  }

  if (is_show_) {
    QString buffer;
    if (ui_->ckHexReceive->isChecked()) {
      buffer = QUIHelperData::byteArrayToHexStr(data);
    } else {
      //buffer = QUIHelperData::byteArrayToAsciiStr(data);
      buffer = QString::fromLocal8Bit(data);
    }

    //启用调试则模拟调试数据
    if (ui_->ckDebug->isChecked()) {
      int count = AppData::keys_.count();
      for (int i = 0; i < count; i++) {
        if (buffer.startsWith(AppData::keys_.at(i))) {
          SendData(AppData::values_.at(i));
          break;
        }
      }
    }

    Append(1, buffer);
    receive_count_ = receive_count_ + data.size();
    ui_->btnReceiveCount->setText(QString("接收 : %1 字节").arg(receive_count_));

    //启用网络转发则调用网络发送数据
    if (tcp_ok_) {
      socket_->write(data);
      Append(4, QString(buffer));
    }
  }
}

void ComTool::SendData() {
  QString str = ui_->cboxData->currentText();
  if (str.isEmpty()) {
    ui_->cboxData->setFocus();
    return;
  }

  SendData(str);

  if (ui_->ckAutoClear->isChecked()) {
    ui_->cboxData->setCurrentIndex(-1);
    ui_->cboxData->setFocus();
  }
}

void ComTool::SendData(QString data) {
  if (com_ == 0 || !com_->isOpen()) {
    return;
  }

  //短信猫调试
  if (data.startsWith("AT")) {
    data += "\r";
  }

  QByteArray buffer;
  if (ui_->ckHexSend->isChecked()) {
    buffer = QUIHelperData::hexStrToByteArray(data);
  } else {
    buffer = QUIHelperData::asciiStrToByteArray(data);
  }

  com_->write(buffer);
  Append(0, data);
  send_count_ = send_count_ + buffer.size();
  ui_->btnSendCount->setText(QString("发送 : %1 字节").arg(send_count_));
}

void ComTool::SaveData() {
  QString tempData = ui_->txtMain->toPlainText();
  if (tempData.isEmpty()) {
    return;
  }

  QDateTime now = QDateTime::currentDateTime();
  QString name = now.toString("yyyy-MM-dd-HH-mm-ss");
  QString fileName = QString("%1/%2.txt").arg(QuiHelper::AppPath()).arg(name);

  QFile file(fileName);
  file.open(QFile::WriteOnly | QIODevice::Text);
  QTextStream out(&file);
  out << tempData;
  file.close();

  on_btnClear_clicked();
}

void ComTool::on_btnOpen_clicked() {
  if (ui_->btnOpen->text() == "打开串口") {
    com_ = new QSerialPort(ui_->cboxPortName->currentText());
    com_ok_ = com_->open(QIODevice::ReadWrite);

    if (com_ok_) {
      //清空缓冲区
      com_->flush();
      //设置波特率
      com_->setBaudRate(ui_->cboxBaudRate->currentText().toInt());
      //设置数据位
      com_->setDataBits((QSerialPort::DataBits) ui_->cboxDataBit->currentText().toInt());
      //设置校验位
      com_->setParity((QSerialPort::Parity) ui_->cboxParity->currentIndex());
      //设置停止位
      com_->setStopBits((QSerialPort::StopBits) ui_->cboxStopBit->currentIndex());
      com_->setFlowControl(QSerialPort::NoFlowControl);

      ChangeEnable(true);
      ui_->btnOpen->setText("关闭串口");
      timer_read_->start();
    }
  }
    else {
    timer_read_->stop();
    com_->close();
    com_->deleteLater();

    ChangeEnable(false);
    ui_->btnOpen->setText("打开串口");
    on_btnClear_clicked();
    com_ok_ = false;
  }
}

void ComTool::on_btnSendCount_clicked() {
  send_count_ = 0;
  ui_->btnSendCount->setText("发送 : 0 字节");
}

void ComTool::on_btnReceiveCount_clicked() {
  receive_count_ = 0;
  ui_->btnReceiveCount->setText("接收 : 0 字节");
}

void ComTool::on_btnStopShow_clicked() {
  if (ui_->btnStopShow->text() == "停止显示") {
    is_show_ = false;
    ui_->btnStopShow->setText("开始显示");
  } else {
    is_show_ = true;
    ui_->btnStopShow->setText("停止显示");
  }
}

void ComTool::on_btnData_clicked() {
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

void ComTool::on_btnClear_clicked() {
  Append(0, "", true);
}

void ComTool::on_btnStart_clicked() {
  if (ui_->btnStart->text() == "启动") {
    if (ComTool::server_ip_ == "" || ComTool::server_port_ == 0) {
      Append(6, "IP地址和远程端口不能为空");
      return;
    }

    socket_->connectToHost(ComTool::server_ip_, ComTool::server_port_);
    if (socket_->waitForConnected(100)) {
      ui_->btnStart->setText("停止");
      Append(6, "连接服务器成功");
      tcp_ok_ = true;
    }
  } else {
    socket_->disconnectFromHost();
    if (socket_->state() == QAbstractSocket::UnconnectedState || socket_->waitForDisconnected(100)) {
      ui_->btnStart->setText("启动");
      Append(6, "断开服务器成功");
      tcp_ok_ = false;
    }
  }
}

void ComTool::on_ckAutoSend_stateChanged(int arg_1) {
  if (arg_1 == 0) {
    ui_->cboxSendInterval->setEnabled(false);
    timer_send_->stop();
  } else {
    ui_->cboxSendInterval->setEnabled(true);
    timer_send_->start();
  }
}

void ComTool::on_ckAutoSave_stateChanged(int arg_1) {
  if (arg_1 == 0) {
    ui_->cboxSaveInterval->setEnabled(false);
    timer_save_->stop();
  } else {
    ui_->cboxSaveInterval->setEnabled(true);
    timer_save_->start();
  }
}

void ComTool::ConnectNet() {
  if (!tcp_ok_ && ComTool::auto_connect_ && ui_->btnStart->text() == "启动") {
    if (ComTool::server_ip_ != "" && ComTool::server_port_ != 0) {
      socket_->connectToHost(ComTool::server_ip_, ComTool::server_port_);
      if (socket_->waitForConnected(100)) {
        ui_->btnStart->setText("停止");
        Append(6, "连接服务器成功");
        tcp_ok_ = true;
      }
    }
  }
}

void ComTool::ReadDataNet() {
  if (socket_->bytesAvailable() > 0) {
    QuiHelper::Sleep(ComTool::sleep_time_);
    QByteArray data = socket_->readAll();

    QString buffer;
    if (ui_->ckHexReceive->isChecked()) {
      buffer = QUIHelperData::byteArrayToHexStr(data);
    } else {
      buffer = QUIHelperData::byteArrayToAsciiStr(data);
    }

    Append(5, buffer);

    //将收到的网络数据转发给串口
    if (com_ok_) {
      com_->write(data);
      Append(0, buffer);
    }
  }
}

void ComTool::ReadErrorNet() {
  ui_->btnStart->setText("启动");
  Append(6, QString("连接服务器失败,%1").arg(socket_->errorString()));
  socket_->disconnectFromHost();
  tcp_ok_ = false;
}

void ComTool::GetConstructConfig() {
  cfg_->beginGroup(group_name_);
  ComTool::port_name_ = cfg_->value("port_name_", ComTool::port_name_).toString();
  ComTool::baud_rate_ = cfg_->value("baud_rate_", ComTool::baud_rate_).toInt();
  ComTool::data_bit_ = cfg_->value("data_bit_", ComTool::data_bit_).toInt();
  ComTool::parity_ = cfg_->value("parity_", ComTool::parity_).toString();
  ComTool::stop_bit_ = cfg_->value("stop_bit_", ComTool::stop_bit_).toInt();

  ComTool::hex_send_ = cfg_->value("hex_send_", ComTool::hex_send_).toBool();
  ComTool::hex_receive_ = cfg_->value("hex_receive_", ComTool::hex_receive_).toBool();
  ComTool::debug_ = cfg_->value("debug_", ComTool::debug_).toBool();
  ComTool::auto_clear_ = cfg_->value("auto_clear_", ComTool::auto_clear_).toBool();

  ComTool::auto_send_ = cfg_->value("auto_send_", ComTool::auto_send_).toBool();
  ComTool::send_interval_ = cfg_->value("send_interval_", ComTool::send_interval_).toInt();
  ComTool::auto_save_ = cfg_->value("auto_save_", ComTool::auto_save_).toBool();
  ComTool::save_interval_ = cfg_->value("save_interval_", ComTool::save_interval_).toInt();
  cfg_->endGroup();

  cfg_->beginGroup("NetConfig");
  ComTool::mode_ = cfg_->value("mode_", ComTool::mode_).toString();
  ComTool::server_ip_ = cfg_->value("server_ip_", ComTool::server_ip_).toString();
  ComTool::server_port_ = cfg_->value("server_port_", ComTool::server_port_).toInt();
  ComTool::listen_port_ = cfg_->value("listen_port_", ComTool::listen_port_).toInt();
  ComTool::sleep_time_ = cfg_->value("sleep_time_", ComTool::sleep_time_).toInt();
  ComTool::auto_connect_ = cfg_->value("auto_connect_", ComTool::auto_connect_).toBool();
  cfg_->endGroup();

  if (!QuiHelper::CheckIniFile(config_file_path_)) {
    SaveConstructConfig();
    return;
  }
}

void ComTool::SaveConstructConfig() {
  cfg_->beginGroup(group_name_);
  cfg_->setValue("port_name_", ComTool::port_name_);
  cfg_->setValue("baud_rate_", ComTool::baud_rate_);
  cfg_->setValue("data_bit_", ComTool::data_bit_);
  cfg_->setValue("parity_", ComTool::parity_);
  cfg_->setValue("stop_bit_", ComTool::stop_bit_);

  cfg_->setValue("hex_send_", ComTool::hex_send_);
  cfg_->setValue("hex_receive_", ComTool::hex_receive_);
  cfg_->setValue("debug_", ComTool::debug_);
  cfg_->setValue("auto_clear_", ComTool::auto_clear_);

  cfg_->setValue("auto_send_", ComTool::auto_send_);
  cfg_->setValue("send_interval_", ComTool::send_interval_);
  cfg_->setValue("auto_save_", ComTool::auto_save_);
  cfg_->setValue("save_interval_", ComTool::save_interval_);
  cfg_->endGroup();

  cfg_->beginGroup("NetConfig");
  cfg_->setValue("mode_", ComTool::mode_);
  cfg_->setValue("server_ip_", ComTool::server_ip_);
  cfg_->setValue("server_port_", ComTool::server_port_);
  cfg_->setValue("listen_port_", ComTool::listen_port_);
  cfg_->setValue("sleep_time_", ComTool::sleep_time_);
  cfg_->setValue("auto_connect_", ComTool::auto_connect_);
  cfg_->endGroup();
}
