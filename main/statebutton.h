//
// Created by xtx on 2022/9/7.
//

#ifndef QT_STATEBUTTON_H
#define QT_STATEBUTTON_H

#include <QWidget>
#include "CfgClass.h"

QT_BEGIN_NAMESPACE
namespace Ui { class stateButton; }
QT_END_NAMESPACE

class stateButton : public QWidget {
Q_OBJECT

public:
    explicit stateButton(QWidget *parent = nullptr);
    explicit stateButton(int num,CfgClass *MainCfg,QWidget *parent = nullptr);


    ~stateButton() override;

private:
    Ui::stateButton *ui;
};


#endif //QT_STATEBUTTON_H
