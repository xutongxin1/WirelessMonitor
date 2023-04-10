//
// Created by xtx on 2023/2/27.
//

// You may need to build the project (run Qt uic code generator) to get "ui_ComToolSetting.h" resolved

#include "comtoolsetting.h"
#include "ui_ComToolSetting.h"

ComToolSetting::ComToolSetting(QWidget *parent) :
    RepeaterWidget(parent), ui(new Ui::ComToolSetting) {
    ui->setupUi(this);
}

ComToolSetting::~ComToolSetting() {
    delete ui;
}
