//
// Created by xtx on 2022/9/21.
//

// You may need to build the project (run Qt uic code generator) to get "ui_TCPBridgeConfiguration.h" resolved

#include <QListView>
#include "tcpbridgeconfiguration.h"
#include "ui_TCPBridgeConfiguration.h"


TCPBridgeConfiguration::TCPBridgeConfiguration(int DeviceNum, int winNum, QSettings *cfg, QWidget *parent) :
        RepeaterWidget(parent), ui(new Ui::TCPBridgeConfiguration) {
    ui->setupUi(this);

    this->cfg = cfg;
    this->ConfigFilePath = "Win" + QString::number(winNum);
    TCPBridgeConfiguration::GetConstructConfig();
    ui->mode1->setCurrentIndex(mode1);
    ui->mode2->setCurrentIndex(mode2);
    ui->mode3->setCurrentIndex(mode3);

    void (QComboBox::*fp)(int) =&QComboBox::currentIndexChanged;
    connect(ui->mode1, fp, this, [&](int num) {
        TCPBridgeConfiguration::mode1 = IOMode(ui->mode1->currentIndex());
        ChangeMode();
    });

    connect(ui->mode2, fp, this, [&](int num) {
        TCPBridgeConfiguration::mode2 = IOMode(ui->mode2->currentIndex());
        ChangeMode();
    });

    connect(ui->mode3, fp, this, [&](int num) {
        TCPBridgeConfiguration::mode3 = IOMode(ui->mode3->currentIndex());
        ChangeMode();
    });
    ChangeMode();//初始化模式选择器

    QStringList baudList;
    baudList << QString::number(ComTool::BaudRate) << "600" << "1200"
             << "1800" << "2400" << "4800" << "9600" << "14400" << "19200" << "38400"
             << "56000" << "57600" << "76800" << "115200" << "128000" << "256000";

}

void TCPBridgeConfiguration::GetConstructConfig() {
    cfg->beginGroup(GroupName);
    TCPBridgeConfiguration::mode1 = IOMode(cfg->value("mode1", TCPBridgeConfiguration::mode1).toInt());
    TCPBridgeConfiguration::mode2 = IOMode(cfg->value("mode2", TCPBridgeConfiguration::mode2).toInt());
    TCPBridgeConfiguration::mode3 = IOMode(cfg->value("mode3", TCPBridgeConfiguration::mode3).toInt());
    cfg->endGroup();
}

void TCPBridgeConfiguration::SaveConstructConfig() {
    cfg->beginGroup(GroupName);
    cfg->setValue("mode1", TCPBridgeConfiguration::mode1);
    cfg->setValue("mode2", TCPBridgeConfiguration::mode2);
    cfg->setValue("mode3", TCPBridgeConfiguration::mode3);
    cfg->endGroup();
}

TCPBridgeConfiguration::~TCPBridgeConfiguration() {
    delete ui;
}

void TCPBridgeConfiguration::ChangeMode() {
    int hasSingle = -1;
    int isSingle = -1;
    if (TCPBridgeConfiguration::mode1 == SingleInput) {
        isSingle = 0;
        ui->mode2->setItemData(3, 0, Qt::UserRole - 1);
        ui->mode3->setItemData(3, 0, Qt::UserRole - 1);
        hasSingle++;
    }
    else {
        ui->mode2->setItemData(3, -1, Qt::UserRole - 1);
        ui->mode3->setItemData(3, -1, Qt::UserRole - 1);
    }


    if (TCPBridgeConfiguration::mode2 == SingleInput) {
        ui->mode1->setItemData(3, 0, Qt::UserRole - 1);
        ui->mode3->setItemData(3, 0, Qt::UserRole - 1);
        isSingle = 0;
        hasSingle++;
    }
    else if (isSingle != 0) {
        ui->mode1->setItemData(3, -1, Qt::UserRole - 1);
        ui->mode3->setItemData(3, -1, Qt::UserRole - 1);
    }


    if (TCPBridgeConfiguration::mode3 == SingleInput) {
        ui->mode1->setItemData(3, 0, Qt::UserRole - 1);
        ui->mode2->setItemData(3, 0, Qt::UserRole - 1);
        isSingle = 0;
        hasSingle++;
    }
    else if (isSingle != 0) {
        ui->mode1->setItemData(3, -1, Qt::UserRole - 1);
        ui->mode2->setItemData(3, -1, Qt::UserRole - 1);
    }

    isSingle=-1;
    if (TCPBridgeConfiguration::mode1 == SingleOutput) {
        ui->mode2->setItemData(4, 0, Qt::UserRole - 1);
        ui->mode3->setItemData(4, 0, Qt::UserRole - 1);
        isSingle = 0;
        hasSingle++;
    }
    else {
        ui->mode2->setItemData(4, -1, Qt::UserRole - 1);
        ui->mode3->setItemData(4, -1, Qt::UserRole - 1);
    }


    if (TCPBridgeConfiguration::mode2 == SingleOutput) {
        ui->mode3->setItemData(4, 0, Qt::UserRole - 1);
        ui->mode1->setItemData(4, 0, Qt::UserRole - 1);
        isSingle = 0;
        hasSingle++;
    }
    else if (isSingle != 0) {
        ui->mode3->setItemData(4, -1, Qt::UserRole - 1);
        ui->mode1->setItemData(4, -1, Qt::UserRole - 1);
    }


    if (TCPBridgeConfiguration::mode3 == SingleOutput) {
        ui->mode2->setItemData(4, 0, Qt::UserRole - 1);
        ui->mode1->setItemData(4, 0, Qt::UserRole - 1);
        isSingle = 0;
        hasSingle++;
    }
    else if (isSingle != 0) {
        ui->mode2->setItemData(4, -1, Qt::UserRole - 1);
        ui->mode1->setItemData(4, -1, Qt::UserRole - 1);
    }

    if(hasSingle!=-1)
    {
        if(ui->mode1->currentIndex()==1||ui->mode1->currentIndex()==2)ui->mode1->setCurrentIndex(0);
        if(ui->mode2->currentIndex()==1||ui->mode2->currentIndex()==2)ui->mode2->setCurrentIndex(0);
        if(ui->mode3->currentIndex()==1||ui->mode3->currentIndex()==2)ui->mode3->setCurrentIndex(0);
    }
    switch (hasSingle) {
        case 1:
            ui->transmit->setEnabled(true);
            hasSingle = 0;
            break;
        case 0:
            hasSingle = 0;
        case -1:
            ui->transmit->setEnabled(false);
            ui->transmit->setChecked(false);
            break;
        default:
            break;
    }


    ui->mode1->setItemData(1, hasSingle, Qt::UserRole - 1);
    ui->mode1->setItemData(2, hasSingle, Qt::UserRole - 1);
    ui->mode2->setItemData(1, hasSingle, Qt::UserRole - 1);
    ui->mode2->setItemData(2, hasSingle, Qt::UserRole - 1);
    ui->mode3->setItemData(1, hasSingle, Qt::UserRole - 1);
    ui->mode3->setItemData(2, hasSingle, Qt::UserRole - 1);
//    ui->mode1->setView(new QListView);
//    ui->mode2->setView(new QListView);
//    ui->mode3->setView(new QListView);
    SaveConstructConfig();
}

