//
// Created by xtx on 2022/11/1.
//

#include "TCPInfoHandle.h"

TCPInfoHandle::TCPInfoHandle(QObject *parent) : QTcpSocket(parent) {


}

///断开服务器重写
void TCPInfoHandle::disconnectFromHost() {
    connect(this, &QTcpSocket::disconnected, this, [=] {
        qInfo("从服务器断开%s", qPrintable(this->IP));
        disconnect(this, &QTcpSocket::disconnected, 0, 0);
        isConnected = false;
        TCPMode = TCPInfoMode_None;
        emit(hasDisconnected());
    });
    QAbstractSocket::disconnectFromHost();
}

/// 连接服务器重写
void TCPInfoHandle::connectToHost(const QString &hostName, quint16 port, QIODevice::OpenMode protocol = ReadWrite,
                                  QAbstractSocket::NetworkLayerProtocol mode = AnyIPProtocol) {
    connect(this, &QTcpSocket::connected, this, [=] {
        qInfo("已连接到服务器%s:%d", qPrintable(hostName), port);
        disconnect(this, &QTcpSocket::connected, this, 0);
        isConnected = true;
        emit(hasConnected());
    });
    this->IP = hostName;
    this->IOPort = port - 1920;
    this->QAbstractSocket::connectToHost(hostName, port, protocol, mode);

}

/// 启动监听的connect
/// \return
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

///修改该通道的运行模式
/// \param mode
void TCPInfoHandle::changeTCPInfoMode(TCPInfoHandle::TCPInfoMode mode) {
    TCPMode = mode;
    if (mode == TCPInfoMode_IN) {
        this->enableRecEmit();
    }
    else {
        disconnect(this, &QTcpSocket::readyRead, 0, 0);
    }
}

//以下为保护性重写
qint64 TCPInfoHandle::write(const QString &data) {
    if (TCPMode == TCPInfoMode_OUT) {
        qDebug("send %s", qPrintable(data));
        return QIODevice::write(data.toUtf8());
    }
    else {
        return -1;
    }
}

qint64 TCPInfoHandle::write(const char *data, qint64 len) {
    if (TCPMode == TCPInfoMode_OUT) {
        qDebug("send %s", qPrintable(data));
        return QIODevice::write(data, len);
    }
    else {
        return -1;
    }
}

qint64 TCPInfoHandle::write(const char *data) {
    if (TCPMode == TCPInfoMode_OUT) {
        qDebug("send %s", qPrintable(data));
        return QIODevice::write(data);
    }
    else {
        return -1;
    }
}

qint64 TCPInfoHandle::write(const QByteArray &data) {
    if (TCPMode == TCPInfoMode_OUT) {
        qDebug("send %s", qPrintable(data));
        return QIODevice::write(data);
    }
    else {
        return -1;
    }
}

QByteArray TCPInfoHandle::read(qint64 maxlen) {
    QByteArray data = QIODevice::read(maxlen);
    qDebug("read %s", qPrintable(data));
    return data;
}

QByteArray TCPInfoHandle::readAll() {
    QByteArray data = QIODevice::readAll();
    qDebug("read %s", qPrintable(data));
    return data;
}
