//
// Created by xtx on 2022/9/8.
//

// You may need to build the project (run Qt uic code generator) to get "ui_ChannelConfiguration.h" resolved

#include <QToolTip>
#include "channelconfiguration.h"
#include "ui_ChannelConfiguration.h"


ChannelConfiguration::ChannelConfiguration(int DeviceNum, CfgClass *MainCfg, ToNewWidget *parentInfo,
                                           RepeaterWidget *parent)
        :
        RepeaterWidget(parent), ui(new Ui::ChannelConfiguration) {
    ui->setupUi(this);
    ui->ESPButton->setChecked(true);
    ui->Disconnect->setEnabled(false);
    ui->progressBar->hide();
    ui->connectionTip->hide();
    this->parentInfo = parentInfo;
    this->DeviceNum = DeviceNum;
    this->TCPHandler = (*(parentInfo->DevicesInfo))[DeviceNum].TCPHandler;//结构体这样用


    QString cfgText = "/Device " + QString::number(DeviceNum) + "/";
    ui->note->setText(MainCfg->GetMainCfg(cfgText + "note"));

    ui->IP->setText(MainCfg->GetMainCfg(cfgText + "IP"));

    connect(ui->ESPButton, &QAbstractButton::toggled, this, [=] {
        if (ui->ESPButton->isChecked()) {
            MainCfg->SaveMainCfg(cfgText + "WayToConnect", "1");
            reflashUi(true);
        }
    });
    connect(ui->COMButton, &QAbstractButton::toggled, this, [=] {
        if (ui->COMButton->isChecked()) {
            MainCfg->SaveMainCfg(cfgText + "WayToConnect", "2");
            reflashUi(false);
        }
    });

    connect(ui->note, &QLineEdit::editingFinished, this, [=] {
        MainCfg->SaveMainCfg(cfgText + "note", ui->note->text());
    });

    connect(ui->IP, &QLineEdit::editingFinished, this, [=] {
        MainCfg->SaveMainCfg(cfgText + "IP", ui->IP->text());
        ui->IP->setStyleSheet("QLineEdit{border:1px solid gray border-radius:1px}");
    });

    connect(ui->Connect, &QPushButton::clicked, this, &ChannelConfiguration::onConnect);
    connect(ui->Disconnect, &QPushButton::clicked, this, &ChannelConfiguration::onDisconnect);

    QRegExp rx(
            R"((25[0-5]|2[0-4]\d|[0-1]\d{2}|[1-9]?\d)\.(25[0-5]|2[0-4]\d|[0-1]\d{2}|[1-9]?\d)\.(25[0-5]|2[0-4]\d|[0-1]\d{2}|[1-9]?\d)\.(25[0-5]|2[0-4]\d|[0-1]\d{2}|[1-9]?\d))");


    auto *pReg = new QRegExpValidator(rx, this);

    ui->IP->setValidator(pReg);

}

ChannelConfiguration::~ChannelConfiguration() {
    delete ui;
}

/*!
 * 连接按钮按下事件
 */
void ChannelConfiguration::onConnect() {
    if (!(ui->IP->hasAcceptableInput())) {
        QToolTip::showText(ui->IP->mapToGlobal(QPoint(100, 0)), "IP地址输入有误");
        // 设置LineEdit变为红色
        ui->IP->setStyleSheet("QLineEdit{border:1px solid red }");
        return;
    }
    ui->progressBar->show();
    ui->progressBar->setValue(5);

    ui->connectionTip->show();
    ui->connectionTip->setText("正在连接到调试器");

    //连接逻辑
//    QStringList list = ui->IP->text().split(":");
//    ip = list[0];
//    port = list[1].toInt();

    disconnect(TCPHandler,0,0,0);
    ip = ui->IP->text();
    connect(TCPHandler, &TCPCommandHandle::hasConnected, this, [=] {
        disconnect(TCPHandler, &TCPCommandHandle::hasConnected, 0, 0);
        ui->progressBar->setValue(25);
        ui->connectionTip->setText("正在验证调试器状态");
        TCPHandler->SendHeart();
    });
    TCPHandler->connectToHost(ip, 1920, QAbstractSocket::ReadWrite, QAbstractSocket::AnyIPProtocol);
    ui->Connect->setEnabled(false);

    connect(TCPHandler,&TCPCommandHandle::receiveFirstHeart,this,[=]
    {
        //第一次收到是检查模块状态
        disconnect(TCPHandler, &TCPCommandHandle::receiveFirstHeart, 0, 0);
        ui->progressBar->setValue(45);
        ui->connectionTip->setText("正在设置调试器模式");

        connect(TCPHandler,&TCPCommandHandle::receiveFirstHeart,this,[=]
        {//第二次收到，证明模式切换全部完成
            ui->progressBar->setValue(100);
            ui->connectionTip->setText("调试器模式设置完成，请进行下一步配置");
            disconnect(TCPHandler, &TCPCommandHandle::receiveFirstHeart, 0, 0);

            ui->Disconnect->setEnabled(true);

        });
        TCPHandler->setMode(1);
    });
    connect(TCPHandler,&TCPCommandHandle::readyReboot,this,[=]
    {
        ui->progressBar->setValue(65);
        ui->connectionTip->setText("设置完成，等待重启");
        disconnect(TCPHandler, &TCPCommandHandle::readyReboot, 0, 0);
    });

    connect(TCPHandler,&TCPCommandHandle::ModeChangeSuccess,this,[=]
    {
        ui->progressBar->setValue(85);
        ui->connectionTip->setText("模式设置成功，检查模块状态");
        disconnect(TCPHandler, &TCPCommandHandle::ModeChangeSuccess, 0, 0);
    });

    QTimer::singleShot(45000, this, [=] {
        if(ui->progressBar->value()!=100)
        {
            TCPHandler->disconnectFromHost();
            ui->connectionTip->setText(ui->connectionTip->text()+"\n错误:操作超时");
            ui->Connect->setEnabled(true);
        }
    });




}

void ChannelConfiguration::SendModePackage() {
    TCPHandler->write("COM\r\n");
    char *buffer;
    connect(TCPHandler, &QTcpSocket::readyRead, this, [=]() {
        TCPHandler->read(buffer, 1024);
        if (strlen(buffer) == 6 && strncmp(buffer, " OK!\r\n", 6) == 0) {
            ConnectStep = 2;
            TCPHandler->disconnect(SIGNAL(readyRead()));

        }
    });
}

/*!
 * 断开按钮按下事件
 */
void ChannelConfiguration::onDisconnect() {
    connect(TCPHandler, &TCPCommandHandle::hasDisconnected, this, [=] {
        disconnect(TCPHandler, &TCPCommandHandle::hasDisconnected, 0, 0);
        ui->Connect->setEnabled(true);
        ui->Disconnect->setEnabled(false);
    });
    TCPHandler->disconnectFromHost();
}

void ChannelConfiguration::reflashUi(bool isXMB) {
    ui->label_3->setVisible(isXMB);
    ui->FuncitonComboBox->setVisible(isXMB);
    ui->label_2->setVisible(isXMB);
    ui->IP->setVisible(isXMB);
    ui->Connect->setVisible(isXMB);
    ui->Disconnect->setVisible(isXMB);
}

void ChannelConfiguration::SecondConnect() {

}
