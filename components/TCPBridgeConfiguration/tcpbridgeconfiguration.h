//
// Created by xtx on 2022/9/21.
//

#ifndef MAIN_TCPBRIDGECONFIGURATION_H
#define MAIN_TCPBRIDGECONFIGURATION_H

#include <RepeaterWidget>


QT_BEGIN_NAMESPACE
namespace Ui { class TCPBridgeConfiguration; }
QT_END_NAMESPACE

class TCPBridgeConfiguration : public RepeaterWidget {
    Q_OBJECT

public:
    explicit TCPBridgeConfiguration(QWidget *parent = nullptr);

    ~TCPBridgeConfiguration() override;

private:
    Ui::TCPBridgeConfiguration *ui;
};


#endif //MAIN_TCPBRIDGECONFIGURATION_H
