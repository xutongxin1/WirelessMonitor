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

namespace Ui {
    class Charts;
}

typedef std::map<std::string, QPushButton> QPushButtonMap;

struct RequestNewWidget {
  WindowsType widget_type;
  int device_num;
};

class RepeaterWidget : public QWidget {
 Q_OBJECT
 signals:
    void OrderExchangeWindow(int device, int windows_num);
    void OrderShowSnackbar (const QString& message);
    void UseHistory();
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

    int connect_mode;

    TCPCommandHandle *tcp_command_handle_;

    TCPInfoHandle *tcp_info_handler_[4];

    void StopAllInfoTCP();
};

#endif //MAIN_API_REPEATERWIDGET_REPEATERWIDGET_H_
