//
// Created by xutongxin on 2023/11/17.
//

// You may need to build the project (run Qt uic code generator) to get "ui_IICDeviceBasic.h" resolved

#include "IICDeviceBasic.h"
#include "ui_IICDeviceBasic.h"

IICDeviceBasic::IICDeviceBasic(QWidget *parent) :
    RepeaterWidget(parent), ui_(new Ui::IICDeviceBasic) {
    ui_->setupUi(this);
}

IICDeviceBasic::~IICDeviceBasic() {
    delete ui_;
}
