//
// Created by xtx on 2022/9/8.
//

// You may need to build the project (run Qt uic code generator) to get "ui_ChannelConfiguration.h" resolved

#include "channelconfiguration.h"
#include "ui_ChannelConfiguration.h"


ChannelConfiguration::ChannelConfiguration(QWidget *parent) :
        QWidget(parent), ui(new Ui::ChannelConfiguration) {
    ui->setupUi(this);
    ui->ESPButton->setChecked(true);
    ui->Disconnect->setEnabled(false);
}

ChannelConfiguration::~ChannelConfiguration() {
    delete ui;
}
