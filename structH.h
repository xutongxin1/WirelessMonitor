//
// Created by xtx on 2022/9/27.
//

#ifndef MAIN__STRUCTH_H_
#define MAIN__STRUCTH_H_

#include <QTcpSocket>
#include "RepeaterWidget.h"
#include "qtmaterialtabs.h"
#include "TCPCommandHandle.h"
#include "TCPInfoHandle.h"
//#include "Charts/charts.h"

class RepeaterWidget;

class Charts;

//窗口类型泛型
enum WindowsType {
  NONE = 0,
  CHANNEL_CONFIGURATION = 1,
  XCOM = 50,
  MAIN_CHART = 51,
  DATA_CIRCULATION = 52,
  TCP_BRIDGE_CONFIGURATION = 201,
  TCP_COM = 202,
};

//窗口配置信息结构体
struct WindowsInfo {
    RepeaterWidget *widget;
    int index;
    WindowsType type;
};

//设备配置信息结构体
struct DevicesInfo {
  int windows_num;
  int tab_index;
  QtMaterialTabs *tab_widget;
  TCPCommandHandle *tcp_command_handler;
  TCPInfoHandle *tcp_info_handler[4];
  bool has_chart = false;
  Charts *charts_windows;
  int config_step = 1;
  int current_window = 1;
};

//传递给新建窗口的结构体
struct ToNewWidget {
  std::vector<std::vector<WindowsInfo>> *devices_windows_info;//设备下窗口信息
  std::vector<DevicesInfo> *devices_info;//设备信息
};

#endif //MAIN__STRUCTH_H_
