//
// Created by xtx on 2022/9/7.
//

#ifndef MAIN_COMPONENTS_SIDEBARBUTTON_SIDEBARBUTTON_H_
#define MAIN_COMPONENTS_SIDEBARBUTTON_SIDEBARBUTTON_H_

#include <QWidget>
#include "ConfigClass.h"
#include "RepeaterWidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class SideBarButton; }
QT_END_NAMESPACE

class SideBarButton : public RepeaterWidget {
 Q_OBJECT

 public:
    explicit SideBarButton(RepeaterWidget *parent = nullptr);
    explicit SideBarButton(int num, ConfigClass *device_cfg, RepeaterWidget *parent = nullptr);
    QPushButton *button_;

    ~SideBarButton() override;

 private:
    Ui::SideBarButton *ui_;

};

#endif //MAIN_COMPONENTS_SIDEBARBUTTON_SIDEBARBUTTON_H_
