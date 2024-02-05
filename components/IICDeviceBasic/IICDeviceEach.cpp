//
// Created by xtx on 2023/11/27.
//

// You may need to build the project (run Qt uic code generator) to get "ui_IICDeviceEach.h" resolved

#include "IICDeviceEach.h"
#include "ui_IICDeviceEach.h"

IICDeviceEach::IICDeviceEach(QWidget *parent) :
    RepeaterWidget(parent), ui_(new Ui::IICDeviceEach) {
    ui_->setupUi(this);


    ui_->splitter->setStretchFactor(0, 1);
    ui_->splitter->setStretchFactor(1, 3);

    ui_->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui_->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    python_work_ = new PythonWorkWithI2C();
    //test
    connect(ui_->ConfigAutoScan, &QPushButton::clicked, this, [&] {
      qDebug() << python_work_->ReciveDataFromI2C("3B", "FB92FFD01E3E");
      qDebug() << python_work_->ReciveDataFromI2C("43", "07DF0AC0F512");
    });

}

IICDeviceEach::~IICDeviceEach() {
    delete ui_;
}
