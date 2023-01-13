//
// Created by xtx on 2022/9/15.
//

#ifndef MAIN_API_REPEATERWIDGET_REPEATERWIDGET_H_
#define MAIN_API_REPEATERWIDGET_REPEATERWIDGET_H_

#include <QFrame>
#include <QtWidgets/QPushButton>
#include <QSettings>
#include <QDebug>
#include "structH.h"

enum DataType {
  SYS_TIME,
  USER_TIME
};
typedef struct DataNode {
  QString name;
  QList<double> *double_list;
  QList<QPair<double, double>> *time_double_list;
  int flag = 0;//判断是否画图不加数据,0——不画图，1——显示图，2——隐藏图但是会有数据
  DataType data_node_type;
  //long long d_size = 0;//记录数据存储大小
  //long long count = 0;//记录每个数据已经画了多少个了。要注意防止溢出！！！
} Datanode;

namespace Ui {
    class Charts;
}

typedef std::map<std::string, QPushButton> QPushButtonMap;

struct RequestNewWidget {
  WindowsType widget_type;
  int device_num;
};

class RepeaterWidget : public QWidget {
public:
//    QPushButtonMap button_;
    explicit RepeaterWidget(QWidget *parent = nullptr);

    ~RepeaterWidget();

    virtual void GetObjectFromUi(QPushButtonMap &result);

    virtual void GetConstructConfig();

    virtual void SaveConstructConfig();
//    virtual Ui::Charts *GetChartUi();
//    virtual QList<Datanode> GetChartDataPair();


    QString group_name_;
    QString config_file_path_;
    QSettings *cfg_;
    int device_num_;
    ToNewWidget *parent_info_;

    bool is_request_new_widget_ = false;
    RequestNewWidget new_widget_;

    TCPCommandHandle *tcp_command_handle_;

    TCPInfoHandle *tcp_info_handler_[4];

};

#endif //MAIN_API_REPEATERWIDGET_REPEATERWIDGET_H_
