//
// Created by xtx on 2022/9/16.
//

#ifndef QT_TCPCom_H
#define QT_TCPCom_H

#include <QTcpSocket>
#include "RepeaterWidget.h"
#include "CfgClass.h"
#include <QtSerialPort/QtSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include "structH.h"
//#include "ui_tcpcom.h"

QT_BEGIN_NAMESPACE
namespace Ui { class tcpcom; }
QT_END_NAMESPACE

class TCPCom : public RepeaterWidget {
Q_OBJECT

public:
    explicit TCPCom(int DeviceNum, int winNum, QSettings *cfg, ToNewWidget *parentInfo, QWidget *parent = nullptr);

    ~TCPCom() override;


    static QString PortName;            //串口号
    static int BaudRate;                //波特率
    static int DataBit;                 //数据位
    static QString Parity;              //校验位
    static double StopBit;              //停止位

    static bool HexSend;                //16进制发送
    static bool HexReceive;             //16进制接收
    static bool Debug;                  //模拟设备
    static bool AutoClear;              //自动清空

    static bool AutoSend;               //自动发送
    static int SendInterval;            //自动发送间隔
    static bool AutoSave;               //自动保存
    static int SaveInterval;            //自动保存间隔

    static QString Mode;                //转换模式
    static QString ServerIP;            //服务器IP
    static int ServerPort;              //服务器端口
    static int ListenPort;              //监听端口
    static int SleepTime;               //延时时间
    static bool AutoConnect;            //自动重连

    QString GroupName;
    QString ConfigFilePath;
    QSettings *cfg;


private:
    Ui::tcpcom *ui;
    QTimer *timerRead;          //定时读取串口数据
    QTimer *timerSend;          //定时发送串口数据
    QTimer *timerSave;          //定时保存串口数据

    int sleepTime;              //接收延时时间
    int sendCount;              //发送数据计数
    int receiveCount;           //接收数据计数
    bool isShow;                //是否显示数据

    QTimer *timerConnect;       //定时器重连



    void GetConstructConfig();

    void SaveConstructConfig();

    QString IP;


private slots:

    void initForm();            //初始化窗体数据
//    void initConfig();          //初始化配置文件
//    void saveConfig();          //保存配置文件
    void getData(const QByteArray &data,int port);            //读取串口数据
    void sendData();            //发送串口数据
    void saveData();            //保存串口数据

    void changeEnable(bool b);  //改变状态
    void append(int type, const QString &data, bool clear = false);

private slots:

    void readErrorNet();

private slots:

    void on_btnClear_clicked();

    void on_btnData_clicked();

};


#endif //QT_TCPCom_H
