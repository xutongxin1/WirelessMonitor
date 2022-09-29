//
// Created by xtx on 2022/9/28.
//

#ifndef MAIN_TCPHANDLE_H
#define MAIN_TCPHANDLE_H


#include <QTcpSocket>
#include <QThread>

class TCPHandle : public QTcpSocket {
public:
    TCPHandle(QObject *parent = nullptr) {};
    bool isConnected;
    bool isWorking;
    bool isFinishLastWork;

    void SendCommand(QString Command);

    bool isFinishWork();

    void connectToHost(const QString &hostName, quint16 port, QIODevice::OpenMode protocol ,
                       QAbstractSocket::NetworkLayerProtocol mode) override;

private:
    QString Command;

    void WaitForOK();
//    void WaitSecondOK();


};


#endif //MAIN_TCPHANDLE_H
