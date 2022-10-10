//
// Created by xtx on 2022/9/28.
//

#ifndef MAIN_TCPCOMMANDHANDLE_H
#define MAIN_TCPCOMMANDHANDLE_H


#include <QTcpSocket>
#include <QThread>
#include <QTimer>

class TCPCommandHandle : public QTcpSocket {
public:
    TCPCommandHandle(QObject *parent = nullptr) ;
    bool isConnected;
    bool isWorking;
    bool isFinishLastWork;

    void SendHeart();

    bool isFinishWork();

    void connectToHost(const QString &hostName, quint16 port, QIODevice::OpenMode protocol ,
                       QAbstractSocket::NetworkLayerProtocol mode) override;

    void disconnectFromHost() override;

private:
    QString Command;

    void WaitForOK();

    bool isHeartRec=false;

    QTimer *heartTimer;
//    void WaitSecondOK();


};


#endif //MAIN_TCPCOMMANDHANDLE_H
