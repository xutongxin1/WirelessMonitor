//
// Created by xtx on 2022/9/21.
//

#ifndef MAIN_TCPBRIDGECONFIGURATION_H
#define MAIN_TCPBRIDGECONFIGURATION_H

#include <QSettings>
#include "RepeaterWidget.h"


QT_BEGIN_NAMESPACE
namespace Ui { class TCPBridgeConfiguration; }
QT_END_NAMESPACE

class TCPBridgeConfiguration : public RepeaterWidget {
Q_OBJECT

public:
    explicit TCPBridgeConfiguration(int DeviceNum, int winNum, QSettings *cfg, QWidget *parent = nullptr);

    ~TCPBridgeConfiguration() override;

    void GetConstructConfig() override;

    void SaveConstructConfig() override;

    enum IOMode {
        Closed = 0,
        Input,
        Output,
        SingleInput,
        SingleOutput
    };

private:
    Ui::TCPBridgeConfiguration *ui;
    IOMode mode1=Closed;
    IOMode mode2=Closed;
    IOMode mode3=Closed;

    void ChangeMode();
};


#endif //MAIN_TCPBRIDGECONFIGURATION_H
