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
    this->parentInfo = parentInfo;
    this->DeviceNum = DeviceNum;
    this->TCPHandler = (*(parentInfo->DevicesSelfInfo))[DeviceNum].TCPHandler;//结构体这样用


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
            R"(^(\d|[1-9]\d|1\d{2}|2[0-4]\d|25[0-5])\.(\d|[1-9]\d|1\d{2}|2[0-4]\d|25[0-5])\.(\d|[1-9]\d|1\d{2}|2[0-4]\d|25[0-5])\.(\d|[1-9]\d|1\d{2}|2[0-4]\d|25[0-5]):([0-9]|[1-9]\d|[1-9]\d{2}|[1-9]\d{3}|[1-5]\d{4}|6[0-4]\d{3}|65[0-4]\d{2}|655[0-2]\d|6553[0-5])$)");

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
        QToolTip::showText(ui->IP->mapToGlobal(QPoint(100, 0)), "IP端口输入有误");
        // 设置LineEdit变为红色
        ui->IP->setStyleSheet("QLineEdit{border:1px solid red }");
        return;
    }

    //连接逻辑
    QStringList list = ui->IP->text().split(":");
    ip = list[0];
    port = list[1].toInt();

    //第一次连接
    ConnectStep=1;
    TCPHandler->connectToHost(ip, port);
//    bool connected = TCPHandler->waitForConnected();
    connect(TCPHandler,&QTcpSocket::connected,this,[&]{
        switch(ConnectStep)
        {
            case 1:
                SendModePackage();
                break;
            case 2:
                break;
            default:
                break;
        }
    });
}

void ChannelConfiguration::SendModePackage() {
    TCPHandler->write("COM\r\n");
    char *buffer;
    connect(TCPHandler,&QTcpSocket::readyRead,this,[=](){
        TCPHandler->read(buffer,1024);
        if(strlen(buffer)==6 &&strncmp(buffer," OK!\r\n",6)==0)
        {
            ConnectStep=2;
        }
    });

}

/*!
 * 断开按钮按下事件
 */
void ChannelConfiguration::onDisconnect() {
    ui->Connect->setEnabled(true);
    ui->Disconnect->setEnabled(false);
}

void ChannelConfiguration::reflashUi(bool isXMB) {
    ui->label_3->setVisible(isXMB);
    ui->FuncitonComboBox->setVisible(isXMB);
    ui->label_2->setVisible(isXMB);
    ui->IP->setVisible(isXMB);
    ui->Connect->setVisible(isXMB);
    ui->Disconnect->setVisible(isXMB);

}
