//
// Created by xtx on 2022/9/16.
//

#ifndef MAIN_COMPONENTS_TCPCOM_TCPCOM_H_
#define MAIN_COMPONENTS_TCPCOM_TCPCOM_H_

#include <QTcpSocket>
#include "RepeaterWidget.h"
#include "CfgClass.h"
#include <QtSerialPort/QtSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include "structH.h"
//#include "ui_tcpcom.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Tcpcom; }
QT_END_NAMESPACE

class TcpCom : public RepeaterWidget {
 Q_OBJECT

 public:
    explicit TcpCom(int device_num, int win_num, QSettings *cfg, ToNewWidget *parent_info, QWidget *parent = nullptr);

    ~TcpCom() override;

    static QString port_name_;            //串口号
    static int baud_rate_;                //波特率
    static int data_bit_;                 //数据位
    static QString parity_;              //校验位
    static double stop_bit_;              //停止位

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
    Ui::Tcpcom *ui_;
    QTimer *timer_read_;          //定时读取串口数据
    QTimer *timer_send_;          //定时发送串口数据
    QTimer *timer_save_;          //定时保存串口数据

    int sleep_time_rec;              //接收延时时间
    int send_count_;              //发送数据计数
    int receive_count_;           //接收数据计数
    bool is_show_;                //是否显示数据

    QTimer *timer_connect_;       //定时器重连



    void GetConstructConfig();

    void SaveConstructConfig();

    QString ip_;

 private slots:

    void InitForm();            //初始化窗体数据
//    void InitConfig();          //初始化配置文件
//    void SaveConfig();          //保存配置文件
    void GetData(const QByteArray &data, int port);            //读取串口数据
    void SendData();            //发送串口数据
    void SaveData();            //保存串口数据

    void ChangeEnable(bool b);  //改变状态
    void Append(int type, const QString &data, bool clear = false);

 private slots:

    void ReadErrorNet();

 private slots:

    void on_btnClear_clicked();

    void on_btnData_clicked();

};

#endif //MAIN_COMPONENTS_TCPCOM_TCPCOM_H_
