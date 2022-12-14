//
// Created by xtx on 2022/9/21.
//

#ifndef MAIN_TCPBRIDGECONFIGURATION_H
#define MAIN_TCPBRIDGECONFIGURATION_H

#include <QSettings>
#include "RepeaterWidget.h"
#include "structH.h"


QT_BEGIN_NAMESPACE
namespace Ui { class TCPBridgeConfiguration; }
QT_END_NAMESPACE

class TCPBridgeConfiguration : public RepeaterWidget {
Q_OBJECT

public:
    explicit TCPBridgeConfiguration(int DeviceNum, int winNum, QSettings *cfg, ToNewWidget *parentInfo,
                                    QWidget *parent = nullptr);

    ~TCPBridgeConfiguration() override;

    void GetConstructConfig() override;

    void SaveConstructConfig() override;

    enum IOMode {
        Closed = 0,
        Input,
        Output,
        SingleInput,
        SingleOutput,
        Follow1Output,
        Follow3Input
    };

private:
    Ui::TCPBridgeConfiguration *ui;
    IOMode mode1 = Closed;
    IOMode mode2 = Closed;
    IOMode mode3 = Closed;

    QString IP;

    int BaudRate1 = 115200;                //波特率1
    int DataBit1 = 8;                 //数据位1
    int Parity1 = 0;              //校验位1
    double StopBit1 = 1;              //停止位1
//    int BaudRate2 = 115200;                //波特率2
//    int DataBit2 = 8;                 //数据位2
//    QString Parity2 = QString::fromUtf8("无");              //校验位2
//    double StopBit2 = 1;              //停止位2
    int BaudRate3 = 115200;                //波特率3
    int DataBit3 = 8;                 //数据位3
    int Parity3 = 0;             //校验位3
    double StopBit3 = 1;              //停止位3


    class TCPCommandHandle *TCPCommandHandle;

    TCPInfoHandle *TCPInfoHandler[4];

    void ChangeMode();

    void RefreshBox();

    void SetUart();

    void setTCP();


};


#endif //MAIN_TCPBRIDGECONFIGURATION_H
