//
// Created by xtx on 2022/9/28.
//

#include <QMessageBox>
#include "TCPCommandHandle.h"

TCPCommandHandle::TCPCommandHandle(QObject *parent) : QTcpSocket(parent) {
    heartTimer = new QTimer(this);
    connect(heartTimer, &QTimer::timeout, this, [&] {
        this->SendHeart();//初始化定时器
    });
}

void TCPCommandHandle::connectToHost(const QString &hostName, quint16 port, QIODevice::OpenMode protocol = ReadWrite,
                                     QAbstractSocket::NetworkLayerProtocol mode = AnyIPProtocol) {
    connect(this, &QTcpSocket::connected, this, [=] {
        qInfo("已连接到服务器%s:%d", qPrintable(hostName), port);
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
        qInfo("从服务器断开%s", qPrintable(this->IP));
        disconnect(this, &QTcpSocket::disconnected, 0, 0);
        isConnected = false;
        heartTimer->stop();//关闭心跳包发送
        emit(hasDisconnected());
    });
    QAbstractSocket::disconnectFromHost();
}

void TCPCommandHandle::SendHeart() {
    qDebug() << "SendHeart";
    if (!isConnected) {
        qDebug() << "没有有效连接";
        return;
    }
    isHeartRec = false;
    QTimer::singleShot(2000, this, [&] {
        if (isHeartRec) {
            isHeartRec = false;//如果已经收到了心跳返回包，则不处理
            HeartErrorTime = 0;
        }
        else {//没有收到心跳返回包，超时了
            if (++HeartErrorTime == 3) {
                qCritical("心跳包3次错误");
                HeartErrorTime = 0;
                heartTimer->stop();//关闭心跳包发送
                emit(heartError());
                this->disconnectFromHost();
                disconnect(this, &QTcpSocket::readyRead, 0, 0);
            }
        }
    });
    connect(this, &QTcpSocket::readyRead, this, [&] {
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
    this->write("COM\r\n");//心跳包
}


void TCPCommandHandle::setMode(int mode) {
    if (!isConnected) {
        qDebug() << "没有有效连接";
        return;
    }
    heartTimer->stop();//关闭心跳包发送，防止误传
    QTimer::singleShot(10000, this, [=] {
        if (!isModeSet) {//设置超时，自动断开.设置成功置位在收到第一个包后
            qCritical("模式设置超时");
            emit(setModeError());
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
            heartTimer->stop();//关闭心跳包发送
            emit(readyReboot());//发送准备重启的信号
            connect(this, &QTcpSocket::disconnected, this, [=] {
                disconnect(this, &QTcpSocket::disconnected, 0, 0);
                isConnected = false;
            });
            QAbstractSocket::disconnectFromHost();

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
        isHeartRec = true;
        disconnect(this, &QTcpSocket::connected, 0, 0);
        isConnected = true;
        isFirstHeart = true;
        connect(this, &QTcpSocket::readyRead, this, [&, mode] {
            //接收到模式切换包
            QByteArray t2 = this->read(1024);
            char tmp[20];
            sprintf(tmp, "RF%d\r\n", mode);
            if (t2.length() == 5 && t2 == tmp) {
                disconnect(this, &QTcpSocket::readyRead, 0, 0);

                emit(ModeChangeSuccess());//发送模式切换成功信号
                isModeSet = true;//完成模式设置的置位
//                this->SendHeart();//发送一个心跳包
                heartTimer->start(3000);//启动定时心跳
            }
        });
    });

}

void TCPCommandHandle::SendCommand(QJsonObject command, QString reply) {
    TCPCommandHandle::SendCommand(getStringFromJsonObject(command), reply);
}

void TCPCommandHandle::SendCommand(QString command, QString reply) {
    hasReceiveReply = false;

    heartTimer->stop();
    connect(this, &QTcpSocket::readyRead, this, [&, reply] {
                QByteArray t2 = this->read(1024);
                if (t2 == reply) {
                    //读取到心跳返回包
                    disconnect(this, &QTcpSocket::readyRead, 0, 0);
                    hasReceiveReply = true;
                    emit(sendCommandSuccess());
                    emit(startInfoConnection());
                    heartTimer->start(3000);
                }
            }
    );
    QTimer::singleShot(5000, this, [&] {
        if (!hasReceiveReply) {
            emit(sendCommandError());
        }
    });
    this->write(command.toLatin1());
}


QString TCPCommandHandle::getStringFromJsonObject(const QJsonObject &jsonObject) {
    return QString(QJsonDocument(jsonObject).toJson());
}

bool TCPCommandHandle::getConnectionState() const {
    return isConnected;
}
