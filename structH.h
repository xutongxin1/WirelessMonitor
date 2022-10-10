//
// Created by xtx on 2022/9/27.
//

#ifndef MAIN_STRUCTH_H
#define MAIN_STRUCTH_H

#include <QTcpSocket>
#include "RepeaterWidget.h"
#include "qtmaterialtabs.h"
#include "TCPCommandHandle.h"

class RepeaterWidget;
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
    TCPCommandHandle* TCPHandler;
};
struct ToNewWidget
{
    std::vector<std::vector<WindowsInfo>> *DevicesWindowsInfo;//设备下窗口信息
    std::vector<DevicesInfo> *DevicesInfo;//设备信息
};

#endif //MAIN_STRUCTH_H
