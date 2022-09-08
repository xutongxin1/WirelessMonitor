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
    QString Port = MainCfg->GetMainCfg(QString(tmp + "/port"));
    ui->Button->setText(Note + "\n" + "端口: " + Port);

}

SideBarButton::~SideBarButton() {
    delete ui;
}

SideBarButton::SideBarButton(QWidget *parent)  :
        QWidget(parent), ui(new Ui::SideBarButton) {
    ui->setupUi(this);
    ui->Button->setText("Main");
}
