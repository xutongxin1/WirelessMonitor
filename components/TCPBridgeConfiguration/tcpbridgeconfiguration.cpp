//
// Created by xtx on 2022/9/21.
//

// You may need to build the project (run Qt uic code generator) to get "ui_TCPBridgeConfiguration.h" resolved

#include <QListView>
#include "tcpbridgeconfiguration.h"
#include "ui_TCPBridgeConfiguration.h"
#include <QLineEdit>
#include <QJsonObject>
#include <QMessageBox>

TCPBridgeConfiguration::TCPBridgeConfiguration(int DeviceNum, int winNum, QSettings *cfg, ToNewWidget *parentInfo,
                                               QWidget *parent) :
        RepeaterWidget(parent), ui(new Ui::TCPBridgeConfiguration) {
    ui->setupUi(this);

    this->cfg = cfg;
    this->GroupName = "Win" + QString::number(winNum);
    TCPBridgeConfiguration::GetConstructConfig();
    this->parentInfo = parentInfo;
    this->DeviceNum = DeviceNum;
    this->TCPCommandHandle = (*(parentInfo->DevicesInfo))[DeviceNum].TCPCommandHandler;//结构体这样用
    this->TCPInfoHandler[1] = (*(parentInfo->DevicesInfo))[DeviceNum].TCPInfoHandler[1];
    this->TCPInfoHandler[2] = (*(parentInfo->DevicesInfo))[DeviceNum].TCPInfoHandler[2];
    this->TCPInfoHandler[3] = (*(parentInfo->DevicesInfo))[DeviceNum].TCPInfoHandler[3];

    void (QComboBox::*fp)(int) =&QComboBox::currentIndexChanged;
    connect(ui->mode1, fp, this, [&](int num) {
        switch (ui->mode1->currentIndex()) {
            case 1:
                TCPBridgeConfiguration::mode1 = Input;
                break;
            case 2:
                TCPBridgeConfiguration::mode1 = SingleInput;
                break;
            default:
                TCPBridgeConfiguration::mode1 = Closed;
        }
        ChangeMode();
    });

    connect(ui->mode2, fp, this, [&](int num) {
        switch (ui->mode2->currentIndex()) {
            case 1:
                TCPBridgeConfiguration::mode2 = Follow1Output;
                break;
            case 2:
                TCPBridgeConfiguration::mode2 = Follow3Input;
                break;
            default:
                TCPBridgeConfiguration::mode2 = Closed;
        }
        ChangeMode();
    });

    connect(ui->mode3, fp, this, [&](int num) {
        switch (ui->mode3->currentIndex()) {
            case 1:
                TCPBridgeConfiguration::mode3 = Output;
                break;
            case 2:
                TCPBridgeConfiguration::mode3 = SingleOutput;
                break;
            default:
                TCPBridgeConfiguration::mode3 = Closed;
        }
        ChangeMode();
    });
    ChangeMode();//初始化模式选择器



    QStringList baudList;
    baudList << QString::number(TCPBridgeConfiguration::BaudRate1) << QString::number(TCPBridgeConfiguration::BaudRate3)
             << "600" << "1200"
             << "1800" << "2400" << "4800" << "9600" << "14400" << "19200" << "38400"
             << "56000" << "57600" << "76800" << "115200" << "128000" << "256000";

    ui->BaudRate1->addItems(baudList);
    ui->BaudRate3->addItems(baudList);
//    connect(ui->BaudRate1->lineEdit(), &QLineEdit::editingFinished, this, [=] {
//        BaudRate1 = ui->BaudRate1->lineEdit()->text().toInt();
//        ChangeMode();
//    });
//
//    connect(ui->BaudRate3->lineEdit(), &QLineEdit::editingFinished, this, [=] {
//        BaudRate3 = ui->BaudRate3->lineEdit()->text().toInt();
//        ChangeMode();
//    });

    connect(ui->BaudRate1, fp, this, [&](int num) {
        BaudRate1 = ui->BaudRate1->currentText().toInt();
        ChangeMode();
    });
    connect(ui->BaudRate3, fp, this, [&](int num) {
        BaudRate3 = ui->BaudRate3->currentText().toInt();
        ChangeMode();
    });

    QStringList dataBitsList;
    dataBitsList << "5" << "6" << "7" << "8";

    ui->DataBit1->addItems(dataBitsList);
    ui->DataBit3->addItems(dataBitsList);
    connect(ui->DataBit1, fp, this, [&](int num) {
        DataBit1 = ui->DataBit1->currentText().toInt();
        ChangeMode();
    });
    connect(ui->DataBit3, fp, this, [&](int num) {
        DataBit3 = ui->DataBit3->currentText().toInt();
        ChangeMode();
    });

    QStringList parityList;
    parityList << "无" << "奇" << "偶";
    ui->Parity1->addItems(parityList);
    ui->Parity3->addItems(parityList);
    connect(ui->Parity1, fp, this, [&](int num) {
        Parity1 = ui->Parity1->currentText();
        ChangeMode();
    });
    connect(ui->Parity3, fp, this, [&](int num) {
        Parity3 = ui->Parity3->currentText();
        ChangeMode();
    });

    QStringList stopBitsList;
    stopBitsList << "1";
#ifdef Q_OS_WIN
    stopBitsList << "1.5";
#endif
    stopBitsList << "2";

    ui->StopBit1->addItems(stopBitsList);
    ui->StopBit3->addItems(stopBitsList);
    connect(ui->StopBit1, fp, this, [&](int num) {
        StopBit1 = ui->StopBit1->currentText().toInt();
        ChangeMode();
    });
    connect(ui->StopBit3, fp, this, [&](int num) {
        StopBit3 = ui->StopBit3->currentText().toInt();
        ChangeMode();
    });

//    connect(ui->)

    ReflashBox();

    connect(ui->save, &QPushButton::clicked, this, [&] {
        this->SetUart();
    });

}

void TCPBridgeConfiguration::GetConstructConfig() {
    cfg->beginGroup(GroupName);
    TCPBridgeConfiguration::mode1 = IOMode(cfg->value("mode1", TCPBridgeConfiguration::mode1).toInt());
    TCPBridgeConfiguration::mode2 = IOMode(cfg->value("mode2", TCPBridgeConfiguration::mode2).toInt());
    TCPBridgeConfiguration::mode3 = IOMode(cfg->value("mode3", TCPBridgeConfiguration::mode3).toInt());
    TCPBridgeConfiguration::BaudRate1 = cfg->value("BaudRate1", TCPBridgeConfiguration::BaudRate1).toInt();
    TCPBridgeConfiguration::DataBit1 = cfg->value("DataBit1", TCPBridgeConfiguration::DataBit1).toInt();
    TCPBridgeConfiguration::Parity1 = cfg->value("Parity1", TCPBridgeConfiguration::Parity1).toString();
    TCPBridgeConfiguration::StopBit1 = cfg->value("StopBit1", TCPBridgeConfiguration::StopBit1).toInt();
    TCPBridgeConfiguration::BaudRate3 = cfg->value("BaudRate3", TCPBridgeConfiguration::BaudRate3).toInt();
    TCPBridgeConfiguration::DataBit3 = cfg->value("DataBit3", TCPBridgeConfiguration::DataBit3).toInt();
    TCPBridgeConfiguration::Parity3 = cfg->value("Parity3", TCPBridgeConfiguration::Parity3).toString();
    TCPBridgeConfiguration::StopBit3 = cfg->value("StopBit3", TCPBridgeConfiguration::StopBit3).toInt();
    cfg->endGroup();
}

void TCPBridgeConfiguration::SaveConstructConfig() {
    cfg->beginGroup(GroupName);
    cfg->setValue("mode1", TCPBridgeConfiguration::mode1);
    cfg->setValue("mode2", TCPBridgeConfiguration::mode2);
    cfg->setValue("mode3", TCPBridgeConfiguration::mode3);
    cfg->setValue("BaudRate1", TCPBridgeConfiguration::BaudRate1);
    cfg->setValue("DataBit1", TCPBridgeConfiguration::DataBit1);
    cfg->setValue("Parity1", TCPBridgeConfiguration::Parity1);
    cfg->setValue("StopBit1", TCPBridgeConfiguration::StopBit1);
    cfg->setValue("BaudRate3", TCPBridgeConfiguration::BaudRate3);
    cfg->setValue("DataBit3", TCPBridgeConfiguration::DataBit3);
    cfg->setValue("Parity3", TCPBridgeConfiguration::Parity3);
    cfg->setValue("StopBit3", TCPBridgeConfiguration::StopBit3);
    cfg->endGroup();
}

TCPBridgeConfiguration::~TCPBridgeConfiguration() {
    delete ui;
}

void TCPBridgeConfiguration::ChangeMode() {

    //独占模式对中间选项的处理
    if (mode1 == SingleInput || mode3 == SingleOutput) {
        ui->mode2->setItemData(1, 0, Qt::UserRole - 1);
        ui->mode2->setItemData(2, 0, Qt::UserRole - 1);
        mode2 = Closed;

        ui->BaudRate2->setText("");
        ui->DataBit2->setText("");
        ui->Parity2->setText("");
        ui->StopBit2->setText("");

        ui->mode1->setItemData(1, 0, Qt::UserRole - 1);
        ui->mode3->setItemData(1, 0, Qt::UserRole - 1);
        if (mode1 == Input) {
            mode1 = Closed;
        }
        if (mode3 == Output) {
            mode3 = Closed;
        }
    }
    else {
        ui->mode2->setItemData(1, -1, Qt::UserRole - 1);
        ui->mode2->setItemData(2, -1, Qt::UserRole - 1);
        ui->mode1->setItemData(1, -1, Qt::UserRole - 1);
        ui->mode3->setItemData(1, -1, Qt::UserRole - 1);
    }

    if (mode1 == SingleInput && mode3 == SingleOutput)//转发模式逻辑
    {
        ui->transmit->setEnabled(true);
    }
    else {
        ui->transmit->setEnabled(false);
        ui->transmit->setChecked(false);
    }

    //当模式为None时禁用下面的选项
    bool m;
    m = mode1 != Closed;
    ui->BaudRate1->setDisabled(true);
    ui->Parity1->setEnabled(m);
    ui->DataBit1->setEnabled(m);
    ui->StopBit1->setEnabled(m);
    m = mode2 != Closed;
    ui->BaudRate2->setEnabled(m);
    ui->Parity2->setEnabled(m);
    ui->DataBit2->setEnabled(m);
    ui->StopBit2->setEnabled(m);
    m = mode3 != Closed;
    ui->BaudRate3->setEnabled(m);
    ui->Parity3->setEnabled(m);
    ui->DataBit3->setEnabled(m);
    ui->StopBit3->setEnabled(m);


    //独占模式对相互的波特率选项影响
    if (mode1 == SingleInput && mode3 == SingleOutput) {
        ui->BaudRate3->setEnabled(false);
        ui->Parity3->setEnabled(false);
        ui->DataBit3->setEnabled(false);
        ui->StopBit3->setEnabled(false);

        ui->BaudRate1->setEnabled(true);
        ui->Parity1->setEnabled(true);
        ui->DataBit1->setEnabled(true);
        ui->StopBit1->setEnabled(true);

        ui->BaudRate3->setCurrentText(QString::number(BaudRate1));
        ui->DataBit3->setCurrentText(QString::number(DataBit1));
        ui->Parity3->setCurrentText(Parity1);
        ui->StopBit3->setCurrentText(QString::number(StopBit1));
    }
    else if (mode3 == SingleOutput) {
        ui->BaudRate3->setEnabled(true);
        ui->Parity3->setEnabled(true);
        ui->DataBit3->setEnabled(true);
        ui->StopBit3->setEnabled(true);

        ui->BaudRate1->setEnabled(false);
        ui->Parity1->setEnabled(false);
        ui->DataBit1->setEnabled(false);
        ui->StopBit1->setEnabled(false);
    }
    else if (mode1 != Closed) {
        ui->BaudRate1->setEnabled(true);
        ui->Parity1->setEnabled(true);
        ui->DataBit1->setEnabled(true);
        ui->StopBit1->setEnabled(true);
    }
    else if (mode3 != Closed) {

        ui->BaudRate3->setEnabled(true);
        ui->Parity3->setEnabled(true);
        ui->DataBit3->setEnabled(true);
        ui->StopBit3->setEnabled(true);
    }

    //中间模式的逻辑
    if (mode1 == Input) {
        ui->mode2->setItemData(1, -1, Qt::UserRole - 1);
    }
    else {
        ui->mode2->setItemData(1, 0, Qt::UserRole - 1);
        if (mode2 == Follow1Output) {
            mode2 = Closed;
        }
    }
    if (mode3 == Output) {
        ui->mode2->setItemData(2, -1, Qt::UserRole - 1);
    }
    else {
        ui->mode2->setItemData(2, 0, Qt::UserRole - 1);
        if (mode2 == Follow3Input) {
            mode2 = Closed;
        }
    }

    //中间选项波特率的设计
    switch (mode2) {
        case Closed:
            ui->BaudRate2->setText("");
            ui->DataBit2->setText("");
            ui->Parity2->setText("");
            ui->StopBit2->setText("");
            break;
        case Follow1Output:
            ui->BaudRate2->setText(QString::number(BaudRate1));
            ui->DataBit2->setText(QString::number(DataBit1));
            ui->Parity2->setText(Parity1);
            ui->StopBit2->setText(QString::number(StopBit1));
            break;
        case Follow3Input:
            ui->BaudRate2->setText(QString::number(BaudRate3));
            ui->DataBit2->setText(QString::number(DataBit3));
            ui->Parity2->setText(Parity3);
            ui->StopBit2->setText(QString::number(StopBit3));
            break;
        default:
            break;
    }

    SaveConstructConfig();
    ReflashBox();
}

void TCPBridgeConfiguration::ReflashBox() {
    switch (mode1) {
        case Input:
            ui->mode1->setCurrentIndex(1);
            break;
        case SingleInput:
            ui->mode1->setCurrentIndex(2);
            break;
        default:
            ui->mode1->setCurrentIndex(0);
    }
    switch (mode2) {
        case Follow1Output:
            ui->mode2->setCurrentIndex(1);
            break;
        case Follow3Input:
            ui->mode2->setCurrentIndex(2);
            break;
        default:
            ui->mode2->setCurrentIndex(0);
    }
    switch (mode3) {
        case Output:
            ui->mode3->setCurrentIndex(1);
            break;
        case SingleOutput:
            ui->mode3->setCurrentIndex(2);
            break;
        default:
            ui->mode3->setCurrentIndex(0);
    }
    ui->BaudRate1->setCurrentIndex(ui->BaudRate1->findText(QString::number(TCPBridgeConfiguration::BaudRate1)));
    ui->BaudRate3->setCurrentIndex(ui->BaudRate3->findText(QString::number(TCPBridgeConfiguration::BaudRate3)));
    ui->StopBit1->setCurrentIndex(ui->StopBit1->findText(QString::number(TCPBridgeConfiguration::StopBit1)));
    ui->StopBit3->setCurrentIndex(ui->StopBit3->findText(QString::number(TCPBridgeConfiguration::StopBit3)));
    ui->Parity1->setCurrentIndex(ui->Parity1->findText(TCPBridgeConfiguration::Parity1));
    ui->Parity3->setCurrentIndex(ui->Parity3->findText(TCPBridgeConfiguration::Parity3));
    ui->DataBit1->setCurrentIndex(ui->DataBit1->findText(QString::number(TCPBridgeConfiguration::DataBit1)));
    ui->DataBit3->setCurrentIndex(ui->DataBit3->findText(QString::number(TCPBridgeConfiguration::DataBit3)));
}

void TCPBridgeConfiguration::SetUart() {

    if (TCPCommandHandle->getConnectionState()==false) {
        qDebug() << "No connection found";
        return;
    }
    QJsonObject c1;
    if (mode1 == Closed) {
        TCPInfoHandler[1]->changeReadOnly(0);
        c1.insert("mode", 0);
    }
    else {
        TCPInfoHandler[1]->changeReadOnly(1);
        if (mode1 == Input) { c1.insert("mode", 1); }
        else { c1.insert("mode", 3); }
        c1.insert("band", QString::number(BaudRate1));
        c1.insert("stop", QString::number(StopBit1));
        c1.insert("parity", Parity1);
        c1.insert("data", QString::number(DataBit1));
    }
    QJsonObject c2;
    if (mode2 == Closed) {
        c2.insert("mode", 0);
        TCPInfoHandler[2]->changeReadOnly(0);
    }
    else {
        if (mode1 == Follow1Output) {
            c2.insert("mode", 5);
            TCPInfoHandler[2]->changeReadOnly(2);
        }
        else {
            c2.insert("mode", 6);
            TCPInfoHandler[2]->changeReadOnly(1);
        }
    }
    QJsonObject c3;
    if (mode1 == Closed) {
        c3.insert("mode", 0);
        TCPInfoHandler[3]->changeReadOnly(0);
    }
    else {
        TCPInfoHandler[3]->changeReadOnly(2);
        if (mode1 == Output) { c3.insert("mode", 2); }
        else { c3.insert("mode", 4); }
        c3.insert("band", QString::number(BaudRate3));
        c3.insert("stop", QString::number(StopBit3));
        c3.insert("parity", Parity3);
        c3.insert("data", QString::number(DataBit3));
    }
    QJsonObject all;

    all.insert("c1", c1);
    all.insert("c2", c2);
    all.insert("c3", c3);

    ui->save->setEnabled(false);
    ui->save->setText("正在设置");
    connect(TCPCommandHandle, &TCPCommandHandle::sendCommandError, this, [=] {
        QMessageBox::critical(this, tr("错误"), tr("设置串口失败"));
        ui->save->setEnabled(true);
        ui->save->setText("保存并应用");
    });
    connect(TCPCommandHandle, &TCPCommandHandle::sendCommandSuccess, this, [=] {
        QMessageBox::information(this, tr("(*^▽^*)"), tr("设置串口完成，进入串口监视界面"), QMessageBox::Ok, QMessageBox::Ok);
        ui->save->setEnabled(true);
        ui->save->setText("保存并应用");
    });
    TCPCommandHandle->SendCommand(all,"OK!\r\n");
}


