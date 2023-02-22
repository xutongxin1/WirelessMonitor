//
// Created by xtx on 2022/9/16.
//

// You may need to build the project (run Qt uic code generator) to get "ui_ComTool.h" resolved

#include "ComTool.h"
#include "ui_comtool.h"
#include "quihelper.h"
#include "quihelperdata.h"



/*
 * TODO:shell语法高亮，使用正则表达式https://c.runoob.com_/front-end/
 * TODO:以回车分隔
 * TODO:ui修改
 */
ComTool::ComTool(int device_num, int win_num, QSettings *cfg, ToNewWidget *parent_info, QWidget *parent) :
    RepeaterWidget(parent), ui_(new Ui::ComTool) {
    this->cfg_ = cfg;
    this->config_file_path_ = "./config/Device" + QString::number(device_num) + ".ini";

    this->group_name_ = "Win" + QString::number(win_num);

    timer_for_port_=new QTimer(this);

    QuiHelper::InitAll();
    ComTool::GetConstructConfig();

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
      if (!tcp_info_handler_[3]->is_connected_) {
          tcp_info_handler_[3]->connectToHost(ip_, 1923, QAbstractSocket::ReadWrite, QAbstractSocket::AnyIPProtocol);
      }

      //默认设置模式
      connect(tcp_info_handler_[3], &QTcpSocket::connected, this, [&] {
        tcp_info_handler_[1]->ChangeTCPInfoMode(TCPInfoHandle::TCP_INFO_MODE_IN);
        tcp_info_handler_[2]->ChangeTCPInfoMode(TCPInfoHandle::TCP_INFO_MODE_IN);
        tcp_info_handler_[3]->ChangeTCPInfoMode(TCPInfoHandle::TCP_INFO_MODE_OUT);
        //数据接收绑定
        if (tcp_info_handler_[1]->tcp_mode_ == TCPInfoHandle::TCP_INFO_MODE_IN) {
            connect(tcp_info_handler_[1], &TCPInfoHandle::RecNewData, this,
                    [&](const QByteArray &data, const QString &ip, int port, QDateTime time) {
                      this->GetData(data, port);
                    });
        }
        if (tcp_info_handler_[2]->tcp_mode_ == TCPInfoHandle::TCP_INFO_MODE_IN) {
            connect(tcp_info_handler_[2], &TCPInfoHandle::RecNewData, this,
                    [&](const QByteArray &data, const QString &ip, int port, QDateTime time) {
                      this->GetData(data, port);
                    });
        }
      });

    });
#else
    ui_->btnStartTest->setHidden(true);
#endif

    QStringList baud_list;
    baud_list << QString::number(baud_rate_)
              << "600"
              << "1200"
              << "1800"
              << "2400"
              << "4800"
              << "9600"
              << "14400"
              << "19200"
              << "38400"
              << "56000"
              << "57600"
              << "76800"
              << "115200"
              << "128000"
              << "256000";

    ui_->BandCombo->addItems(baud_list);
    void (QComboBox::*fp)(int) = &QComboBox::currentIndexChanged;
    connect(ui_->BandCombo, fp, this, [&](int num) {
      baud_rate_ = ui_->BandCombo->currentText().toInt();
      UpdateComSetting();
    });

    QStringList data_bits_list;
    data_bits_list << "5"
                   << "6"
                   << "7"
                   << "8";

    ui_->DataBitCombo->addItems(data_bits_list);
    connect(ui_->DataBitCombo, fp, this, [&](int num) {
      data_bit_ = ui_->DataBitCombo->currentText().toInt();
      UpdateComSetting();
    });

    QStringList parity_list;
    parity_list << "无"
                << "奇"
                << "偶";
    ui_->ParityBitCombo->addItems(parity_list);
    connect(ui_->ParityBitCombo, fp, this, [&](int num) {
      parity_ = ui_->ParityBitCombo->currentIndex();
      UpdateComSetting();
    });

    // 初始化停止位表
    QStringList stop_bits_list;
    stop_bits_list << "1";
#ifdef Q_OS_WIN
    stop_bits_list << "1.5";
#endif
    stop_bits_list << "2";

    ui_->StopBitCombo->addItems(stop_bits_list);
    connect(ui_->StopBitCombo, fp, this, [&](int num) {
      stop_bit_ = ui_->StopBitCombo->currentText().toDouble();
      UpdateComSetting();
    });

    //扫描有效的端口
    timer_for_port_->start(500);

    connect(timer_for_port_,&QTimer::timeout,this, &ComTool::ReflashComCombo);







}

ComTool::~ComTool() {
    delete ui_;
}

QStringList ComTool::GetPortInfo()
{
    QStringList serialportinfo;
        foreach(QSerialPortInfo info,QSerialPortInfo::availablePorts())
        {
            serialportinfo<<info.portName();
        }
    // ui->comboBox->addItems(serialportinfo);
    return serialportinfo;
}

void ComTool::ReflashComCombo()
{
    timer_for_port_->stop();
    old_portinfo=my_serialportinfo;
    my_serialportinfo= Getportinfo();
    QString com=ui->comboBox->currentText();
    if(old_portinfo.length()!=my_serialportinfo.length())
    {
        ui->comboBox->clear();   //清空列表
        //说明串口列表出现变化,更新列表
        if(my_serialport->isOpen())        //有串口打开的时候
        {   //保证
            ui->comboBox->addItem(my_serialport->portName());
                foreach(QString comname, my_serialportinfo)
                {
                    if(comname!=my_serialport->portName())
                        ui->comboBox->addItem(comname);
                }
        }
        else                              //无串口打开的时候
        {
                foreach(QString comname, my_serialportinfo)
                {
                    ui->comboBox->addItem(comname);
                }

        }
        if(!my_serialportinfo.contains(com)&&my_serialport->isOpen())
        {
            QMessageBox::critical(this, tr("Error"), "串口连接中断，请检查是否正确连接！");
            closeserialport();
            ui->comboBox->removeItem(ui->comboBox->currentIndex());
        }
    }

    timerforport->start();
}



///初始化统计
void ComTool::InitForm() {
    sleep_time_rec_ = 10;
    receive_count_ = 0;
    send_count_ = 0;
    is_show_ = true;

    ui_->tabWidget->setCurrentIndex(0);
    ChangeEnable(false);

#ifdef __arm__
    ui_->widgetRight->setFixedWidth(280);
#endif
}

void ComTool::ChangeEnable(bool b) {

}

/// 往日志区添加数据
/// \param type 数据类型
/// \param data 数据
/// \param clear 是否清空
void ComTool::Append(int type, const QString &data, bool clear) {
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
//    str_data = str_data.replace("\r", "");
//    str_data = str_data.replace("\n", "");

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

    str_data = QString("时间[%1] %2 %3").arg(TIMEMS, str_type, str_data);
    ui_->txtMain->append(str_data);
    current_count++;
}

/// 数据收入处理
/// \param data 数据
/// \param port 端口
void ComTool::GetData(const QByteArray &data, int port) {

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
    ui_->ReceiveCount->setText(QString("接收 : %1 字节").arg(receive_count_));
}

///发送发送栏里的数据
void ComTool::SendData() {
    QString data = ui_->SendDataEdit->toPlainText();
    if (data.isEmpty()) {
        ui_->SendDataEdit->setFocus();
        return;
    }

    QByteArray buffer;
    if (ui_->ckHexSend->isChecked()) {
        buffer = QUIHelperData::hexStrToByteArray(data);
    } else {
        buffer = QUIHelperData::asciiStrToByteArray(data);
    }

    if (ui_->channelToSend->currentIndex() == 0) {
        if (tcp_info_handler_[2]->tcp_mode_ == TCPInfoHandle::TCP_INFO_MODE_OUT) {
            Append(2, data);
            tcp_info_handler_[2]->write(buffer);
        }
        if (tcp_info_handler_[3]->tcp_mode_ == TCPInfoHandle::TCP_INFO_MODE_OUT) {
            Append(3, data);
            tcp_info_handler_[3]->write(buffer);
        }
    } else if (ui_->channelToSend->currentIndex() == 1) {
        Append(2, data);
        tcp_info_handler_[2]->write(buffer);
    } else if (ui_->channelToSend->currentIndex() == 2) {
        Append(3, data);
        tcp_info_handler_[3]->write(buffer);
    }

    send_count_ = send_count_ + buffer.size();
    ui_->SendCount->setText(QString("发送 : %1 字节").arg(send_count_));
}

void ComTool::SaveData() {
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

void ComTool::on_btnClear_clicked() {
    Append(0, "", true);
}

void ComTool::GetConstructConfig() {
    cfg_->beginGroup(group_name_);

    cfg_->endGroup();

}

void ComTool::SaveConstructConfig() {
    cfg_->beginGroup(group_name_);

    cfg_->endGroup();
}

void ComTool::ReadErrorNet() {

}
void ComTool::UpdateComSetting() {

}
