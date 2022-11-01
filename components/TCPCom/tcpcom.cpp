//
// Created by xtx on 2022/9/16.
//

// You may need to build the project (run Qt uic code generator) to get "ui_TCPCom.h" resolved

#include "tcpcom.h"
#include "ui_tcpcom.h"
#include "quihelper.h"
#include "quihelperdata.h"

/*
 * TODO:shell语法高亮，使用正则表达式https://c.runoob.com/front-end/
 * TODO:以回车分隔
 * TODO:ui修改
 */



QString TCPCom::PortName = "COM1";
int TCPCom::BaudRate = 9600;
int TCPCom::DataBit = 8;
QString TCPCom::Parity = QString::fromUtf8("无");
double TCPCom::StopBit = 1;

bool TCPCom::HexSend = false;
bool TCPCom::HexReceive = false;
bool TCPCom::Debug = false;
bool TCPCom::AutoClear = false;

bool TCPCom::AutoSend = false;
int TCPCom::SendInterval = 1000;
bool TCPCom::AutoSave = false;
int TCPCom::SaveInterval = 5000;

QString TCPCom::Mode = "Tcp_Client";
QString TCPCom::ServerIP = "127.0.0.1";
int TCPCom::ServerPort = 6000;
int TCPCom::ListenPort = 6000;
int TCPCom::SleepTime = 100;
bool TCPCom::AutoConnect = false;

TCPCom::TCPCom(int DeviceNum, int winNum, QSettings *cfg, ToNewWidget *parentInfo, QWidget *parent) :
        RepeaterWidget(parent), ui(new Ui::tcpcom) {
    this->cfg = cfg;
    this->ConfigFilePath = "./config/Device" + QString::number(DeviceNum) + ".ini";

    this->GroupName = "Win" + QString::number(winNum);
    QUIHelper::initAll();
    TCPCom::GetConstructConfig();

//    AppData::Intervals << "1" << "10" << "20" << "50" << "100" << "200" << "300" << "500" << "1000" << "1500" << "2000"
//                       << "3000" << "5000" << "10000";
    AppData::readSendData();
    AppData::readDeviceData();

    ui->setupUi(this);

    this->initForm();
    this->initConfig();
    QUIHelper::setFormInCenter(this);
}

TCPCom::~TCPCom() {
    delete ui;
}

void TCPCom::initForm() {
    comOk = false;
    com = 0;
    sleepTime = 10;
    receiveCount = 0;
    sendCount = 0;
    isShow = true;

//    ui->cboxSendInterval->addItems(AppData::Intervals);
//    ui->SendDataEdit->addItems(AppData::Datas);

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

void TCPCom::initConfig() {
//    QStringList comList;
//    for (int i = 1; i <= 20; i++) {
//        comList << QString("COM%1").arg(i);
//    }
//
//    comList << "ttyUSB0" << "ttyS0" << "ttyS1" << "ttyS2" << "ttyS3" << "ttyS4";
//    comList << "ttymxc1" << "ttymxc2" << "ttymxc3" << "ttymxc4";
//    comList << "ttySAC1" << "ttySAC2" << "ttySAC3" << "ttySAC4";
//    ui->cboxPortName->addItems(comList);
//    ui->cboxPortName->setCurrentIndex(ui->cboxPortName->findText(TCPCom::PortName));
//    connect(ui->cboxPortName, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

//    QStringList baudList;
//    baudList << QString::number(TCPCom::BaudRate) << "600" << "1200"
//             << "1800" << "2400" << "4800" << "9600" << "14400" << "19200" << "38400"
//             << "56000" << "57600" << "76800" << "115200" << "128000" << "256000";
//
//    ui->cboxBaudRate->addItems(baudList);
//    ui->cboxBaudRate->setCurrentIndex(ui->cboxBaudRate->findText(QString::number(TCPCom::BaudRate)));
//    connect(ui->cboxBaudRate, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));
////    void (QComboBox::*fp)(int) =&QComboBox::activated;
//    connect(ui->cboxBaudRate->lineEdit(), &QLineEdit::editingFinished, this, [=] {
//
//        TCPCom::BaudRate = ui->cboxBaudRate->lineEdit()->text().toInt();
//        saveConfig();
//
//
//    });

//    QStringList dataBitsList;
//    dataBitsList << "5" << "6" << "7" << "8";
//
//    ui->cboxDataBit->addItems(dataBitsList);
//    ui->cboxDataBit->setCurrentIndex(ui->cboxDataBit->findText(QString::number(TCPCom::DataBit)));
//    connect(ui->cboxDataBit, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

//    QStringList parityList;
//    parityList << "无" << "奇" << "偶";
//#ifdef Q_OS_WIN
//    parityList << "标志";
//#endif
//    parityList << "空格";
//
//    ui->cboxParity->addItems(parityList);
//    ui->cboxParity->setCurrentIndex(ui->cboxParity->findText(TCPCom::Parity));
//    connect(ui->cboxParity, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));
//
//    QStringList stopBitsList;
//    stopBitsList << "1";
//#ifdef Q_OS_WIN
//    stopBitsList << "1.5";
//#endif
//    stopBitsList << "2";
//
//    ui->cboxStopBit->addItems(stopBitsList);
//    ui->cboxStopBit->setCurrentIndex(ui->cboxStopBit->findText(QString::number(TCPCom::StopBit)));
//    connect(ui->cboxStopBit, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    ui->ckHexSend->setChecked(TCPCom::HexSend);
    connect(ui->ckHexSend, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckHexReceive->setChecked(TCPCom::HexReceive);
    connect(ui->ckHexReceive, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

//    ui->ckDebug->setChecked(TCPCom::Debug);
//    connect(ui->ckDebug, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));
//
//    ui->ckAutoClear->setChecked(TCPCom::AutoClear);
//    connect(ui->ckAutoClear, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));
//
//    ui->ckAutoSend->setChecked(TCPCom::AutoSend);
//    connect(ui->ckAutoSend, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));
//
//    ui->ckAutoSave->setChecked(TCPCom::AutoSave);
//    connect(ui->ckAutoSave, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));
//
//    QStringList sendInterval;
//    QStringList saveInterval;
//    sendInterval << "100" << "300" << "500";
//
//    for (int i = 1000; i <= 10000; i = i + 1000) {
//        sendInterval << QString::number(i);
//        saveInterval << QString::number(i);
//    }
//
//    ui->cboxSendInterval->addItems(sendInterval);
//    ui->cboxSaveInterval->addItems(saveInterval);
//
//    ui->cboxSendInterval->setCurrentIndex(ui->cboxSendInterval->findText(QString::number(TCPCom::SendInterval)));
//    connect(ui->cboxSendInterval, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));
//    ui->cboxSaveInterval->setCurrentIndex(ui->cboxSaveInterval->findText(QString::number(TCPCom::SaveInterval)));
//    connect(ui->cboxSaveInterval, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    timerSend->setInterval(TCPCom::SendInterval);
    timerSave->setInterval(TCPCom::SaveInterval);

    if (TCPCom::AutoSend) {
        timerSend->start();
    }

    if (TCPCom::AutoSave) {
        timerSave->start();
    }

    //串口转网络部分
//    ui->cboxMode->setCurrentIndex(ui->cboxMode->findText(TCPCom::Mode));
//    connect(ui->cboxMode, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));
//
//    ui->txtServerIP->setText(TCPCom::ServerIP);
//    connect(ui->txtServerIP, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));
//
//    ui->txtServerPort->setText(QString::number(TCPCom::ServerPort));
//    connect(ui->txtServerPort, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));
//
//    ui->txtListenPort->setText(QString::number(TCPCom::ListenPort));
//    connect(ui->txtListenPort, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

//    QStringList values;
//    values << "0" << "10" << "50";
//
//    for (int i = 100; i < 1000; i = i + 100) {
//        values << QString("%1").arg(i);
//    }
//
//    ui->cboxSleepTime->addItems(values);
//
//    ui->cboxSleepTime->setCurrentIndex(ui->cboxSleepTime->findText(QString::number(TCPCom::SleepTime)));
//    connect(ui->cboxSleepTime, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));
//
//    ui->ckAutoConnect->setChecked(TCPCom::AutoConnect);
//    connect(ui->ckAutoConnect, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));
}

void TCPCom::saveConfig() {


    TCPCom::HexSend = ui->ckHexSend->isChecked();
    TCPCom::HexReceive = ui->ckHexReceive->isChecked();



    TCPCom::SaveConstructConfig();
}

void TCPCom::changeEnable(bool b) {
//    ui->cboxBaudRate->setEnabled(!b);
//    ui->cboxDataBit->setEnabled(!b);
//    ui->cboxParity->setEnabled(!b);
//    ui->cboxPortName->setEnabled(!b);
//    ui->cboxStopBit->setEnabled(!b);
//    ui->btnSend->setEnabled(b);
//    ui->ckAutoSend->setEnabled(b);
//    ui->ckAutoSave->setEnabled(b);
}

void TCPCom::append(int type, const QString &data, bool clear) {
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

void TCPCom::readData() {
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

//        //启用调试则模拟调试数据
//        if (ui->ckDebug->isChecked()) {
//            int count = AppData::Keys.count();
//            for (int i = 0; i < count; i++) {
//                if (buffer.startsWith(AppData::Keys.at(i))) {
//                    sendData(AppData::Values.at(i));
//                    break;
//                }
//            }
//        }

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

void TCPCom::sendData() {
    QString str = ui->SendDataEdit->toPlainText();
    if (str.isEmpty()) {
        ui->SendDataEdit->setFocus();
        return;
    }

    sendData(str);

//    if (ui->ckAutoClear->isChecked()) {
//        ui->cboxData->setCurrentIndex(-1);
//        ui->cboxData->setFocus();
//    }
}

void TCPCom::sendData(QString data) {
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

void TCPCom::saveData() {
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

void TCPCom::on_btnOpen_clicked() {

}

void TCPCom::on_btnSendCount_clicked() {
    sendCount = 0;
    ui->btnSendCount->setText("发送 : 0 字节");
}

void TCPCom::on_btnReceiveCount_clicked() {
    receiveCount = 0;
    ui->btnReceiveCount->setText("接收 : 0 字节");
}

void TCPCom::on_btnStopShow_clicked() {
    if (ui->btnStopShow->text() == "停止显示") {
        isShow = false;
        ui->btnStopShow->setText("开始显示");
    }
    else {
        isShow = true;
        ui->btnStopShow->setText("停止显示");
    }
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




void TCPCom::readDataNet() {
    if (socket->bytesAvailable() > 0) {
        QUIHelper::sleep(TCPCom::SleepTime);
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


void TCPCom::GetConstructConfig() {
    cfg->beginGroup(GroupName);
    TCPCom::PortName = cfg->value("PortName", TCPCom::PortName).toString();
    TCPCom::BaudRate = cfg->value("BaudRate", TCPCom::BaudRate).toInt();
    TCPCom::DataBit = cfg->value("DataBit", TCPCom::DataBit).toInt();
    TCPCom::Parity = cfg->value("Parity", TCPCom::Parity).toString();
    TCPCom::StopBit = cfg->value("StopBit", TCPCom::StopBit).toInt();

    TCPCom::HexSend = cfg->value("HexSend", TCPCom::HexSend).toBool();
    TCPCom::HexReceive = cfg->value("HexReceive", TCPCom::HexReceive).toBool();
    TCPCom::Debug = cfg->value("Debug", TCPCom::Debug).toBool();
    TCPCom::AutoClear = cfg->value("AutoClear", TCPCom::AutoClear).toBool();

    TCPCom::AutoSend = cfg->value("AutoSend", TCPCom::AutoSend).toBool();
    TCPCom::SendInterval = cfg->value("SendInterval", TCPCom::SendInterval).toInt();
    TCPCom::AutoSave = cfg->value("AutoSave", TCPCom::AutoSave).toBool();
    TCPCom::SaveInterval = cfg->value("SaveInterval", TCPCom::SaveInterval).toInt();
    cfg->endGroup();

    cfg->beginGroup("NetConfig");
    TCPCom::Mode = cfg->value("Mode", TCPCom::Mode).toString();
    TCPCom::ServerIP = cfg->value("ServerIP", TCPCom::ServerIP).toString();
    TCPCom::ServerPort = cfg->value("ServerPort", TCPCom::ServerPort).toInt();
    TCPCom::ListenPort = cfg->value("ListenPort", TCPCom::ListenPort).toInt();
    TCPCom::SleepTime = cfg->value("SleepTime", TCPCom::SleepTime).toInt();
    TCPCom::AutoConnect = cfg->value("AutoConnect", TCPCom::AutoConnect).toBool();
    cfg->endGroup();

    if (!QUIHelper::checkIniFile(ConfigFilePath)) {
        SaveConstructConfig();
        return;
    }
}

void TCPCom::SaveConstructConfig() {
    cfg->beginGroup(GroupName);
    cfg->setValue("PortName", TCPCom::PortName);
    cfg->setValue("BaudRate", TCPCom::BaudRate);
    cfg->setValue("DataBit", TCPCom::DataBit);
    cfg->setValue("Parity", TCPCom::Parity);
    cfg->setValue("StopBit", TCPCom::StopBit);

    cfg->setValue("HexSend", TCPCom::HexSend);
    cfg->setValue("HexReceive", TCPCom::HexReceive);
    cfg->setValue("Debug", TCPCom::Debug);
    cfg->setValue("AutoClear", TCPCom::AutoClear);

    cfg->setValue("AutoSend", TCPCom::AutoSend);
    cfg->setValue("SendInterval", TCPCom::SendInterval);
    cfg->setValue("AutoSave", TCPCom::AutoSave);
    cfg->setValue("SaveInterval", TCPCom::SaveInterval);
    cfg->endGroup();

    cfg->beginGroup("NetConfig");
    cfg->setValue("Mode", TCPCom::Mode);
    cfg->setValue("ServerIP", TCPCom::ServerIP);
    cfg->setValue("ServerPort", TCPCom::ServerPort);
    cfg->setValue("ListenPort", TCPCom::ListenPort);
    cfg->setValue("SleepTime", TCPCom::SleepTime);
    cfg->setValue("AutoConnect", TCPCom::AutoConnect);
    cfg->endGroup();
}

void TCPCom::on_ckAutoSave_stateChanged(int arg1) {
//    if (arg1 == 0) {
//        ui->cboxSaveInterval->setEnabled(false);
//        timerSave->stop();
//    }
//    else {
//        ui->cboxSaveInterval->setEnabled(true);
//        timerSave->start();
//    }
}

void TCPCom::on_ckAutoSend_stateChanged(int arg1) {
//    if (arg1 == 0) {
//        ui->cboxSendInterval->setEnabled(false);
//        timerSend->stop();
//    }
//    else {
//        ui->cboxSendInterval->setEnabled(true);
//        timerSend->start();
//    }
}

void TCPCom::on_btnStart_clicked() {
//    if (ui->btnStart->text() == "启动") {
//        if (ComTool::ServerIP == "" || ComTool::ServerPort == 0) {
//            append(6, "IP地址和远程端口不能为空");
//            return;
//        }
//
//        socket->connectToHost(ComTool::ServerIP, ComTool::ServerPort);
//        if (socket->waitForConnected(100)) {
//            ui->btnStart->setText("停止");
//            append(6, "连接服务器成功");
//            tcpOk = true;
//        }
//    }
//    else {
//        socket->disconnectFromHost();
//        if (socket->state() == QAbstractSocket::UnconnectedState || socket->waitForDisconnected(100)) {
//            ui->btnStart->setText("启动");
//            append(6, "断开服务器成功");
//            tcpOk = false;
//        }
//    }
}
void TCPCom::connectNet() {
//    if (!tcpOk && ComTool::AutoConnect && ui->btnStart->text() == "启动") {
//        if (ComTool::ServerIP != "" && ComTool::ServerPort != 0) {
//            socket->connectToHost(ComTool::ServerIP, ComTool::ServerPort);
//            if (socket->waitForConnected(100)) {
//                ui->btnStart->setText("停止");
//                append(6, "连接服务器成功");
//                tcpOk = true;
//            }
//        }
//    }
}

void TCPCom::readErrorNet() {
//    ui->btnStart->setText("启动");
//    append(6, QString("连接服务器失败,%1").arg(socket->errorString()));
//    socket->disconnectFromHost();
//    tcpOk = false;
}