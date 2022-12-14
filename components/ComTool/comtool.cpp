//
// Created by xtx on 2022/9/16.
//

// You may need to build the project (run Qt uic code generator) to get "ui_ComTool.h" resolved

#include "comtool.h"
#include "ui_comtool.h"
#include "quihelper.h"
#include "quihelperdata.h"

/*
 * TODO:shell语法高亮，使用正则表达式https://c.runoob.com/front-end/
 * TODO:以回车分隔
 * TODO:ui修改
 */



QString ComTool::PortName = "COM1";
int ComTool::BaudRate = 9600;
int ComTool::DataBit = 8;
QString ComTool::Parity = QString::fromUtf8("无");
double ComTool::StopBit = 1;

bool ComTool::HexSend = false;
bool ComTool::HexReceive = false;
bool ComTool::Debug = false;
bool ComTool::AutoClear = false;

bool ComTool::AutoSend = false;
int ComTool::SendInterval = 1000;
bool ComTool::AutoSave = false;
int ComTool::SaveInterval = 5000;

QString ComTool::Mode = "Tcp_Client";
QString ComTool::ServerIP = "127.0.0.1";
int ComTool::ServerPort = 6000;
int ComTool::ListenPort = 6000;
int ComTool::SleepTime = 100;
bool ComTool::AutoConnect = false;

ComTool::ComTool(int DeviceNum, int winNum, QSettings *cfg, ToNewWidget *parentInfo, QWidget *parent) :
        RepeaterWidget(parent), ui(new Ui::comtool) {
    this->cfg = cfg;
    this->ConfigFilePath = "./config/Device" + QString::number(DeviceNum) + ".ini";

    this->GroupName = "Win" + QString::number(winNum);
    QUIHelper::initAll();
    ComTool::GetConstructConfig();

    AppData::Intervals << "1" << "10" << "20" << "50" << "100" << "200" << "300" << "500" << "1000" << "1500" << "2000"
                       << "3000" << "5000" << "10000";
    AppData::readSendData();
    AppData::readDeviceData();

    ui->setupUi(this);

    this->initForm();
    this->initConfig();
    QUIHelper::setFormInCenter(this);
}

ComTool::~ComTool() {
    delete ui;
}

void ComTool::initForm() {
    comOk = false;
    com = 0;
    sleepTime = 10;
    receiveCount = 0;
    sendCount = 0;
    isShow = true;

    ui->cboxSendInterval->addItems(AppData::Intervals);
    ui->cboxData->addItems(AppData::Datas);

    //读取数据
    timerRead = new QTimer(this);
    timerRead->setInterval(100);
    connect(timerRead, SIGNAL(timeout()), this, SLOT(readData()));

    //发送数据
    timerSend = new QTimer(this);
    connect(timerSend, SIGNAL(timeout()), this, SLOT(sendData()));
    connect(ui->btnSend, SIGNAL(clicked()), this, SLOT(sendData()));

    //保存数据
    timerSave = new QTimer(this);
    connect(timerSave, SIGNAL(timeout()), this, SLOT(saveData()));
    connect(ui->btnSave, SIGNAL(clicked()), this, SLOT(saveData()));

    ui->tabWidget->setCurrentIndex(0);
    changeEnable(false);

    tcpOk = false;
    socket = new QTcpSocket(this);
    socket->abort();
    connect(socket, SIGNAL(readyRead()), this, SLOT(readDataNet()));
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    connect(socket, SIGNAL(errorOccurred(QAbstractSocket::SocketError)), this, SLOT(readErrorNet()));
#else
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(readErrorNet()));
#endif

    timerConnect = new QTimer(this);
    connect(timerConnect, SIGNAL(timeout()), this, SLOT(connectNet()));
    timerConnect->setInterval(3000);
    timerConnect->start();

#ifdef __arm__
    ui->widgetRight->setFixedWidth(280);
#endif
}

void ComTool::initConfig() {
    QStringList comList;
    for (int i = 1; i <= 20; i++) {
        comList << QString("COM%1").arg(i);
    }

    comList << "ttyUSB0" << "ttyS0" << "ttyS1" << "ttyS2" << "ttyS3" << "ttyS4";
    comList << "ttymxc1" << "ttymxc2" << "ttymxc3" << "ttymxc4";
    comList << "ttySAC1" << "ttySAC2" << "ttySAC3" << "ttySAC4";
    ui->cboxPortName->addItems(comList);
    ui->cboxPortName->setCurrentIndex(ui->cboxPortName->findText(ComTool::PortName));
    connect(ui->cboxPortName, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    QStringList baudList;
    baudList << QString::number(ComTool::BaudRate) << "600" << "1200"
             << "1800" << "2400" << "4800" << "9600" << "14400" << "19200" << "38400"
             << "56000" << "57600" << "76800" << "115200" << "128000" << "256000";

    ui->cboxBaudRate->addItems(baudList);
    ui->cboxBaudRate->setCurrentIndex(ui->cboxBaudRate->findText(QString::number(ComTool::BaudRate)));
    connect(ui->cboxBaudRate, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));
//    void (QComboBox::*fp)(int) =&QComboBox::activated;
    connect(ui->cboxBaudRate->lineEdit(), &QLineEdit::editingFinished, this, [=] {

        ComTool::BaudRate = ui->cboxBaudRate->lineEdit()->text().toInt();
        saveConfig();


    });

    QStringList dataBitsList;
    dataBitsList << "5" << "6" << "7" << "8";

    ui->cboxDataBit->addItems(dataBitsList);
    ui->cboxDataBit->setCurrentIndex(ui->cboxDataBit->findText(QString::number(ComTool::DataBit)));
    connect(ui->cboxDataBit, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    QStringList parityList;
    parityList << "无" << "奇" << "偶";
#ifdef Q_OS_WIN
    parityList << "标志";
#endif
    parityList << "空格";

    ui->cboxParity->addItems(parityList);
    ui->cboxParity->setCurrentIndex(ui->cboxParity->findText(ComTool::Parity));
    connect(ui->cboxParity, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    QStringList stopBitsList;
    stopBitsList << "1";
#ifdef Q_OS_WIN
    stopBitsList << "1.5";
#endif
    stopBitsList << "2";

    ui->cboxStopBit->addItems(stopBitsList);
    ui->cboxStopBit->setCurrentIndex(ui->cboxStopBit->findText(QString::number(ComTool::StopBit)));
    connect(ui->cboxStopBit, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    ui->ckHexSend->setChecked(ComTool::HexSend);
    connect(ui->ckHexSend, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckHexReceive->setChecked(ComTool::HexReceive);
    connect(ui->ckHexReceive, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckDebug->setChecked(ComTool::Debug);
    connect(ui->ckDebug, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckAutoClear->setChecked(ComTool::AutoClear);
    connect(ui->ckAutoClear, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckAutoSend->setChecked(ComTool::AutoSend);
    connect(ui->ckAutoSend, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckAutoSave->setChecked(ComTool::AutoSave);
    connect(ui->ckAutoSave, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    QStringList sendInterval;
    QStringList saveInterval;
    sendInterval << "100" << "300" << "500";

    for (int i = 1000; i <= 10000; i = i + 1000) {
        sendInterval << QString::number(i);
        saveInterval << QString::number(i);
    }

    ui->cboxSendInterval->addItems(sendInterval);
    ui->cboxSaveInterval->addItems(saveInterval);

    ui->cboxSendInterval->setCurrentIndex(ui->cboxSendInterval->findText(QString::number(ComTool::SendInterval)));
    connect(ui->cboxSendInterval, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));
    ui->cboxSaveInterval->setCurrentIndex(ui->cboxSaveInterval->findText(QString::number(ComTool::SaveInterval)));
    connect(ui->cboxSaveInterval, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    timerSend->setInterval(ComTool::SendInterval);
    timerSave->setInterval(ComTool::SaveInterval);

    if (ComTool::AutoSend) {
        timerSend->start();
    }

    if (ComTool::AutoSave) {
        timerSave->start();
    }

    //串口转网络部分
    ui->cboxMode->setCurrentIndex(ui->cboxMode->findText(ComTool::Mode));
    connect(ui->cboxMode, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    ui->txtServerIP->setText(ComTool::ServerIP);
    connect(ui->txtServerIP, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    ui->txtServerPort->setText(QString::number(ComTool::ServerPort));
    connect(ui->txtServerPort, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    ui->txtListenPort->setText(QString::number(ComTool::ListenPort));
    connect(ui->txtListenPort, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    QStringList values;
    values << "0" << "10" << "50";

    for (int i = 100; i < 1000; i = i + 100) {
        values << QString("%1").arg(i);
    }

    ui->cboxSleepTime->addItems(values);

    ui->cboxSleepTime->setCurrentIndex(ui->cboxSleepTime->findText(QString::number(ComTool::SleepTime)));
    connect(ui->cboxSleepTime, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    ui->ckAutoConnect->setChecked(ComTool::AutoConnect);
    connect(ui->ckAutoConnect, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));
}

void ComTool::saveConfig() {
    ComTool::PortName = ui->cboxPortName->currentText();
    ComTool::BaudRate = ui->cboxBaudRate->currentText().toInt();
    ComTool::DataBit = ui->cboxDataBit->currentText().toInt();
    ComTool::Parity = ui->cboxParity->currentText();
    ComTool::StopBit = ui->cboxStopBit->currentText().toDouble();

    ComTool::HexSend = ui->ckHexSend->isChecked();
    ComTool::HexReceive = ui->ckHexReceive->isChecked();
    ComTool::Debug = ui->ckDebug->isChecked();
    ComTool::AutoClear = ui->ckAutoClear->isChecked();

    ComTool::AutoSend = ui->ckAutoSend->isChecked();
    ComTool::AutoSave = ui->ckAutoSave->isChecked();

    int sendInterval = ui->cboxSendInterval->currentText().toInt();
    if (sendInterval != ComTool::SendInterval) {
        ComTool::SendInterval = sendInterval;
        timerSend->setInterval(ComTool::SendInterval);
    }

    int saveInterval = ui->cboxSaveInterval->currentText().toInt();
    if (saveInterval != ComTool::SaveInterval) {
        ComTool::SaveInterval = saveInterval;
        timerSave->setInterval(ComTool::SaveInterval);
    }

//    ComTool::Mode = ui->cboxMode->currentText();
//    ComTool::ServerIP = ui->txtServerIP->text().trimmed();
//    ComTool::ServerPort = ui->txtServerPort->text().toInt();
//    ComTool::ListenPort = ui->txtListenPort->text().toInt();
//    ComTool::SleepTime = ui->cboxSleepTime->currentText().toInt();
//    ComTool::AutoConnect = ui->ckAutoConnect->isChecked();

    ComTool::SaveConstructConfig();
}

void ComTool::changeEnable(bool b) {
    ui->cboxBaudRate->setEnabled(!b);
    ui->cboxDataBit->setEnabled(!b);
    ui->cboxParity->setEnabled(!b);
    ui->cboxPortName->setEnabled(!b);
    ui->cboxStopBit->setEnabled(!b);
    ui->btnSend->setEnabled(b);
    ui->ckAutoSend->setEnabled(b);
    ui->ckAutoSave->setEnabled(b);
}

void ComTool::append(int type, const QString &data, bool clear) {
    static int currentCount = 0;
    static int maxCount = 100;

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
        strType = "串口发送 >>";
        ui->txtMain->setTextColor(QColor("dodgerblue"));
    }
    else if (type == 1) {
        strType = "串口接收 <<";
        ui->txtMain->setTextColor(QColor("black"));
    }
    else if (type == 2) {
        strType = "处理延时 >>";
        ui->txtMain->setTextColor(QColor("gray"));
    }
    else if (type == 3) {
        strType = "正在校验 >>";
        ui->txtMain->setTextColor(QColor("green"));
    }
    else if (type == 4) {
        strType = "网络发送 >>";
        ui->txtMain->setTextColor(QColor(24, 189, 155));
    }
    else if (type == 5) {
        strType = "网络接收 <<";
        ui->txtMain->setTextColor(QColor(255, 107, 107));
    }
    else if (type == 6) {
        strType = "提示信息 >>";
        ui->txtMain->setTextColor(QColor(100, 184, 255));
    }

    strData = QString("时间[%1] %2 %3").arg(TIMEMS).arg(strType).arg(strData);
    ui->txtMain->append(strData);
    currentCount++;
}

void ComTool::readData() {
    if (com->bytesAvailable() <= 0) {
        return;
    }

    QUIHelper::sleep(sleepTime);
    QByteArray data = com->readAll();
    int dataLen = data.length();
    if (dataLen <= 0) {
        return;
    }

    if (isShow) {
        QString buffer;
        if (ui->ckHexReceive->isChecked()) {
            buffer = QUIHelperData::byteArrayToHexStr(data);
        }
        else {
            //buffer = QUIHelperData::byteArrayToAsciiStr(data);
            buffer = QString::fromLocal8Bit(data);
        }

        //启用调试则模拟调试数据
        if (ui->ckDebug->isChecked()) {
            int count = AppData::Keys.count();
            for (int i = 0; i < count; i++) {
                if (buffer.startsWith(AppData::Keys.at(i))) {
                    sendData(AppData::Values.at(i));
                    break;
                }
            }
        }

        append(1, buffer);
        receiveCount = receiveCount + data.size();
        ui->btnReceiveCount->setText(QString("接收 : %1 字节").arg(receiveCount));

        //启用网络转发则调用网络发送数据
        if (tcpOk) {
            socket->write(data);
            append(4, QString(buffer));
        }
    }
}

void ComTool::sendData() {
    QString str = ui->cboxData->currentText();
    if (str.isEmpty()) {
        ui->cboxData->setFocus();
        return;
    }

    sendData(str);

    if (ui->ckAutoClear->isChecked()) {
        ui->cboxData->setCurrentIndex(-1);
        ui->cboxData->setFocus();
    }
}

void ComTool::sendData(QString data) {
    if (com == 0 || !com->isOpen()) {
        return;
    }

    //短信猫调试
    if (data.startsWith("AT")) {
        data += "\r";
    }

    QByteArray buffer;
    if (ui->ckHexSend->isChecked()) {
        buffer = QUIHelperData::hexStrToByteArray(data);
    }
    else {
        buffer = QUIHelperData::asciiStrToByteArray(data);
    }

    com->write(buffer);
    append(0, data);
    sendCount = sendCount + buffer.size();
    ui->btnSendCount->setText(QString("发送 : %1 字节").arg(sendCount));
}

void ComTool::saveData() {
    QString tempData = ui->txtMain->toPlainText();
    if (tempData.isEmpty()) {
        return;
    }

    QDateTime now = QDateTime::currentDateTime();
    QString name = now.toString("yyyy-MM-dd-HH-mm-ss");
    QString fileName = QString("%1/%2.txt").arg(QUIHelper::appPath()).arg(name);

    QFile file(fileName);
    file.open(QFile::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    out << tempData;
    file.close();

    on_btnClear_clicked();
}

void ComTool::on_btnOpen_clicked() {
    if (ui->btnOpen->text() == "打开串口") {
        com = new QSerialPort(ui->cboxPortName->currentText());
        comOk = com->open(QIODevice::ReadWrite);

        if (comOk) {
            //清空缓冲区
            com->flush();
            //设置波特率
            com->setBaudRate(ui->cboxBaudRate->currentText().toInt());
            //设置数据位
            com->setDataBits((QSerialPort::DataBits) ui->cboxDataBit->currentText().toInt());
            //设置校验位
            com->setParity((QSerialPort::Parity) ui->cboxParity->currentIndex());
            //设置停止位
            com->setStopBits((QSerialPort::StopBits) ui->cboxStopBit->currentIndex());
            com->setFlowControl(QSerialPort::NoFlowControl);

            changeEnable(true);
            ui->btnOpen->setText("关闭串口");
            timerRead->start();
        }
    }
    else {
        timerRead->stop();
        com->close();
        com->deleteLater();

        changeEnable(false);
        ui->btnOpen->setText("打开串口");
        on_btnClear_clicked();
        comOk = false;
    }
}

void ComTool::on_btnSendCount_clicked() {
    sendCount = 0;
    ui->btnSendCount->setText("发送 : 0 字节");
}

void ComTool::on_btnReceiveCount_clicked() {
    receiveCount = 0;
    ui->btnReceiveCount->setText("接收 : 0 字节");
}

void ComTool::on_btnStopShow_clicked() {
    if (ui->btnStopShow->text() == "停止显示") {
        isShow = false;
        ui->btnStopShow->setText("开始显示");
    }
    else {
        isShow = true;
        ui->btnStopShow->setText("停止显示");
    }
}

void ComTool::on_btnData_clicked() {
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

void ComTool::on_btnClear_clicked() {
    append(0, "", true);
}

void ComTool::on_btnStart_clicked() {
    if (ui->btnStart->text() == "启动") {
        if (ComTool::ServerIP == "" || ComTool::ServerPort == 0) {
            append(6, "IP地址和远程端口不能为空");
            return;
        }

        socket->connectToHost(ComTool::ServerIP, ComTool::ServerPort);
        if (socket->waitForConnected(100)) {
            ui->btnStart->setText("停止");
            append(6, "连接服务器成功");
            tcpOk = true;
        }
    }
    else {
        socket->disconnectFromHost();
        if (socket->state() == QAbstractSocket::UnconnectedState || socket->waitForDisconnected(100)) {
            ui->btnStart->setText("启动");
            append(6, "断开服务器成功");
            tcpOk = false;
        }
    }
}

void ComTool::on_ckAutoSend_stateChanged(int arg1) {
    if (arg1 == 0) {
        ui->cboxSendInterval->setEnabled(false);
        timerSend->stop();
    }
    else {
        ui->cboxSendInterval->setEnabled(true);
        timerSend->start();
    }
}

void ComTool::on_ckAutoSave_stateChanged(int arg1) {
    if (arg1 == 0) {
        ui->cboxSaveInterval->setEnabled(false);
        timerSave->stop();
    }
    else {
        ui->cboxSaveInterval->setEnabled(true);
        timerSave->start();
    }
}

void ComTool::connectNet() {
    if (!tcpOk && ComTool::AutoConnect && ui->btnStart->text() == "启动") {
        if (ComTool::ServerIP != "" && ComTool::ServerPort != 0) {
            socket->connectToHost(ComTool::ServerIP, ComTool::ServerPort);
            if (socket->waitForConnected(100)) {
                ui->btnStart->setText("停止");
                append(6, "连接服务器成功");
                tcpOk = true;
            }
        }
    }
}

void ComTool::readDataNet() {
    if (socket->bytesAvailable() > 0) {
        QUIHelper::sleep(ComTool::SleepTime);
        QByteArray data = socket->readAll();

        QString buffer;
        if (ui->ckHexReceive->isChecked()) {
            buffer = QUIHelperData::byteArrayToHexStr(data);
        }
        else {
            buffer = QUIHelperData::byteArrayToAsciiStr(data);
        }

        append(5, buffer);

        //将收到的网络数据转发给串口
        if (comOk) {
            com->write(data);
            append(0, buffer);
        }
    }
}

void ComTool::readErrorNet() {
    ui->btnStart->setText("启动");
    append(6, QString("连接服务器失败,%1").arg(socket->errorString()));
    socket->disconnectFromHost();
    tcpOk = false;
}

void ComTool::GetConstructConfig() {
    cfg->beginGroup(GroupName);
    ComTool::PortName = cfg->value("PortName", ComTool::PortName).toString();
    ComTool::BaudRate = cfg->value("BaudRate", ComTool::BaudRate).toInt();
    ComTool::DataBit = cfg->value("DataBit", ComTool::DataBit).toInt();
    ComTool::Parity = cfg->value("Parity", ComTool::Parity).toString();
    ComTool::StopBit = cfg->value("StopBit", ComTool::StopBit).toInt();

    ComTool::HexSend = cfg->value("HexSend", ComTool::HexSend).toBool();
    ComTool::HexReceive = cfg->value("HexReceive", ComTool::HexReceive).toBool();
    ComTool::Debug = cfg->value("Debug", ComTool::Debug).toBool();
    ComTool::AutoClear = cfg->value("AutoClear", ComTool::AutoClear).toBool();

    ComTool::AutoSend = cfg->value("AutoSend", ComTool::AutoSend).toBool();
    ComTool::SendInterval = cfg->value("SendInterval", ComTool::SendInterval).toInt();
    ComTool::AutoSave = cfg->value("AutoSave", ComTool::AutoSave).toBool();
    ComTool::SaveInterval = cfg->value("SaveInterval", ComTool::SaveInterval).toInt();
    cfg->endGroup();

    cfg->beginGroup("NetConfig");
    ComTool::Mode = cfg->value("Mode", ComTool::Mode).toString();
    ComTool::ServerIP = cfg->value("ServerIP", ComTool::ServerIP).toString();
    ComTool::ServerPort = cfg->value("ServerPort", ComTool::ServerPort).toInt();
    ComTool::ListenPort = cfg->value("ListenPort", ComTool::ListenPort).toInt();
    ComTool::SleepTime = cfg->value("SleepTime", ComTool::SleepTime).toInt();
    ComTool::AutoConnect = cfg->value("AutoConnect", ComTool::AutoConnect).toBool();
    cfg->endGroup();

    if (!QUIHelper::checkIniFile(ConfigFilePath)) {
        SaveConstructConfig();
        return;
    }
}

void ComTool::SaveConstructConfig() {
    cfg->beginGroup(GroupName);
    cfg->setValue("PortName", ComTool::PortName);
    cfg->setValue("BaudRate", ComTool::BaudRate);
    cfg->setValue("DataBit", ComTool::DataBit);
    cfg->setValue("Parity", ComTool::Parity);
    cfg->setValue("StopBit", ComTool::StopBit);

    cfg->setValue("HexSend", ComTool::HexSend);
    cfg->setValue("HexReceive", ComTool::HexReceive);
    cfg->setValue("Debug", ComTool::Debug);
    cfg->setValue("AutoClear", ComTool::AutoClear);

    cfg->setValue("AutoSend", ComTool::AutoSend);
    cfg->setValue("SendInterval", ComTool::SendInterval);
    cfg->setValue("AutoSave", ComTool::AutoSave);
    cfg->setValue("SaveInterval", ComTool::SaveInterval);
    cfg->endGroup();

    cfg->beginGroup("NetConfig");
    cfg->setValue("Mode", ComTool::Mode);
    cfg->setValue("ServerIP", ComTool::ServerIP);
    cfg->setValue("ServerPort", ComTool::ServerPort);
    cfg->setValue("ListenPort", ComTool::ListenPort);
    cfg->setValue("SleepTime", ComTool::SleepTime);
    cfg->setValue("AutoConnect", ComTool::AutoConnect);
    cfg->endGroup();
}
