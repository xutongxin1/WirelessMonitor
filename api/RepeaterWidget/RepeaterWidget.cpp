//
// Created by xtx on 2022/9/15.
//

#include "RepeaterWidget.h"


void RepeaterWidget::GetObjectFromUi(QPushButtonMap &result) {

}

RepeaterWidget::RepeaterWidget(QWidget *parent): QWidget(parent) {

}

void RepeaterWidget::GetConstructConfig() {

}

RepeaterWidget::~RepeaterWidget() {

}

void RepeaterWidget::SaveConstructConfig() {

}

void RepeaterWidget::StopAllInfoTCP() {
    if (tcp_info_handler_[1] == nullptr) { return; }
    if (tcp_info_handler_[1]->is_connected_) {
        tcp_info_handler_[1]->disconnectFromHost();
    }
    if (tcp_info_handler_[2]->is_connected_) {
        tcp_info_handler_[2]->disconnectFromHost();
    }
    if (tcp_info_handler_[3]->is_connected_) {
        tcp_info_handler_[3]->disconnectFromHost();
    }
}

/*Ui::Charts *RepeaterWidget::GetChartUi()
{

}

QList<QPair<QString,Datanode>> RepeaterWidget::GetChartDataPair()
{

}*/
