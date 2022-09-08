//
// Created by xtx on 2022/9/8.
//

#ifndef QT_CHANNELCONFIGURATION_H
#define QT_CHANNELCONFIGURATION_H

#include <QWidget>


QT_BEGIN_NAMESPACE
namespace Ui { class ChannelConfiguration; }
QT_END_NAMESPACE

class ChannelConfiguration : public QWidget {
Q_OBJECT

public:
    explicit ChannelConfiguration(QWidget *parent = nullptr);

    ~ChannelConfiguration() override;

private:
    Ui::ChannelConfiguration *ui;
};


#endif //QT_CHANNELCONFIGURATION_H
