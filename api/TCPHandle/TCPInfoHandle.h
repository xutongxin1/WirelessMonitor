//
// Created by xtx on 2022/11/1.
//

#ifndef MAIN_TCPINFOHANDLE_H
#define MAIN_TCPINFOHANDLE_H


#include <QTcpSocket>
#include <QTime>

class TCPInfoHandle : public QTcpSocket {
Q_OBJECT
signals:

    void hasConnected();//防止与原生方法冲突
    void hasDisconnected();

    void RecNewData(QByteArray data, QString ip, int port, QTime time);

public:
    TCPInfoHandle(QObject *parent = nullptr);

    bool isConnected;

    void connectToHost(const QString &hostName, quint16 port, QIODevice::OpenMode protocol,
                       QAbstractSocket::NetworkLayerProtocol mode) override;

    void disconnectFromHost() override;

    qint64 write(const QString &data);

    qint64 write(const char *data, qint64 len);

    qint64 write(const char *data);

    virtual qint64 write(const QByteArray &data);

    enum TCPInfoMode {
        TCPInfoMode_None = 0,
        TCPInfoMode_IN,
        TCPInfoMode_OUT,
    };

    void changeTCPInfoMode(TCPInfoMode mode);


    bool enableRecEmit();

    TCPInfoMode TCPMode;
private:
    QString IP;
    int IOPort;
};


#endif //MAIN_TCPINFOHANDLE_H
