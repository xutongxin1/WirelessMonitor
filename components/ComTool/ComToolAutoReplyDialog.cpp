//
// Created by xtx on 24-2-18.
//

// You may need to build the project (run Qt uic code generator) to get "ui_ComToolAutoReplyDialog.h" resolved

#include "ComToolAutoReplyDialog.h"
#include "ui_ComToolAutoReplyDialog.h"


ComToolAutoReplyDialog::ComToolAutoReplyDialog(QWidget *parent) :
    RepeaterDialog(parent), ui(new Ui::ComToolAutoReplyDialog) {
    ui->setupUi(this);
}

ComToolAutoReplyDialog::~ComToolAutoReplyDialog() {
    delete ui;
}
