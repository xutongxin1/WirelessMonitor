//
// Created by xtx on 2022/9/8.
//

#ifndef QT_CHANNELCONFIGURATION_H
#define QT_CHANNELCONFIGURATION_H

#include <QWidget>
#include "CfgClass.h"
#include "RepeaterWidget.h"
#include "structH.h"


QT_BEGIN_NAMESPACE
namespace Ui { class ChannelConfiguration; }
QT_END_NAMESPACE

class ChannelConfiguration : public RepeaterWidget {
Q_OBJECT

public:
    explicit ChannelConfiguration(int DeviceNum, CfgClass *MainCfg, ToNewWidget *parentInfo,
                                  RepeaterWidget *parent = nullptr);

    ~ChannelConfiguration() override;

private:
    Ui::ChannelConfiguration *ui;

    ToNewWidget *parentInfo;

    QTcpSocket* TCPHandler;

    int DeviceNum;

    QString ip;
    int port;
    int ConnectStep;

    void onConnect();

    void onDisconnect();

    void reflashUi(bool isXMB);

    void SendModePackage();
};


#endif //QT_CHANNELCONFIGURATION_H
