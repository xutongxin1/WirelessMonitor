//
// Created by xtx on 2022/9/7.
//

// You may need to build the project (run Qt uic code generator) to get "ui_stateButton.h" resolved

#include "SideBarButton.h"
#include "./ui_SideBarButton.h"
#include <string>

SideBarButton::SideBarButton(int num, ConfigClass *device_cfg, RepeaterWidget *parent) :
    RepeaterWidget(parent), ui_(new Ui::SideBarButton) {

    ui_->setupUi(this);

    QString note = device_cfg->value("/All/note").toString();
    QString connect_mode = device_cfg->value("/All/connect_mode").toString();
    QString attach = "";
    if (connect_mode == "1") {
        attach = device_cfg->value("/All/ip").toString();
        connect_mode = "XMB无线调试器";
    } else if (connect_mode == "2") {
        connect_mode = "有线串口设备";
    } else if (connect_mode == "3") {
        connect_mode = "TCP连接";
    }
    ui_->button_->setText(note + "\n" + connect_mode + "\n" + attach);
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
