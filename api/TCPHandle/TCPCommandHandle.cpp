//
// Created by xtx on 2022/9/28.
//

#include "TCPCommandHandle.h"

//void TCPCommandHandle::WaitForOK() {
//    QByteArray t1 = this->Command.toLatin1();
//    connect(this, &QTcpSocket::readyRead, this, [=] {
//        QByteArray t2 = this->read(1024);
//        if (t2.length() == 5 && t2 == "OK!\r\n") {
//            disconnect(this, &QTcpSocket::readyRead, 0, 0);
//            isWorking = false;
//            isFinishLastWork = true;
//        }
//    });
//    this->write(t1);
//}

void TCPCommandHandle::connectToHost(const QString &hostName, quint16 port, QIODevice::OpenMode protocol = ReadWrite,
                                     QAbstractSocket::NetworkLayerProtocol mode = AnyIPProtocol) {
    this->QAbstractSocket::connectToHost(hostName, port, protocol, mode);
    connect(this, &QTcpSocket::connected, this, [=] {
        isConnected = true;
        heartTimer->start(3000);
    });
}

void TCPCommandHandle::disconnectFromHost() {
    connect(this, &QTcpSocket::disconnected, this, [=] {
        isConnected = false;
        heartTimer->stop();
    });
    QAbstractSocket::disconnectFromHost();
}

void TCPCommandHandle::SendHeart() {
    if (!isConnected) {
        qDebug() << "没有有效连接";
        return;
    }
    QTimer::singleShot(2000, this, [=] {
        if (isHeartRec) {
            isHeartRec = false;
        }
        else {
            this->disconnectFromHost();
        }
    });
    connect(this, &QTcpSocket::readyRead, this, [=] {
        QByteArray t2 = this->read(1024);
        if (t2.length() == 5 && t2 == "OK!\r\n") {
            disconnect(this, &QTcpSocket::readyRead, 0, 0);
            isHeartRec = true;
        }
    });
    this->write("COM\r\n", 5);
}

TCPCommandHandle::TCPCommandHandle(QObject *parent) : QTcpSocket(parent) {
    heartTimer = new QTimer(this);
    connect(heartTimer, &QTimer::timeout, this,[=]{
        this->SendHeart();
    });
}
