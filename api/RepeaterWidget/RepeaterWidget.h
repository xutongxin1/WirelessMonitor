//
// Created by xtx on 2022/9/15.
//

#ifndef QT_REPEATERWIDGET_H
#define QT_REPEATERWIDGET_H


#include <QFrame>
#include <QtWidgets/QPushButton>
#include <QSettings>

typedef struct DataNode{
    QString name;
    double *DataBuff;
    int flag = 0;//判断是否画图不加数据,0——不画图，1——显示图，2——隐藏图但是会有数据
    int num;//在graph里对应的名次，需要实时更新
    long long count=0;//记录每个数据已经画了多少个了。要注意防止溢出！！！
}Datanode;

namespace Ui {
class Charts;
}

typedef std::map<std::string, QPushButton> QPushButtonMap;
enum WindowsType
{
    None,
    ChannelConfiguration,
    XCOM,
    MainChart,

};
struct RequestNewWidget {
    WindowsType widgetType;
    int DeviceNum;
};
class RepeaterWidget : public QWidget {
public:
//    QPushButtonMap Button;
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

    bool isRequestNewWidget=false;
    RequestNewWidget NewWidget;

};


#endif //QT_REPEATERWIDGET_H
