//
// Created by xtx on 2022/9/21.
//

// You may need to build the project (run Qt uic code generator) to get "ui_TCPBridgeConfiguration.h" resolved

#include "tcpbridgeconfiguration.h"
#include "ui_TCPBridgeConfiguration.h"


TCPBridgeConfiguration::TCPBridgeConfiguration(QWidget *parent) :
        RepeaterWidget(parent), ui(new Ui::TCPBridgeConfiguration) {
    ui->setupUi(this);
}

TCPBridgeConfiguration::~TCPBridgeConfiguration() {
    delete ui;
}
