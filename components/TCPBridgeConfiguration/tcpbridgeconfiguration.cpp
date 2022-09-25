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

//    QStringList baudList;
//    baudList << QString::number(ComTool::BaudRate) << "600" << "1200"
//             << "1800" << "2400" << "4800" << "9600" << "14400" << "19200" << "38400"
//             << "56000" << "57600" << "76800" << "115200" << "128000" << "256000";

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

    //独占模式处理
    if (mode1 == SingleInput || mode3 == SingleOutput) {
        ui->mode2->setItemData(1, 0, Qt::UserRole - 1);
        ui->mode2->setItemData(2, 0, Qt::UserRole - 1);
        mode2 = Closed;
        ui->mode2->setCurrentIndex(0);

        ui->mode1->setItemData(1, 0, Qt::UserRole - 1);
        ui->mode3->setItemData(1, 0, Qt::UserRole - 1);
        if(mode1==Input)
        {
            mode1 = Closed;
            ui->mode1->setCurrentIndex(0);
        }
        if(mode3==Output)
        {
            mode3 = Closed;
            ui->mode3->setCurrentIndex(0);
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

    //中间模式的逻辑
    if (mode1 == Input) {
        ui->mode2->setItemData(1, -1, Qt::UserRole - 1);
    }
    else {
        ui->mode2->setItemData(1, 0, Qt::UserRole - 1);
        if (mode2 == Follow1Output) {
            mode2 = Closed;
            ui->mode2->setCurrentIndex(0);
        }
    }
    if (mode3 == Output) {
        ui->mode2->setItemData(2, -1, Qt::UserRole - 1);
    }
    else {
        ui->mode2->setItemData(2, 0, Qt::UserRole - 1);
        if (mode2 == Follow3Input) {
            mode2 = Closed;
            ui->mode2->setCurrentIndex(0);
        }
    }


    SaveConstructConfig();
}


