//
// Created by xtx on 2023/2/27.
//

#ifndef MAIN_COMPONENTS_COMTOOL_COMTOOLSETTING_H_
#define MAIN_COMPONENTS_COMTOOL_COMTOOLSETTING_H_

#include "RepeaterWidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ComToolSetting; }
QT_END_NAMESPACE

class ComToolSetting : public RepeaterWidget {
 Q_OBJECT

 public:
    explicit ComToolSetting(QWidget *parent = nullptr);
    ~ComToolSetting() override;

 private:
    Ui::ComToolSetting *ui;
};

#endif //MAIN_COMPONENTS_COMTOOL_COMTOOLSETTING_H_
