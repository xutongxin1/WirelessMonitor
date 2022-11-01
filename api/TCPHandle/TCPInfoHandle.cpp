//
// Created by xtx on 2022/11/1.
//

#include "TCPInfoHandle.h"

TCPInfoHandle::TCPInfoHandle(QObject *parent) : QTcpSocket(parent) {
    this->isReadOnly=isReadOnly;

}


void TCPInfoHandle::disconnectFromHost() {
    connect(this, &QTcpSocket::disconnected, this, [=] {
        disconnect(this, &QTcpSocket::disconnected, 0, 0);
        isConnected = false;
        emit(hasDisconnected());
    });
    QAbstractSocket::disconnectFromHost();
}

void TCPInfoHandle::connectToHost(const QString &hostName, quint16 port, QIODevice::OpenMode protocol = ReadWrite,
                                     QAbstractSocket::NetworkLayerProtocol mode = AnyIPProtocol) {
    connect(this, &QTcpSocket::connected, this, [=] {
        disconnect(this, &QTcpSocket::connected, 0, 0);
        isConnected = true;
        emit(hasConnected());
    });
    this->IP = hostName;
    this->QAbstractSocket::connectToHost(hostName, port, protocol, mode);

}