//
// Created by xtx on 2022/9/15.
//

#ifndef QT_REPEATERWIDGET_H
#define QT_REPEATERWIDGET_H


#include <QFrame>
#include <QtWidgets/QPushButton>
#include <QSettings>
#include <QDebug>
#include "structH.h"

enum DataType {
    sys_time,
    user_time
};
typedef struct DataNode {
    QString name;
    QList<double> *DoubleList;
    QList<QPair<double, double>> *TimeDouble_List;
    int flag = 0;//判断是否画图不加数据,0——不画图，1——显示图，2——隐藏图但是会有数据
    DataType DataNodeType;
    //long long d_size = 0;//记录数据存储大小
    //long long count = 0;//记录每个数据已经画了多少个了。要注意防止溢出！！！
} Datanode;

namespace Ui {
    class Charts;
}

typedef std::map<std::string, QPushButton> QPushButtonMap;

struct RequestNewWidget {
    WindowsType widgetType;
    int DeviceNum;
};

class RepeaterWidget : public QWidget {
public:
//    QPushButtonMap button_;
    explicit RepeaterWidget(QWidget *parent = nullptr);

    ~RepeaterWidget();

    virtual void GetObjectFromUI(QPushButtonMap &result);

    virtual void GetConstructConfig();

    virtual void SaveConstructConfig();
//    virtual Ui::Charts *GetChartUi();
//    virtual QList<Datanode> GetChartDataPair();


    QString GroupName;
    QString ConfigFilePath;
    QSettings *cfg;
    int DeviceNum;
    ToNewWidget *parentInfo;

    bool isRequestNewWidget = false;
    RequestNewWidget NewWidget;

    TCPCommandHandle *TCPCommandHandle;

    TCPInfoHandle *TCPInfoHandler[4];

};


#endif //QT_REPEATERWIDGET_H
