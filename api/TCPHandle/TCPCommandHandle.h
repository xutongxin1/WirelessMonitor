//
// Created by xtx on 2022/9/28.
//

#ifndef MAIN_API_TCPHANDLE_TCPCOMMANDHANDLE_H_
#define MAIN_API_TCPHANDLE_TCPCOMMANDHANDLE_H_

#include <QTcpSocket>
#include <QThread>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>

class TCPCommandHandle : public QTcpSocket {
 Q_OBJECT
 signals:
    void HasConnected();//防止与原生方法冲突
    void ReceiveFirstHeart();
    void ReadyReboot();
    void ModeChangeSuccess();
    void HasDisconnected();
    void HeartError();
    void SetModeError();
    void WaitForModeError();
    void SendCommandSuccess();
    void SendCommandError();
    void StartInfoConnection();
    void StopInfoConnection();
 public:
    explicit TCPCommandHandle(QObject *parent = nullptr);
    bool is_connected_=false;
//    bool is_working_{};
//    bool is_finish_last_work_{};

    void SendHeart();

//    bool isFinishWork();

    void connectToHost(const QString &host_name, quint16 port, QIODevice::OpenMode protocol,
                       QAbstractSocket::NetworkLayerProtocol mode) override;

    void disconnectFromHost() override;

    void SetMode(int mode);

    void SendCommand(const QJsonObject& command, const QString& reply);
    void SendCommand(const QString& command,const QString& reply);

    static QString GetStringFromJsonObject(const QJsonObject &json_object);

    [[nodiscard]] bool GetConnectionState() const;

    QString ip_;
    virtual qint64 write(const char *data, qint64 len);

    virtual qint64 write(const char *data);

    virtual qint64 write(const QByteArray &data);

    virtual QByteArray read(qint64 maxlen);

    [[maybe_unused]] virtual QByteArray readAll();

//    bool has_receive_reply_ = false;

 private:
    QString command_;

    QTimer *send_command_timer_;

    int heart_error_time_ = 0;

    void WaitForMode(int mode);

    bool is_heart_rec_ = false;

    bool is_first_heart_ = false;

    bool is_mode_set_ = false;

    QTimer *heart_timer_;
//    void WaitSecondOK();



};

#endif //MAIN_API_TCPHANDLE_TCPCOMMANDHANDLE_H_
