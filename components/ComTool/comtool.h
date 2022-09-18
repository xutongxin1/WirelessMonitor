//
// Created by xtx on 2022/9/16.
//

#ifndef QT_COMTOOL_H
#define QT_COMTOOL_H

#include <QTcpSocket>
#include "RepeaterWidget.h"
#include <QtSerialPort/QtSerialPort>
#include <QtSerialPort/QSerialPortInfo>


QT_BEGIN_NAMESPACE
namespace Ui { class comtool; }
QT_END_NAMESPACE

class ComTool : public RepeaterWidget {
Q_OBJECT

public:
    explicit ComTool(QWidget *parent = nullptr);

    ~ComTool() override;

private:
    Ui::comtool *ui;
    bool comOk;                 //串口是否打开
    QSerialPort *com;        //串口通信对象
    QTimer *timerRead;          //定时读取串口数据
    QTimer *timerSend;          //定时发送串口数据
    QTimer *timerSave;          //定时保存串口数据

    int sleepTime;              //接收延时时间
    int sendCount;              //发送数据计数
    int receiveCount;           //接收数据计数
    bool isShow;                //是否显示数据

    bool tcpOk;                 //网络是否正常
    QTcpSocket *socket;         //网络连接对象
    QTimer *timerConnect;       //定时器重连

private slots:

    void initForm();            //初始化窗体数据
    void initConfig();          //初始化配置文件
    void saveConfig();          //保存配置文件
    void readData();            //读取串口数据
    void sendData();            //发送串口数据
    void sendData(QString data);//发送串口数据带参数
    void saveData();            //保存串口数据

    void changeEnable(bool b);  //改变状态
    void append(int type, const QString &data, bool clear = false);

private slots:

    void connectNet();

    void readDataNet();

    void readErrorNet();

private slots:

    void on_btnOpen_clicked();

    void on_btnStopShow_clicked();

    void on_btnSendCount_clicked();

    void on_btnReceiveCount_clicked();

    void on_btnClear_clicked();

    void on_btnData_clicked();

    void on_btnStart_clicked();

    void on_ckAutoSend_stateChanged(int arg1);

    void on_ckAutoSave_stateChanged(int arg1);
};


#endif //QT_COMTOOL_H
