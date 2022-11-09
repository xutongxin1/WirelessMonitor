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

    enum TCPInfoMode {
        TCPInfoMode_None = 0,
        TCPInfoMode_IN,
        TCPInfoMode_OUT,
    };

    void changeTCPInfoMode(TCPInfoMode mode) { TCPMode = mode; };


private:
    QString IP;
    TCPInfoMode TCPMode;
};


#endif //MAIN_TCPINFOHANDLE_H
