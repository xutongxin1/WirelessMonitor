//
// Created by xtx on 2022/9/28.
//

#ifndef MAIN_TCPCOMMANDHANDLE_H
#define MAIN_TCPCOMMANDHANDLE_H


#include <QTcpSocket>
#include <QThread>
#include <QTimer>

class TCPCommandHandle : public QTcpSocket {
    Q_OBJECT
signals:
    void hasConnected();//防止与原生方法冲突
    void receiveFirstHeart();
    void readyReboot();
    void ModeChangeSuccess();
    void hasDisconnected();
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

    void setMode(int mode);

private:
    QString Command;

    QString IP;

    void WaitForMode(int mode);

    bool isHeartRec=false;

    bool isFirstHeart=false;

    bool isModeSet=false;

    QTimer *heartTimer;
//    void WaitSecondOK();


};


#endif //MAIN_TCPCOMMANDHANDLE_H
