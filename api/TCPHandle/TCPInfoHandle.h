//
// Created by xtx on 2022/11/1.
//

#ifndef MAIN_API_TCPHANDLE_TCPINFOHANDLE_H_
#define MAIN_API_TCPHANDLE_TCPINFOHANDLE_H_

#include <QTcpSocket>
#include <QTime>

class TCPInfoHandle : public QTcpSocket {
 Q_OBJECT
 signals:

    void HasConnected();//防止与原生方法冲突
    void HasDisconnected();

    void RecNewData(QByteArray data, QString ip, int port, QDateTime time);

 public:
    TCPInfoHandle(QObject *parent = nullptr);

    bool is_connected_;

    void connectToHost(const QString &host_name, quint16 port, QIODevice::OpenMode protocol,
                       QAbstractSocket::NetworkLayerProtocol mode) override;

    void disconnectFromHost() override;

    qint64 write(const QString &data);

    qint64 write(const char *data, qint64 len);

    qint64 write(const char *data);

    virtual qint64 write(const QByteArray &data);

    virtual QByteArray read(qint64 maxlen);

    virtual QByteArray readAll();

    enum TCPInfoMode {
      TCP_INFO_MODE_NONE = 0,
      TCP_INFO_MODE_IN,
      TCP_INFO_MODE_OUT
    } tcp_mode_ = TCP_INFO_MODE_NONE;

    void ChangeTCPInfoMode(TCPInfoMode mode);

    bool EnableRecEmit();

 private:
    QString ip_;
    int io_port_;
};

#endif //MAIN_API_TCPHANDLE_TCPINFOHANDLE_H_
