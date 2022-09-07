//
// Created by xtx on 2022/9/7.
//

// You may need to build the project (run Qt uic code generator) to get "ui_stateButton.h" resolved

#include "statebutton.h"
#include "./ui/ui_stateButton.h"
#include <string>

stateButton::stateButton(int num, CfgClass *MainCfg, QWidget *parent) :
        QWidget(parent), ui(new Ui::stateButton) {

    ui->setupUi(this);
    QString tmp = "/Device ";
    tmp.append(QString::number(num));

    QString Note = MainCfg->GetMainCfg(QString(tmp + "/note"));
    QString Port = MainCfg->GetMainCfg(QString(tmp + "/port"));
    ui->Button->setText(Note + "\n" + "端口: " + Port);

}

stateButton::~stateButton() {
    delete ui;
}

stateButton::stateButton(QWidget *parent)  :
        QWidget(parent), ui(new Ui::stateButton) {
    ui->setupUi(this);
}
