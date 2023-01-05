//
// Created by xtx on 2022/9/7.
//

#ifndef QT_SIDEBARBUTTON_H
#define QT_SIDEBARBUTTON_H

#include <QWidget>
#include "CfgClass.h"
#include "RepeaterWidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class SideBarButton; }
QT_END_NAMESPACE

class SideBarButton : public RepeaterWidget {
Q_OBJECT

public:
    explicit SideBarButton(RepeaterWidget *parent = nullptr);
    explicit SideBarButton(int num, CfgClass *MainCfg, RepeaterWidget *parent = nullptr);
    QPushButton *Button;

    ~SideBarButton() override;

private:
    Ui::SideBarButton *ui;


};


#endif //QT_SIDEBARBUTTON_H
