//
// Created by xtx on 2022/9/8.
//

#ifndef MAIN_COMPONENTS_CHANNELCONFIGURATION_CHANNELCONFIGURATION_H_
#define MAIN_COMPONENTS_CHANNELCONFIGURATION_CHANNELCONFIGURATION_H_

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
  explicit ChannelConfiguration(int device_num, QSettings *main_cfg, QSettings *device_cfg, ToNewWidget *parent_info,
                                RepeaterWidget *parent = nullptr);

  ~ChannelConfiguration() override;

  void GetConstructConfig() override;

    void SaveConstructConfig() override;

 private:
    Ui::ChannelConfiguration *ui_;

    QString device_group_;

    bool has_give_up_ = false;

    QString ip_;
//    int port_ = 0;
//    int connect_step_ = 0;

    QSettings *main_cfg_;

    void OnConnect();

    void OnDisconnect();

    void ReflashUi(bool is_xmb);

//    void SendModePackage();
//
//    void SecondConnect();
};

#endif //MAIN_COMPONENTS_CHANNELCONFIGURATION_CHANNELCONFIGURATION_H_
