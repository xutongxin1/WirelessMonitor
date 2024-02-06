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

RepeaterWidget::~RepeaterWidget() = default;

void RepeaterWidget::SaveConstructConfig() {

}

void RepeaterWidget::StopAllInfoTCP() {
    if (tcp_info_handler_[1] == nullptr) { return; }
    if (tcp_info_handler_[2] == nullptr) { return; }
    if (tcp_info_handler_[3] == nullptr) { return; }
    disconnect(tcp_info_handler_[1], &TCPInfoHandle::RecNewData, 0, 0);
    disconnect(tcp_info_handler_[2], &TCPInfoHandle::RecNewData, 0, 0);
    disconnect(tcp_info_handler_[3], &TCPInfoHandle::RecNewData, 0, 0);
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
