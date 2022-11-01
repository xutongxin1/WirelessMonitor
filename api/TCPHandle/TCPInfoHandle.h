//
// Created by xtx on 2022/11/1.
//

#ifndef MAIN_TCPINFOHANDLE_H
#define MAIN_TCPINFOHANDLE_H


#include <QTcpSocket>

class TCPInfoHandle : public QTcpSocket {
Q_OBJECT
signals:

    void hasConnected();//防止与原生方法冲突
    void hasDisconnected();

public:
    TCPInfoHandle(QObject *parent = nullptr);

    bool isConnected;

    void connectToHost(const QString &hostName, quint16 port, QIODevice::OpenMode protocol,
                       QAbstractSocket::NetworkLayerProtocol mode) override;

    void disconnectFromHost() override;

    void changeReadOnly(char isReadOnly) { this->isReadOnly = isReadOnly; };

private:
    QString IP;
    char isReadOnly;//0关闭,1读取,2写入
};


#endif //MAIN_TCPINFOHANDLE_H
