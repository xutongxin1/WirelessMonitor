//
// Created by xtx on 2023/2/27.
//
//警告：该类实现的是非模态对话框，且使用模态方案
#ifndef MAIN_API_REPEATERWIDGET_REPEATERDIALOG_H_
#define MAIN_API_REPEATERWIDGET_REPEATERDIALOG_H_

#include <QDialog>
#include "RepeaterWidget.h"

class RepeaterDialog : public QDialog {
public:
	RepeaterDialog(QWidget *parent = nullptr);

	~RepeaterDialog() override = default;

private:
	[[maybe_unused]] void connectResultSingal();

	virtual void GetData();
};

#endif //MAIN_API_REPEATERWIDGET_REPEATERDIALOG_H_
