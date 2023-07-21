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
#include <QCheckBox>
#include <QColorDialog>

#include "defineH.h"

class RepeaterWidget;
class ComTool;
class ChartsNext;
class MainWindow;

//窗口类型泛型
enum WindowsType {
    NONE = 0,
    CHANNEL_CONFIGURATION = 1,
    COM_TOOL = 50,
    MAIN_CHART = 51,
    DATA_CIRCULATION = 52,
    TCP_BRIDGE_CONFIGURATION = 201,
    TCP_COM = 202,
};

//窗口配置信息结构体
struct WindowsInfo {
    RepeaterWidget *widget{};
    int index = -1;
    WindowsType type = NONE;
};

//设备配置信息结构体
struct DevicesInfo {
    int connect_mode=0;//0:无,1:调试器,2:COM,3:TCP
    int windows_num=-1;
    int tab_index=-1;
    QtMaterialTabs *tab_widget{};
    TCPCommandHandle *tcp_command_handler{};
    TCPInfoHandle *tcp_info_handler[4]{};
    ComTool *com_tool{};
    bool has_chart = false;
    ChartsNext *charts_windows{};
    int config_step = 1;
    int current_window = 1;
};

//传递给新建窗口的结构体
struct ToNewWidget {
    MainWindow *main_window;
    std::vector<std::vector<WindowsInfo>> *devices_windows_info;//设备下窗口信息
    std::vector<DevicesInfo> *devices_info;//设备信息
};

///时间格式
enum TimeType {
    PROGRAM_TIME = 0,//程序启动时间
    DATE_TIME,//日期时间
    DATA_TIME//数据排列时间
};

///共用体,降低内存占用
///其实我很担心这一段会有一些奇怪的问题
union DataTime {
    const QDateTime *date_time_{};
    double program_time_;
    int data_time_;
    DataTime() {

    }
    ~DataTime() {
        //注意，该析构函数无任何释放过程，即QDateTime类需要被手动释放
    }
};

struct singaldata {
    double data{};
    DataTime time;
    singaldata() = default;
    ~singaldata() = default;
};

///数据点
struct DataNode {
    QString data_name;                  ///数据名称
    QColor line_color = Qt::red;        ///线条颜色
    int line_width = 2;
//  QList<double> *double_list;
    QVector<singaldata> data_list;
    bool is_visible = true;             ///是否可见
    TimeType time_type = PROGRAM_TIME;
    //long long d_size = 0;//记录数据存储大小
    //long long count = 0;//记录每个数据已经画了多少个了。要注意防止溢出！！！
    bool is_update = false;
    int last_draw_index = -1;
};

///数据点的索引
struct DataNodeIndex {
    QVector<singaldata> *data_list;
    bool *is_update;
    int *last_draw_index;
};

/// charts中的变量信息
struct ChartsList {
    QString data_name;
    QPushButton *choose_color = new QPushButton;
    QCheckBox *check_visible = new QCheckBox;
};

#endif //MAIN__STRUCTH_H_
