//
// Created by xutongxin on 2023/11/17.
//

// You may need to build the project (run Qt uic code generator) to get "ui_IICDeviceBasic.h" resolved

#include <QFileDialog>
#include <QMessageBox>
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

    this->tcp_command_handle_ = (*(parent_info->devices_info))[device_num].tcp_command_handler;  // 结构体这样用
    this->tcp_info_handler_[1] = (*(parent_info->devices_info))[device_num].tcp_info_handler[1];
    this->tcp_info_handler_[2] = (*(parent_info->devices_info))[device_num].tcp_info_handler[2];
    this->tcp_info_handler_[3] = (*(parent_info->devices_info))[device_num].tcp_info_handler[3];
    tcp_info_handler_[1]->ChangeTCPInfoMode(TCPInfoHandle::TCP_INFO_MODE_BOTH);
    tcp_info_handler_[2]->ChangeTCPInfoMode(TCPInfoHandle::TCP_INFO_MODE_BOTH);



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
    this->ip_ = tcp_command_handle_->ip_;
    if (!tcp_info_handler_[1]->is_connected_) {
        tcp_info_handler_[1]->connectToHost(ip_, 1921, QAbstractSocket::ReadWrite, QAbstractSocket::AnyIPProtocol);
    }
    if (!tcp_info_handler_[2]->is_connected_) {
        tcp_info_handler_[2]->connectToHost(ip_, 1922, QAbstractSocket::ReadWrite, QAbstractSocket::AnyIPProtocol);
    }
    QTimer::singleShot(3000, this, [&] {
      disconnect(tcp_command_handle_, &TCPCommandHandle::SendCommandSuccess, nullptr, nullptr);
      disconnect(tcp_command_handle_, &TCPCommandHandle::SendCommandError, nullptr, nullptr);
      if ((!tcp_info_handler_[1]->is_connected_)
          || (!tcp_info_handler_[2]->is_connected_)) {
          StopAllInfoTCP();
          QMessageBox::critical(this, tr("错误"), tr("连接消息服务器失败"));
      } else {
          //解决收到OK后但INFO通道还没连上的时序问题
//          QMessageBox::information(this, tr("(*^▽^*)"), tr("设置串口完成，进入串口监视界面"), QMessageBox::Ok,
//                                   QMessageBox::Ok);
          emit(OrderShowSnackbar("(*^▽^*)\n设置I2C完成"));
          get_command_reply_ = false;
      }
    });

//    QString file_name = QFileDialog::getOpenFileName(
//        this,
//        tr("打开面板配置文件"),
//        last_board_ini_folder_.isEmpty() ? QDir::currentPath() : last_board_ini_folder_,
//        tr("BoardConfig(*.ini );;All files(*.*)"));
//    if (file_name.isEmpty()) {
//        return;
//    }
}
void IICDeviceBasic::SetIIC() {

    QJsonArray device;
    QJsonObject device_1;
    QString addr = "0xD0";
    device_1.insert("addr", addr.toInt(0, 16));
    device_1.insert("rate", 20000);
    device_1.insert("rst", 1);
    device_1.insert("data", 7);
}


