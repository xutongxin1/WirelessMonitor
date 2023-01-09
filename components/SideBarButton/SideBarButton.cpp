//
// Created by xtx on 2022/9/7.
//

// You may need to build the project (run Qt uic code generator) to get "ui_stateButton.h" resolved

#include "SideBarButton.h"
#include "./ui_SideBarButton.h"
#include <string>

SideBarButton::SideBarButton(int num, CfgClass *main_cfg, RepeaterWidget *parent) :
    RepeaterWidget(parent), ui_(new Ui::SideBarButton) {

    ui_->setupUi(this);
    QString tmp = "/Device ";
    tmp.append(QString::number(num));

    QString Note = main_cfg->GetMainCfg(QString(tmp + "/note"));
    QString way = main_cfg->GetMainCfg(QString(tmp + "/WayToConnect"));
    if (way == "1") {
        way = "XMB无线调试器";
    } else if (way == "2") {
        way = "有线串口设备";
    } else if (way == "3") {
        way = "TCP连接";
    }
    ui_->button_->setText(Note + "\n" + way);
    button_ = ui_->button_;
}

SideBarButton::~SideBarButton() {
    delete ui_;
}

SideBarButton::SideBarButton(RepeaterWidget *parent) :
    RepeaterWidget(parent), ui_(new Ui::SideBarButton) {
    ui_->setupUi(this);
    ui_->button_->setText("Main");
    button_ = ui_->button_;
}
