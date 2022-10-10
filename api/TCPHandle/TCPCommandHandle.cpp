//
// Created by xtx on 2022/9/28.
//

#include "TCPCommandHandle.h"

TCPCommandHandle::TCPCommandHandle(QObject *parent) : QTcpSocket(parent) {
    heartTimer = new QTimer(this);
    connect(heartTimer, &QTimer::timeout, this, [=] {
        this->SendHeart();//初始化定时器
    });
}

void TCPCommandHandle::connectToHost(const QString &hostName, quint16 port, QIODevice::OpenMode protocol = ReadWrite,
                                     QAbstractSocket::NetworkLayerProtocol mode = AnyIPProtocol) {
    connect(this, &QTcpSocket::connected, this, [=] {
        disconnect(this, &QTcpSocket::connected, 0, 0);
        isConnected = true;
        isFirstHeart = true;
        emit(hasConnected());
        heartTimer->start(3000);//启动定时心跳
    });
    this->IP = hostName;
    this->QAbstractSocket::connectToHost(hostName, port, protocol, mode);

}

void TCPCommandHandle::disconnectFromHost() {
    connect(this, &QTcpSocket::disconnected, this, [=] {
        disconnect(this, &QTcpSocket::disconnected, 0, 0);
        isConnected = false;
        heartTimer->stop();//关闭心跳包发送
    });
    QAbstractSocket::disconnectFromHost();
}

void TCPCommandHandle::SendHeart() {
    if (!isConnected) {
        qDebug() << "没有有效连接";
        return;
    }
    QTimer::singleShot(2000, this, [=] {
        if (isHeartRec) {
            isHeartRec = false;//如果已经收到了心跳返回包，则不处理
        }
        else {//没有收到心跳返回包，超时了
            this->disconnectFromHost();
            disconnect(this, &QTcpSocket::readyRead, 0, 0);
        }
    });
    connect(this, &QTcpSocket::readyRead, this, [=] {
        QByteArray t2 = this->read(1024);
        if (t2.length() == 5 && t2 == "OK!\r\n") {
            //读取到心跳返回包
            disconnect(this, &QTcpSocket::readyRead, 0, 0);
            isHeartRec = true;
            if (isFirstHeart) {//是第一次收到心跳返回包，发送信号
                isFirstHeart = false;
                emit(receiveFirstHeart());
            }
        }
    });
    this->write("COM\r\n", 5);//心跳包
}


void TCPCommandHandle::setMode(int mode) {
    if (!isConnected) {
        qDebug() << "没有有效连接";
        return;
    }
    heartTimer->stop();//关闭心跳包发送，防止误传
    QTimer::singleShot(10000, this, [=] {
        if (!isModeSet) {//超时，自动断开
            this->disconnectFromHost();
            disconnect(this, &QTcpSocket::connected, 0, 0);
            disconnect(this, &QTcpSocket::readyRead, 0, 0);
        }
    });
    connect(this, &QTcpSocket::readyRead, this, [=] {
        //此处的包是模式设置返回包，收到该包后调试器应当重启
        QByteArray t2 = this->read(1024);
        if (t2.length() == 5 && t2 == "OK!\r\n") {
            disconnect(this, &QTcpSocket::readyRead, 0, 0);

            connect(this, &QTcpSocket::disconnected, this, [=] {
                disconnect(this, &QTcpSocket::disconnected, 0, 0);
                isConnected = false;
                heartTimer->stop();//关闭心跳包发送
                this->WaitForMode(mode);
            });
            emit(readyReboot());//发送准备重启的信号
            this->QAbstractSocket::disconnectFromHost();//此处也直接断开，使用原生方法而不是覆写方法，保证时序

        }
    });
    char tmp[100];
    sprintf(tmp, R"("{"command":101,"attach":"%d"}")", mode);
    this->write(tmp);

}

void TCPCommandHandle::WaitForMode(int mode) {
    //此处不使用重构方法，防止先收到心跳返回包
    this->QAbstractSocket::connectToHost(IP, 1920, QAbstractSocket::ReadWrite, QAbstractSocket::AnyIPProtocol);
    connect(this, &QTcpSocket::connected, this, [=] {

        disconnect(this, &QTcpSocket::connected, 0, 0);
        isConnected = true;
        isFirstHeart = true;
        connect(this, &QTcpSocket::readyRead, this, [=] {
            //接收到模式切换包
            QByteArray t2 = this->read(1024);
            char tmp[20];
            sprintf(tmp, "RF%d\r\n", mode);
            if (t2.length() == 5 && t2 == tmp) {
                disconnect(this, &QTcpSocket::readyRead, 0, 0);

                emit(ModeChangeSuccess());//发送模式切换成功信号
                isHeartRec = false;
                isModeSet = true;//完成模式设置
                this->SendHeart();//发送一个心跳包
                heartTimer->start(3000);//启动定时心跳
            }
        });
    });

}


