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
    explicit ChannelConfiguration(int DeviceNum,  QSettings *MainCFG,QSettings *DeviceCFG, ToNewWidget *parentInfo,
                                  RepeaterWidget *parent = nullptr);

    ~ChannelConfiguration() override;

    void GetConstructConfig() override;

    void SaveConstructConfig() override;


private:
    Ui::ChannelConfiguration *ui;

    QString DeviceGroup;

    bool hasGiveUp = false;

    QString ip;
    int port;
    int ConnectStep;

    QSettings* MainCFG;

    void onConnect();

    void onDisconnect();

    void reflashUi(bool isXMB);

    void SendModePackage();

    void SecondConnect();
};


#endif //QT_CHANNELCONFIGURATION_H
