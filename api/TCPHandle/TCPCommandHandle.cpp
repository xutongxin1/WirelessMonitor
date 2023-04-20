//
// Created by xtx on 2022/9/28.
//

#include <QMessageBox>
#include "TCPCommandHandle.h"

TCPCommandHandle::TCPCommandHandle(QObject *parent) : QTcpSocket(parent) {
    heart_timer_ = new QTimer(this);
    connect(heart_timer_, &QTimer::timeout, this, [&] {
      this->SendHeart();//初始化定时器
    });
    send_command_timer_ = new QTimer(this);

    //绑定send_command_timer_的超时事件
    //该超时函数不会考虑断开整个连接，而只是重启心跳包发送。如果没有响应，由心跳包超时器负责

    //解绑可能的错误响应
    disconnect(this, &QTcpSocket::readyRead, nullptr, nullptr);

    connect(send_command_timer_, &QTimer::timeout, this, [&] {
      send_command_timer_->stop();
      emit(SendCommandError());
//      if (!has_receive_reply_) {
//          emit(SendCommandError());
//      }
    });
}

///重写连接服务器
void TCPCommandHandle::connectToHost(const QString &host_name, quint16 port, QIODevice::OpenMode protocol = ReadWrite,
                                     QAbstractSocket::NetworkLayerProtocol mode = AnyIPProtocol) {
    connect(this, &QTcpSocket::connected, this, [=] {
        qInfo("已连接到服务器%s:%d", qPrintable(host_name), port);
        disconnect(this, &QTcpSocket::connected, nullptr, nullptr);
        is_connected_ = true;
        is_first_heart_ = true;
        emit(HasConnected());
        heart_timer_->start(3000);//启动定时心跳
    });
    this->ip_ = host_name;
    this->QAbstractSocket::connectToHost(host_name, port, protocol, mode);

}

///重写断开服务器
void TCPCommandHandle::disconnectFromHost() {
    connect(this, &QTcpSocket::disconnected, this, [=] {
        qInfo("从服务器断开%s", qPrintable(this->ip_));
        disconnect(this, &QTcpSocket::disconnected, nullptr, nullptr);
        is_connected_ = false;
        heart_timer_->stop();//关闭心跳包发送
        emit(HasDisconnected());
    });
    QAbstractSocket::disconnectFromHost();
}

/// 发送心跳包
void TCPCommandHandle::SendHeart() {
    qDebug() << "SendHeart";
    if (!is_connected_) {
        qDebug() << "没有有效连接";
        return;
    }
    is_heart_rec_ = false;
    QTimer::singleShot(2000, this, [&] {
        if (is_heart_rec_) {
            is_heart_rec_ = false;//如果已经收到了心跳返回包，则不处理
            heart_error_time_ = 0;
        }
        else {//没有收到心跳返回包，超时了
            if (++heart_error_time_ == 3) {
                qCritical("心跳包3次错误");
                heart_error_time_ = 0;
                heart_timer_->stop();//关闭心跳包发送
                emit(HeartError());
                this->disconnectFromHost();
                disconnect(this, &QTcpSocket::readyRead, nullptr, nullptr);
            }
        }
    });
    disconnect(this, &QTcpSocket::readyRead, nullptr, nullptr);//理论上可以不加的，但不加第二次连接会炸
    connect(this, &QTcpSocket::readyRead, this, [&] {
      QByteArray t_2 = this->read(1024);
      if (t_2.length() == 5 && t_2 == "OK!\r\n") {
          //读取到心跳返回包
          disconnect(this, &QTcpSocket::readyRead, nullptr, nullptr);
          is_heart_rec_ = true;
          if (is_first_heart_) {//是第一次收到心跳返回包，发送信号
              is_first_heart_ = false;
              emit(ReceiveFirstHeart());
          }
      }
    });
    this->write("COM\r\n");//心跳包
}

/// 设置模式
/// \param mode 设置的模式
void TCPCommandHandle::SetMode(int mode) {
    if (!is_connected_) {
        qDebug() << "没有有效连接";
        return;
    }
    heart_timer_->stop();//关闭心跳包发送，防止误传
    QTimer::singleShot(10000, this, [=] {
        if (!is_mode_set_) {//设置超时，自动断开.设置成功置位在收到第一个包后
            qCritical("模式设置超时");
            emit(SetModeError());
            this->disconnectFromHost();
            disconnect(this, &QTcpSocket::connected, nullptr, nullptr);
            disconnect(this, &QTcpSocket::readyRead, nullptr, nullptr);
        }
    });
    disconnect(this, &QTcpSocket::disconnected, nullptr, nullptr);
    connect(this, &QTcpSocket::disconnected, this, [=] {
      disconnect(this, &QTcpSocket::disconnected, nullptr, nullptr);
      qDebug("%s断开连接", qPrintable(ip_));
      is_connected_ = false;
      heart_timer_->stop();//关闭心跳包发送
      this->WaitForMode(mode);
    });//这里选择放在外面是因为服务端会先关闭
    char sf[10];
    sprintf(sf, "SF%d\r\n", mode);
    connect(this, &QTcpSocket::readyRead, this, [&, sf] {
      //此处的包是模式设置返回包，收到该包后调试器应当重启
      QByteArray t_2 = this->read(1024);
      if (t_2.length() == 5 && t_2 == "OK!\r\n") {
          disconnect(this, &QTcpSocket::readyRead, nullptr, nullptr);
          heart_timer_->stop();//关闭心跳包发送
          emit(ReadyReboot());//发送准备重启的信号
          qDebug("准备断开连接");
          QAbstractSocket::disconnectFromHost();
      } else if (t_2.length() == 5 && strncmp(sf, t_2.data(), strlen(sf)) == 0) {
          disconnect(this, &QTcpSocket::readyRead, nullptr, nullptr); //快速启动
          emit(ReceiveFirstHeart());
          is_mode_set_ = true;
          this->SendHeart();//发送一个心跳包
          heart_timer_->start(3000);//启动定时心跳
      }
    });
    char tmp[100];
    sprintf(tmp, R"({"command":101,"attach":"%d"})", mode);

    this->write(tmp);
}

/// 等待开机回复的RF消息
/// \param mode 期望模式
void TCPCommandHandle::WaitForMode(int mode) {
    //此处不使用重构方法，防止先收到心跳返回包
    this->QAbstractSocket::connectToHost(ip_, 1920, QAbstractSocket::ReadWrite, QAbstractSocket::AnyIPProtocol);
    connect(this, &QTcpSocket::connected, this, [=] {
        qInfo("已连接到服务器%s:%d", qPrintable(ip_), 1920);
        is_heart_rec_ = true;
        disconnect(this, &QTcpSocket::connected, nullptr, nullptr);
        is_connected_ = true;
        is_first_heart_ = true;
        connect(this, &QTcpSocket::readyRead, this, [&, mode] {
            //接收到模式切换包
            QByteArray t_2 = this->read(1024);
            char tmp[20];
            sprintf(tmp, "RF%d\r\n", mode);
            if (t_2.length() == 5 && t_2 == tmp) {
                disconnect(this, &QTcpSocket::readyRead, nullptr, nullptr);

                emit(ModeChangeSuccess());//发送模式切换成功信号
                is_mode_set_ = true;//完成模式设置的置位
//                this->SendHeart();//发送一个心跳包

                //快发第一个包
                QTimer::singleShot(1000, this, [&] {
                    this->SendHeart();//发送一个心跳包
                    heart_timer_->start(3000);//启动定时心跳
                });
            }
        });
    });

}

/// 发送指令并绑定回复
/// \param command 指令
/// \param reply 回复
void TCPCommandHandle::SendCommand(const QJsonObject &command, const QString &reply) {
    QJsonDocument json_doc;
    json_doc.setObject(command);
    TCPCommandHandle::SendCommand(json_doc.toJson(QJsonDocument::Compact), reply);
}

/// 发送指令并绑定回复
/// \param command 指令
/// \param reply 回复
void TCPCommandHandle::SendCommand(const QByteArray &command, const QString &reply) {
//    has_receive_reply_ = false;

    heart_timer_->stop();
    connect(this, &QTcpSocket::readyRead, this, [&, reply] {
              QByteArray t_2 = this->read(1024);
              if (t_2 == reply) {
                  //读取到心跳返回包
                  disconnect(this, &QTcpSocket::readyRead, nullptr, nullptr);
//                    has_receive_reply_ = true;
                  send_command_timer_->stop();
                  emit(SendCommandSuccess());
                  emit(StartInfoConnection());
                  heart_timer_->start(3000);
              }
            }
    );
    send_command_timer_->start(5000);
    this->write(command);
}

/// 从JsonObject提取为QString
/// \param json_object 输入的JsonObject
/// \return 返回的字符串
[[maybe_unused]] QString TCPCommandHandle::GetStringFromJsonObject(const QJsonObject &json_object) {
    return QString(QJsonDocument(json_object).toJson());
}

/// 获取连接状态
/// \return 连接状态
bool TCPCommandHandle::GetConnectionState() const {
    return is_connected_;
}

//以下均为重写
qint64 TCPCommandHandle::write(const char *data, qint64 len) {
    qDebug("send %s", data);
    return QIODevice::write(data, len);
}

qint64 TCPCommandHandle::write(const char *data) {
    qDebug("send %s", data);
    return QIODevice::write(data);
}

qint64 TCPCommandHandle::write(const QByteArray &data) {
    qDebug("send %s", qPrintable(data));
    return QIODevice::write(data);
}

QByteArray TCPCommandHandle::read(qint64 maxlen) {
    QByteArray data = QIODevice::read(maxlen);
    qDebug("read %s", qPrintable(data));
    return data;
}

[[maybe_unused]] QByteArray TCPCommandHandle::readAll() {
    QByteArray data = QIODevice::readAll();
    qDebug("read %s", qPrintable(data));
    return data;
}

