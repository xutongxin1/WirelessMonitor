//
// Created by xtx on 2023/2/27.
//

#ifndef MAIN_API_REPEATERWIDGET_REPEATERDIALOG_H_
#define MAIN_API_REPEATERWIDGET_REPEATERDIALOG_H_

#include <QDialog>
#include "RepeaterWidget.h"

class RepeaterDialog : public RepeaterWidget, public QDialog {
public:
    RepeaterDialog() = default;

private:
    ~RepeaterDialog() override = default;
};

#endif //MAIN_API_REPEATERWIDGET_REPEATERDIALOG_H_
