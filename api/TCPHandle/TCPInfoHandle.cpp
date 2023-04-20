//
// Created by xtx on 2022/11/1.
//

#include "TCPInfoHandle.h"

TCPInfoHandle::TCPInfoHandle(QObject *parent) : QTcpSocket(parent) {


}

///断开服务器重写
void TCPInfoHandle::disconnectFromHost() {
    connect(this, &QTcpSocket::disconnected, this, [=] {
      qInfo("从服务器断开%s", qPrintable(this->ip_));
      disconnect(this, &QTcpSocket::disconnected, 0, 0);
      is_connected_ = false;
      tcp_mode_ = TCP_INFO_MODE_NONE;
      emit(HasDisconnected());
    });
    QAbstractSocket::disconnectFromHost();
}

/// 连接服务器重写
void TCPInfoHandle::connectToHost(const QString &host_name, quint16 port, QIODevice::OpenMode protocol = ReadWrite,
                                  QAbstractSocket::NetworkLayerProtocol mode = AnyIPProtocol) {
    connect(this, &QTcpSocket::connected, this, [=] {
      qInfo("已连接到服务器%s:%d", qPrintable(host_name), port);
      disconnect(this, &QTcpSocket::connected, this, 0);
      is_connected_ = true;
      emit(HasConnected());
    });
    this->ip_ = host_name;
    this->io_port_ = port - 1920;
    this->QAbstractSocket::connectToHost(host_name, port, protocol, mode);

}

/// 启动监听的connect
/// \return
bool TCPInfoHandle::EnableRecEmit() {
    if (this->tcp_mode_ != TCP_INFO_MODE_SEND) {
        disconnect(this, &QTcpSocket::readyRead, nullptr, nullptr);
        connect(this, &QTcpSocket::readyRead, this, [&] {
          QByteArray buffer = this->readAll();
          emit(RecNewData(buffer, ip_, io_port_, QDateTime::currentDateTime()));//忽视TCP可能的延迟
        });
        return true;
    } else {
        return false;
    }
}

///修改该通道的运行模式
/// \param mode
void TCPInfoHandle::ChangeTCPInfoMode(TCPInfoHandle::TCPInfoMode mode) {
    qDebug("port %d change mode to %d", io_port_, mode);
    tcp_mode_ = mode;
    if (mode == TCP_INFO_MODE_SEND) {
        disconnect(this, &QTcpSocket::readyRead, nullptr, nullptr);
    } else {
        this->EnableRecEmit();
    }
}

//以下为保护性重写
qint64 TCPInfoHandle::write(const QString &data) {
    if (tcp_mode_ != TCP_INFO_MODE_REC) {
        qDebug("send %s", qPrintable(data));
        return QIODevice::write(data.toUtf8());
    } else {
        return -1;
    }
}

qint64 TCPInfoHandle::write(const char *data, qint64 len) {
    if (tcp_mode_ != TCP_INFO_MODE_REC) {
        qDebug("send %s", qPrintable(data));
        return QIODevice::write(data, len);
    } else {
        return -1;
    }
}

qint64 TCPInfoHandle::write(const char *data) {
    if (tcp_mode_ != TCP_INFO_MODE_REC) {
        qDebug("send %s", qPrintable(data));
        return QIODevice::write(data);
    } else {
        return -1;
    }
}

qint64 TCPInfoHandle::write(const QByteArray &data) {
    if (tcp_mode_ != TCP_INFO_MODE_REC) {
        qDebug("send %s", qPrintable(data));
        return QIODevice::write(data);
    } else {
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
