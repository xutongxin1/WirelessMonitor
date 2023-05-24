//
// Created by xtx on 2022/9/16.
//

#ifndef MAIN_COMPONENTS_ComTool_ComTool_H_
#define MAIN_COMPONENTS_ComTool_ComTool_H_

#include <QTcpSocket>
#include "RepeaterWidget.h"
#include "ConfigClass.h"
#include <QtSerialPort/QtSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include "structH.h"
#include "highlighter.h"
//#include "ui_ComTool.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ComTool; }
QT_END_NAMESPACE

class ComTool : public RepeaterWidget {
 Q_OBJECT

signals:
    void RecNewData(QByteArray data, const QDateTime &time);

 public:
    explicit ComTool(int row, int win_num, QSettings *cfg, ToNewWidget *parent_info, QWidget *parent = nullptr);

    ~ComTool() override;

    QString port_name_;            //串口号
    int baud_rate_ = 115200;                //波特率
    int data_bit_ = 8;                 //数据位
    int parity_ = 0;              //校验位
    double stop_bit_ = 1;              //停止位

    static bool hex_send_;                //16进制发送
    static bool hex_receive_;             //16进制接收
    static bool debug_;                  //模拟设备
    static bool auto_clear_;              //自动清空

    static bool auto_send_;               //自动发送
    static int send_interval_;            //自动发送间隔
    static bool auto_save_;               //自动保存
    static int save_interval_;            //自动保存间隔

    static QString mode_;                //转换模式
    static QString server_ip_;            //服务器IP
    static int server_port_;              //服务器端口
    static int listen_port_;              //监听端口
    static int sleep_time_;               //延时时间
    static bool auto_connect_;            //自动重连

    QString group_name_;
    QString config_file_path_;
    QSettings *cfg_;

 private:
    Ui::ComTool *ui_;
    QTimer *timer_read_;          //定时读取串口数据
    QTimer *timer_send_;          //定时发送串口数据
    QTimer *timer_save_;          //定时保存串口数据

    int sleep_time_rec_;              //接收延时时间
    int send_count_;              //发送数据计数
    int receive_count_;           //接收数据计数
    bool is_show_;                //是否显示数据

    QTimer *timer_connect_;       //定时器重连

    QTimer *timer_for_port_;

    void GetConstructConfig() override;

    void SaveConstructConfig() override;

    QString ip_;

    void UpdateComSetting();

    QStringList GetPortInfo();
    void ReflashComCombo();
    bool OpenSerial();
    QStringList my_serialportinfo_ = {};

    QSerialPort *my_serialport_;

    void StartTool();
    void ChangeMode();

    Highlighter *highlighter1;
    Highlighter *highlighter2;

    bool is_start_ = false;

    typedef struct history_send {
        QString data;
        bool is_Hex = false;
        int send_num = 1;
        QDateTime time;
    } HistorySend;
    QHash<QString, HistorySend> history_send_list_;

    void UpdateSendHistory();

 private slots:

//    void InitConfig();          //初始化配置文件
//    void SaveConfig();          //保存配置文件
    void GetData();            //读取串口数据
    void ProcessData(QByteArray main_serial_recv_data);
    void SendData();            //发送串口数据
    void SaveData();            //保存串口数据

    void Append(int type, const QString &data, bool clear = false);

    void ReadErrorNet();

    void on_btnClear_clicked();

};

//class ComToolSend : public QSerialPort{
// Q_OBJECT
//
// signals:
//    static void RecData(QByteArray data, const QDateTime &time);
//
// public:
//    ~ComToolSend() override;
//
//};

#endif //MAIN_COMPONENTS_ComTool_ComTool_H_
