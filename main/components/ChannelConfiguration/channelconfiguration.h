//
// Created by xtx on 2022/9/8.
//

#ifndef QT_CHANNELCONFIGURATION_H
#define QT_CHANNELCONFIGURATION_H

#include <QWidget>
#include "CfgClass.h"


QT_BEGIN_NAMESPACE
namespace Ui { class ChannelConfiguration; }
QT_END_NAMESPACE

class ChannelConfiguration : public QWidget {
Q_OBJECT

public:
    explicit ChannelConfiguration(int num, CfgClass *MainCfg, QWidget *parent = nullptr);

    ~ChannelConfiguration() override;

private:
    Ui::ChannelConfiguration *ui;
    void onConnect();
    void onDisconnect();
};


#endif //QT_CHANNELCONFIGURATION_H
