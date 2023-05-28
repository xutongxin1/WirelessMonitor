//
// Created by xtx on 2022/9/16.
//

// You may need to build the project (run Qt uic code generator) to get "ui_TCPCom.h" resolved

#include "TCPCom.h"
#include "ui_tcpcom.h"
#include "quihelper.h"
#include "quihelperdata.h"

/*
 * TODO:shell语法高亮，使用正则表达式https://c.runoob.com_/front-end/
 * TODO:以回车分隔
 * TODO:ui修改
 */
TCPCom::TCPCom(int device_num, int win_num, QSettings *cfg, ToNewWidget *parent_info, QWidget *parent) :
    RepeaterWidget(parent), ui_(new Ui::TCPCom) {
    this->cfg_ = cfg;
    this->config_file_path_ = "./config/Device" + QString::number(device_num) + ".ini";

    this->group_name_ = "Win" + QString::number(win_num);
    QuiHelper::InitAll();
    TCPCom::GetConstructConfig();

    AppData::ReadSendData();
    AppData::ReadDeviceData();

    ui_->setupUi(this);

    this->InitForm();
//    this->InitConfig();
    QuiHelper::SetFormInCenter(this);

    this->tcp_command_handle_ = (*(parent_info->devices_info))[device_num].tcp_command_handler;//结构体这样用
    this->tcp_info_handler_[1] = (*(parent_info->devices_info))[device_num].tcp_info_handler[1];
    this->tcp_info_handler_[2] = (*(parent_info->devices_info))[device_num].tcp_info_handler[2];
    this->tcp_info_handler_[3] = (*(parent_info->devices_info))[device_num].tcp_info_handler[3];

    this->ip_ = tcp_command_handle_->ip_;

    connect(tcp_command_handle_, &TCPCommandHandle::StartInfoConnection, this, [&] {
//        disconnect(tcp_command_handle_, &tcp_command_handle_::StartInfoConnection, 0, 0);

      //选项栏绑定
      if (tcp_info_handler_[1]->tcp_mode_ == TCPInfoHandle::TCP_INFO_MODE_BOTH) {
          ui_->channelToSend->setItemData(1, -1, Qt::UserRole - 1);
          connect(tcp_info_handler_[1], &TCPInfoHandle::RecNewData, this,
                  [&](const QByteArray &data, const QString &ip, int port, const QDateTime &time) {
                    this->GetData(data, port);
                  });
      } else {
          ui_->channelToSend->setItemData(1, 0, Qt::UserRole - 1);
      }
      if (tcp_info_handler_[2]->tcp_mode_ == TCPInfoHandle::TCP_INFO_MODE_BOTH) {
          connect(tcp_info_handler_[2], &TCPInfoHandle::RecNewData, this,
                  [&](const QByteArray &data, const QString &ip, int port, const QDateTime &time) {
                    this->GetData(data, port);
                  });
          ui_->channelToSend->setItemData(2, -1, Qt::UserRole - 1);
      } else {
          ui_->channelToSend->setItemData(2, 0, Qt::UserRole - 1);
      }

    });

    connect(ui_->btnSend, &QPushButton::clicked, this, [&] {
      this->SendData();
    });

    connect(ui_->btnPaste, &QPushButton::clicked, this, [&] {
      QClipboard *clipboard = QApplication::clipboard();
      QString text = clipboard->text();
      ui_->SendDataEdit->setPlainText(text);
      this->SendData();
    });

#if DEBUG
    //测试按钮绑定
    ui_->btnStartTest->setHidden(false);
    connect(ui_->btnStartTest, &QPushButton::clicked, this, [&] {
      qDebug("工作在测试模式，发送通道的comboBox设置错误为正常现象");
      this->ip_ = "127.0.0.1";
      if (!tcp_info_handler_[1]->is_connected_) {
          tcp_info_handler_[1]->connectToHost(ip_, 1921, QAbstractSocket::ReadWrite, QAbstractSocket::AnyIPProtocol);
      }
      if (!tcp_info_handler_[2]->is_connected_) {
          tcp_info_handler_[2]->connectToHost(ip_, 1922, QAbstractSocket::ReadWrite, QAbstractSocket::AnyIPProtocol);
      }

      //默认设置模式
      connect(tcp_info_handler_[2], &QTcpSocket::connected, this, [&] {
        tcp_info_handler_[1]->ChangeTCPInfoMode(TCPInfoHandle::TCP_INFO_MODE_BOTH);
        tcp_info_handler_[2]->ChangeTCPInfoMode(TCPInfoHandle::TCP_INFO_MODE_BOTH);
        //数据接收绑定
        if (tcp_info_handler_[1]->tcp_mode_ == TCPInfoHandle::TCP_INFO_MODE_BOTH) {
            connect(tcp_info_handler_[1], &TCPInfoHandle::RecNewData, this,
                    [&](const QByteArray &data, const QString &ip, int port, const QDateTime &time) {
                      this->GetData(data, port);
                    });
        }
        if (tcp_info_handler_[2]->tcp_mode_ == TCPInfoHandle::TCP_INFO_MODE_BOTH) {
            connect(tcp_info_handler_[2], &TCPInfoHandle::RecNewData, this,
                    [&](const QByteArray &data, const QString &ip, int port, const QDateTime &time) {
                      this->GetData(data, port);
                    });
        }
      });

    });
#else
    ui_->btnStartTest->setHidden(true);
#endif

    //高亮转义字符
    highlighter1 = new Highlighter(ui_->SendDataEdit->document());
    highlighter2 = new Highlighter(ui_->txtMain->document());

    //表格自动拉宽
    ui_->historyTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

}

TCPCom::~TCPCom() {
    delete ui_;
}
///初始化统计
void TCPCom::InitForm() {
    sleep_time_rec_ = 10;
    receive_count_ = 0;
    send_count_ = 0;
    is_show_ = true;

    ui_->tabWidget->setCurrentIndex(0);

#ifdef __arm__
    ui_->widgetRight->setFixedWidth(280);
#endif
}

/// 往数据日志区添加数据
/// \param type 数据类型
/// \param data 数据
/// \param clear 是否清空
void TCPCom::Append(int type, const QString &data, bool clear) {
    static int current_count = 0;
    static int max_count = 81920;

    if (clear) {
        ui_->txtMain->clear();
        current_count = 0;
        return;
    }

    if (current_count >= max_count) {
        ui_->txtMain->clear();
        current_count = 0;
    }

    if (!is_show_) {
        return;
    }

//    //过滤回车换行符
    QString str_data = data;
    str_data = str_data.replace("\a", "\\a");
    str_data = str_data.replace("\b", "\\b");
    str_data = str_data.replace("\f", "\\f");
    str_data = str_data.replace("\t", "\\t");
    str_data = str_data.replace("\v", "\\v");
    str_data = str_data.replace("\\", "\\\\");
    str_data = str_data.replace("\'", "\\'");
    str_data = str_data.replace("\"", R"RX(\\")RX");
    str_data = str_data.replace("\r", "\\r");
    str_data = str_data.replace("\n", "\\n\n");

    //不同类型不同颜色显示
    QString str_type;
    if (type == 0) {
        str_type = "一通道接收 <<";
        ui_->txtMain->setTextColor(QColor("dodgerblue"));
    } else if (type == 1) {
        str_type = "二通道接收 <<";
        ui_->txtMain->setTextColor(QColor("black"));
    }
    else if (type == 2) {
        str_type = "二通道发送 >>";
        ui_->txtMain->setTextColor(QColor("gray"));
    } else if (type == 3) {
        str_type = "三通道发送 >>";
        ui_->txtMain->setTextColor(QColor("green"));
    } else if (type == 4) {
        str_type = "提示信息 >>";
        ui_->txtMain->setTextColor(QColor(100, 184, 255));
    }

    if (str_data.at(str_data.length() - 1) != '\n') {
        str_data = QString("时间[%1] %2 %3\n").arg(TIMEMS, str_type, str_data);
    } else {
        str_data = QString("时间[%1] %2 %3").arg(TIMEMS, str_type, str_data);
    }

    ui_->txtMain->append(str_data);
    current_count++;
}

/// 数据收入处理
/// \param data 数据
/// \param port 端口
void TCPCom::GetData(const QByteArray &data, int port) {

    QString buffer;
    if (ui_->ckHexReceive->isChecked()) {
        buffer = QUIHelperData::byteArrayToHexStr(data);
    } else {
        buffer = QString::fromLocal8Bit(data);
    }

    if (port == 1) {
        Append(0, buffer);
    } else if (port == 2) {
        Append(1, buffer);
    }

    receive_count_ = receive_count_ + data.size();
    ui_->btnReceiveCount->setText(QString("接收 : %1 字节").arg(receive_count_));
}

///发送发送栏里的数据
void TCPCom::SendData() {
    QString data = ui_->SendDataEdit->toPlainText();
    if (data.isEmpty()) {
        ui_->SendDataEdit->setFocus();
        return;
    }

    if (history_send_list_.contains(data)) {
        if (ui_->ckHexSend->isChecked() != history_send_list_[data].is_Hex) {
            history_send_list_[data].is_Hex = ui_->ckHexSend->isChecked();
            history_send_list_[data].send_num = 1;
        } else {
            history_send_list_[data].send_num++;
        }

        history_send_list_[data].time = QDateTime::currentDateTime();
    } else {
        HistorySend tmp;
        tmp.is_Hex = ui_->ckHexSend->isChecked();
        tmp.data = data;
        tmp.time = QDateTime::currentDateTime();
        history_send_list_.insert(data, tmp);
    }
    UpdateSendHistory();

    data = data.replace("\\n", "\n");
    data = data.replace("\\a", "\a");
    data = data.replace("\\b", "\b");
    data = data.replace("\\f", "\f");
    data = data.replace("\\r", "\r");
    data = data.replace("\\t", "\t");
    data = data.replace("\\v", "\v");
    data = data.replace("\\\\", "\\");
    data = data.replace("\\'", "\'");
    data = data.replace(R"RX(\\")RX", "\"");

    if (ui_->checkBox->isChecked()) {
        data = data.append("\r\n");
    }

    QByteArray buffer;
    if (ui_->ckHexSend->isChecked()) {
        buffer = QUIHelperData::hexStrToByteArray(data);
    } else {
        buffer = QUIHelperData::asciiStrToByteArray(data);
    }

    if (ui_->channelToSend->currentIndex() == 0) {
        if (tcp_info_handler_[1]->tcp_mode_ == TCPInfoHandle::TCP_INFO_MODE_BOTH) {
            Append(2, data);
            tcp_info_handler_[1]->write(buffer);
        }
        if (tcp_info_handler_[2]->tcp_mode_ == TCPInfoHandle::TCP_INFO_MODE_BOTH) {
            Append(3, data);
            tcp_info_handler_[2]->write(buffer);
        }
    } else if (ui_->channelToSend->currentIndex() == 1) {
        Append(2, data);
        tcp_info_handler_[1]->write(buffer);
    } else if (ui_->channelToSend->currentIndex() == 2) {
        Append(3, data);
        tcp_info_handler_[2]->write(buffer);
    }

    send_count_ = send_count_ + buffer.size();
    ui_->btnSendCount->setText(QString("发送 : %1 字节").arg(send_count_));
}

void TCPCom::SaveData() {
    QString temp_data = ui_->txtMain->toPlainText();
    if (temp_data.isEmpty()) {
        return;
    }

    QDateTime now = QDateTime::currentDateTime();
    QString name = now.toString("yyyy-MM-dd-HH-mm-ss");
    QString file_name = QString("%1/%2.txt").arg(QuiHelper::AppPath(), name);

    QFile file(file_name);
    file.open(QFile::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    out << temp_data;
    file.close();

    on_btnClear_clicked();
}

void TCPCom::on_btnData_clicked() {
    QString file_name = QString("%1/%2").arg(QuiHelper::AppPath()).arg("send.txt");
    QFile file(file_name);
    if (!file.exists()) {
        return;
    }

    if (ui_->btnData->text() == "管理数据") {
        ui_->txtMain->setReadOnly(false);
        ui_->txtMain->clear();
        file.open(QFile::ReadOnly | QIODevice::Text);
        QTextStream in(&file);
        ui_->txtMain->setText(in.readAll());
        file.close();
        ui_->btnData->setText("保存数据");
    } else {
        ui_->txtMain->setReadOnly(true);
        file.open(QFile::WriteOnly | QIODevice::Text);
        QTextStream out(&file);
        out << ui_->txtMain->toPlainText();
        file.close();
        ui_->txtMain->clear();
        ui_->btnData->setText("管理数据");
        AppData::ReadSendData();
    }
}

void TCPCom::on_btnClear_clicked() {
    Append(0, "", true);
}

void TCPCom::GetConstructConfig() {
    cfg_->beginGroup(group_name_);

    cfg_->endGroup();

}

void TCPCom::SaveConstructConfig() {
    cfg_->beginGroup(group_name_);

    cfg_->endGroup();
}
void TCPCom::UpdateSendHistory() {
    ui_->historyTable->setRowCount(0);
    ui_->historyTable->clearContents();
    ui_->historyTable->setSortingEnabled(false);
    int j = 0;
    for (QHash<QString, HistorySend>::iterator i = history_send_list_.begin(); i != history_send_list_.end(); ++i) {
        ui_->historyTable->insertRow(j);
        HistorySend tmp = i.value();
        ui_->historyTable->setItem(j, 0, new QTableWidgetItem(tmp.data));
        ui_->historyTable->setItem(j, 1, new QTableWidgetItem(tmp.time.toString("yyyy-MM-dd HH:mm:ss")));
        ui_->historyTable->setItem(j, 2, new QTableWidgetItem(QString::number(tmp.send_num)));
        if (tmp.is_Hex) {
            ui_->historyTable->setItem(j, 3, new QTableWidgetItem("hex"));
        } else {
            ui_->historyTable->setItem(j, 3, new QTableWidgetItem("str"));
        }
        j++;
    }
    ui_->historyTable->setSortingEnabled(true);
}
