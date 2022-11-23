//
// Created by xtx on 2022/9/27.
//

#ifndef MAIN_STRUCTH_H
#define MAIN_STRUCTH_H

#include <QTcpSocket>
#include "RepeaterWidget.h"
#include "qtmaterialtabs.h"
#include "TCPCommandHandle.h"
#include "TCPInfoHandle.h"
//#include "Charts/charts.h"

class RepeaterWidget;
class Charts;
enum WindowsType {
    None = 0,
    Channel_Configuration = 1,
    XCOM = 50,
    MainChart = 51,
    TCP_Bridge_Configuration = 201,
    TCP_Com = 202,
};
struct WindowsInfo//窗口配置信息结构体
{
    RepeaterWidget *widget;
    int index;
    WindowsType type;
};
struct DevicesInfo//设备配置信息结构体
{
    int windowsNum;
    int TabIndex;
    QtMaterialTabs *TabWidget;
    TCPCommandHandle* TCPCommandHandler;
    TCPInfoHandle* TCPInfoHandler[4];
    bool hasChart=false;
    Charts *ChartsWindows;
};
struct ToNewWidget
{
    std::vector<std::vector<WindowsInfo>> *DevicesWindowsInfo;//设备下窗口信息
    std::vector<DevicesInfo> *DevicesInfo;//设备信息
};

#endif //MAIN_STRUCTH_H
