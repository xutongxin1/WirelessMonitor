//
// Created by xtx on 2022/11/1.
//

#include "TCPInfoHandle.h"

TCPInfoHandle::TCPInfoHandle(QObject *parent) : QTcpSocket(parent) {
//    this->isReadOnly=isReadOnly;

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
        disconnect(this, &QTcpSocket::connected, this, 0);
        isConnected = true;
        emit(hasConnected());
    });
    this->IP = hostName;
    this->IOPort = port - 1920;
    this->QAbstractSocket::connectToHost(hostName, port, protocol, mode);

}

bool TCPInfoHandle::enableRecEmit() {
    if (this->isConnected && this->TCPMode == TCPInfoMode_IN) {
        disconnect(this, &QTcpSocket::readyRead, 0, 0);
        connect(this, &QTcpSocket::readyRead, this, [&] {
            QByteArray buffer = this->readAll();
            emit(RecNewData(buffer, IP, IOPort, QTime::currentTime()));
        });
        return true;
    }
    else {
        return false;
    }
}

void TCPInfoHandle::changeTCPInfoMode(TCPInfoHandle::TCPInfoMode mode) {
    TCPMode = mode;
    if (mode == TCPInfoMode_IN) {
        this->enableRecEmit();
    }
}

qint64 TCPInfoHandle::write(const QString &data) {
    if (TCPMode == TCPInfoMode_OUT) {
        return QIODevice::write(data.toUtf8());
    }
    else {
        return -1;
    }
}

qint64 TCPInfoHandle::write(const char *data, qint64 len) {
    if (TCPMode == TCPInfoMode_OUT) {
        return QIODevice::write(data, len);
    }
    else {
        return -1;
    }
}

qint64 TCPInfoHandle::write(const char *data) {
    if (TCPMode == TCPInfoMode_OUT) {
        return QIODevice::write(data);
    }
    else {
        return -1;
    }
}

qint64 TCPInfoHandle::write(const QByteArray &data) {
    if (TCPMode == TCPInfoMode_OUT) {
        return QIODevice::write(data);
    }
    else {
        return -1;
    }
}
