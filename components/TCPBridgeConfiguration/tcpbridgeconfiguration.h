//
// Created by xtx on 2022/9/21.
//

#ifndef MAIN_COMPONENTS_TCPBRIDGECONFIGURATION_TCPBRIDGECONFIGURATION_H_
#define MAIN_COMPONENTS_TCPBRIDGECONFIGURATION_TCPBRIDGECONFIGURATION_H_

#include <QSettings>
#include "RepeaterWidget.h"
#include "structH.h"

QT_BEGIN_NAMESPACE
namespace Ui { class TCPBridgeConfiguration; }
QT_END_NAMESPACE

class TCPBridgeConfiguration : public RepeaterWidget {
 Q_OBJECT

 public:
    explicit TCPBridgeConfiguration(int device_num, int win_num, QSettings *cfg, ToNewWidget *parent_info,
                                    QWidget *parent = nullptr);

    ~TCPBridgeConfiguration() override;

    void GetConstructConfig() override;

    void SaveConstructConfig() override;

    enum IOMode {
        IO_MODE_CLOSED = 0,//关闭
        IO_MODE_ON = 1,//开启输入输出
        IO_MODE_TRANSMIT_ON = 2,//开启输入输出并转发
    };

 private:
    Ui::TCPBridgeConfiguration *ui_;
    IOMode mode_1_ = IO_MODE_CLOSED;
    IOMode mode_2_ = IO_MODE_CLOSED;

    bool get_command_reply_ = false;

    QString ip_;

    int baud_rate_1_ = 115200;                //波特率1
    int data_bit_1_ = 8;                 //数据位1
    int parity_1_ = 0;              //校验位1
    double stop_bit_1_ = 1;              //停止位1
    int baud_rate_2_ = 115200;                //波特率2
    int data_bit_2_ = 8;                 //数据位2
    int parity_2_ = 0;              //校验位2
    double stop_bit_2_ = 1;              //停止位2
//    int baud_rate_3_ = 115200;                //波特率3
//    int data_bit_3_ = 8;                 //数据位3
//    int parity_3_ = 0;             //校验位3
//    double stop_bit_3_ = 1;              //停止位3


    class TCPCommandHandle *tcp_command_handle_;

    void ChangeMode();

    void RefreshBox();

    void SetUart();

    void SetTcp();


};

#endif //MAIN_COMPONENTS_TCPBRIDGECONFIGURATION_TCPBRIDGECONFIGURATION_H_
