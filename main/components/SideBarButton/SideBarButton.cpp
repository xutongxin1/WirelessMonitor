//
// Created by xtx on 2022/9/7.
//

// You may need to build the project (run Qt uic code generator) to get "ui_stateButton.h" resolved

#include "SideBarButton.h"
#include "./ui_SideBarButton.h"
#include <string>

SideBarButton::SideBarButton(int num, CfgClass *MainCfg, QWidget *parent) :
        QWidget(parent), ui(new Ui::SideBarButton) {

    ui->setupUi(this);
    QString tmp = "/Device ";
    tmp.append(QString::number(num));

    QString Note = MainCfg->GetMainCfg(QString(tmp + "/note"));
    QString way = MainCfg->GetMainCfg(QString(tmp + "/WayToConnect"));
    if(way=="1")
    {
        way="XMB无线调试器";
    }
    else if(way=="2")
    {
        way="有线串口设备";
    }
    else if(way=="3")
    {
        way="TCP连接";
    }
    ui->Button->setText(Note + "\n" + way);

}

SideBarButton::~SideBarButton() {
    delete ui;
}

SideBarButton::SideBarButton(QWidget *parent)  :
        QWidget(parent), ui(new Ui::SideBarButton) {
    ui->setupUi(this);
    ui->Button->setText("Main");
}
