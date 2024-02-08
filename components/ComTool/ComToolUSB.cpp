//
// Created by xtx on 24-2-8.
//

#include <QMessageBox>

#include "Comtool.h"
#include "enumser.h"


QSet<QString> ComTool::GetPortInfo() {
    QSet<QString> serialportinfo;
    foreach(QSerialPortInfo portInfo, QSerialPortInfo::availablePorts()) {
        // qDebug() << "\n"
        //         << "Port:" << portInfo.portName() << "\n"
        //         << "Location:" << portInfo.systemLocation() << "\n"
        //         << "Description:" << portInfo.description() << "\n"
        //         << "Manufacturer:" << portInfo.manufacturer() << "\n"
        //         << "Serial number:" << portInfo.serialNumber() << "\n"
        //         << "Vendor Identifier:"
        //         << (portInfo.hasVendorIdentifier()
        //                 ? QByteArray::number(portInfo.vendorIdentifier(), 16)
        //                 : QByteArray()) << "\n"
        //         << "Product Identifier:"
        //         << (portInfo.hasProductIdentifier()
        //                 ? QByteArray::number(portInfo.productIdentifier(), 16)
        //                 : QByteArray());
        serialportinfo << (portInfo.portName() + " : " + portInfo.description());
    }
    // ui->comboBox->addItems(serialportinfo);
    return serialportinfo;
}

void ComTool::ReflashComCombo() {
    timer_for_port_->stop();
    QSet<QString> old_portinfo = my_serialportinfo_;

    my_serialportinfo_ = GetPortInfo();
    // qDebug() << my_serialportinfo_;
    QString com = GetNowTrueComName();
    if (old_portinfo != my_serialportinfo_) //串口列表出现变化
    {
        ui_->COMCombo->clear(); //清空列表
        UpdateComNameAndSymbolic(); //更新usb信息列表

        if (my_serialport_->isOpen()) //有串口打开的时候
        {
            //保证正在打开的不会变，任然在第一位
            foreach(QString comname, my_serialportinfo_) {
                if (GetTrueComName(comname) == my_serialport_->portName()) {
                    COMComboAddItem(comname);
                }
            }

            //然后再添加剩下的
            foreach(QString comname, my_serialportinfo_) {
                if (GetTrueComName(comname) != my_serialport_->portName()) {
                    COMComboAddItem(comname);
                }
            }
        } else //无串口打开的时候
        {
            foreach(QString comname, my_serialportinfo_) {
                COMComboAddItem(comname);
            }
        }

        //串口是开启状态，但是事实上串口已经丢失
        if (!my_serialportinfo_.contains(com) && my_serialport_->isOpen()) {
            QMessageBox::critical(this, tr("Error"), "串口连接中断，请检查是否正确连接！");
            my_serialport_->close();
            ui_->COMCombo->removeItem(ui_->COMCombo->currentIndex());
            ui_->StartTool->setText("启动");
            is_start_ = false;
            ui_->StartTool->setStyleSheet("background-color: rgba(170, 255, 0, 125);");
            ui_->COMButton->setEnabled(true);
            ui_->TCPClientButton->setEnabled(true);
            ui_->TCPServerButton->setEnabled(true);
        }
    }

    timer_for_port_->start();
}


QString ComTool::GetNowTrueComName() {
    QStringList strList = ui_->COMCombo->currentText().split(" : ");
    return strList[0];
}

QString ComTool::GetTrueComName(QString tmp) {
    return tmp.split(" : ")[0];
}

void ComTool::COMComboAddItem(QString Name) {
    ui_->COMCombo->addItem(Name);

    Name = GetTrueComName(Name);
    ui_->COMCombo->setItemData(ui_->COMCombo->count() - 1, ComNameAndSymbolic.value(Name, "error"), Qt::ToolTipRole);
}

void ComTool::ComboChange(int num) {
    ComUUID = ComNameAndSymbolic.value(GetTrueComName(ui_->COMCombo->currentText()), "error");
    ComUUID = QCryptographicHash::hash(ComUUID.toUtf8(), QCryptographicHash::Md5).toHex();
    GetConstructConfig();
}

void ComTool::UpdateComNameAndSymbolic() {
    ComNameAndSymbolic.clear();

    // USBWork();
    CEnumerateSerial::CPortsArray ports;
    CEnumerateSerial::CPortAndNamesArray portAndSymbolic;
    CEnumerateSerial::CNamesArray names;
    if (CEnumerateSerial::UsingSetupAPI2(portAndSymbolic)) {
        for (const auto &port: portAndSymbolic)
#pragma warning(suppress: 26489)
            // qDebug() << "COM" << QString::number(port.first) << " " << QString::fromStdWString(port.second);
            ComNameAndSymbolic.insert("COM" + QString::number(port.first),
                                      QString::fromStdWString(port.second));
    } else {
        qCritical() << "UsingSetupAPI2 to UpdateComNameAndSymbolic failed";
    }
}
