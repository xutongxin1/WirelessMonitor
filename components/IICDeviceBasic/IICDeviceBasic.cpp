//
// Created by xutongxin on 2023/11/17.
//

// You may need to build the project (run Qt uic code generator) to get "ui_IICDeviceBasic.h" resolved

#include <QFileDialog>
#include "IICDeviceBasic.h"
#include "ui_IICDeviceBasic.h"
#include "IICDeviceEach.h"

IICDeviceBasic::IICDeviceBasic(int device_num, int win_num, QSettings *cfg, ToNewWidget *parent_info,
                               QWidget *parent) :
    RepeaterWidget(parent), ui_(new Ui::IICDeviceBasic) {
    ui_->setupUi(this);

    this->cfg_ = cfg;
    this->group_name_ = "Win" + QString::number(win_num);
    IICDeviceBasic::GetConstructConfig();
    this->parent_info_ = parent_info;
    this->device_num_ = device_num;

    ui_->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui_->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    connect(ui_->ChooseBoardIni, &QPushButton::clicked, this, &IICDeviceBasic::ChooseBoardConfig);

    ui_->tabWidget->addTab(new IICDeviceEach(this), "设备1");
}

IICDeviceBasic::~IICDeviceBasic() {
    delete ui_;
}

void IICDeviceBasic::GetConstructConfig() {
    RepeaterWidget::GetConstructConfig();
}
void IICDeviceBasic::ChooseBoardConfig() {
    QString file_name = QFileDialog::getOpenFileName(
        this,
        tr("打开面板配置文件"),
        last_board_ini_folder_.isEmpty() ? QDir::currentPath() : last_board_ini_folder_,
        tr("BoardConfig(*.ini );;All files(*.*)"));
    if (file_name.isEmpty()) {
        return;
    }
}


