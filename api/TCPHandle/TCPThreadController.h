//
// Created by xtx on 2022/9/29.
//

#ifndef MAIN_TCPTHREADCONTROLLER_H
#define MAIN_TCPTHREADCONTROLLER_H


#include <QObject>
#include <QThread>

class TCPThreadController : public QObject{
public:
    TCPThreadController(QObject *parent);
    bool isConnected;
    bool isSendingPackage;
    bool isFinishLastWork;
    QThread WorkThread;
};


#endif //MAIN_TCPTHREADCONTROLLER_H
