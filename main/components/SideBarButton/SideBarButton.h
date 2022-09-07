//
// Created by xtx on 2022/9/7.
//

#ifndef QT_SIDEBARBUTTON_H
#define QT_SIDEBARBUTTON_H

#include <QWidget>
#include "CfgClass.h"

QT_BEGIN_NAMESPACE
namespace Ui { class SideBarButton; }
QT_END_NAMESPACE

class SideBarButton : public QWidget {
Q_OBJECT

public:
    explicit SideBarButton(QWidget *parent = nullptr);
    explicit SideBarButton(int num, CfgClass *MainCfg, QWidget *parent = nullptr);


    ~SideBarButton() override;

private:
    Ui::SideBarButton *ui;
};


#endif //QT_SIDEBARBUTTON_H
