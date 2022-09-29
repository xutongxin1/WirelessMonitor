//
// Created by xtx on 2022/9/28.
//

#include "TCPHandle.h"

void TCPHandle::WaitForOK() {
    QByteArray t1 = this->Command.toLatin1();
    connect(this, &QTcpSocket::readyRead, this, [=] {
        QByteArray t2 = this->read(1024);
        if (t2.length() == 5 && t2 == "OK!\r\n") {
            disconnect(this, &QTcpSocket::readyRead, 0, 0);
            isWorking = false;
            isFinishLastWork = true;
        }
    });
    this->write(t1);
}

void TCPHandle::connectToHost(const QString &hostName, quint16 port, QIODevice::OpenMode protocol = ReadWrite,
                              QAbstractSocket::NetworkLayerProtocol mode = AnyIPProtocol) {
    this->QAbstractSocket::connectToHost(hostName, port, protocol, mode);
    connect(this, &QTcpSocket::connected, this, [=] {
        isConnected = true;
        this->SendCommand("Hello");
    });
}

void TCPHandle::SendCommand(QString Command) {
    this->Command = Command;
    if (!isConnected) {
        qDebug() << "没有有效连接";
        return;
    }
    isWorking = true;
    connect(this, &QTcpSocket::readyRead, this, [=] {
        QByteArray t2 = this->read(1024);
        if (t2.length() == 5 && t2 == "OK!\r\n") {
            disconnect(this, &QTcpSocket::readyRead, 0, 0);
            WaitForOK();
        }
    });
    this->write("COM\r\n", 5);
}
