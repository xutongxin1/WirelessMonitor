//
// Created by xtx on 2022/9/7.
//

// You may need to build the project (run Qt uic code generator) to get "ui_stateButton.h" resolved

#include "statebutton.h"
#include "./ui/ui_stateButton.h"


stateButton::stateButton(QWidget *parent) :
        QWidget(parent), ui(new Ui::stateButton) {
    ui->setupUi(this);
}

stateButton::~stateButton() {
    delete ui;
}
