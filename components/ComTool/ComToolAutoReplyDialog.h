//
// Created by xtx on 24-2-18.
//

#ifndef COMTOOLAUTOREPLYDIALOG_H
#define COMTOOLAUTOREPLYDIALOG_H

#include "RepeaterDialog.h"


QT_BEGIN_NAMESPACE
namespace Ui { class ComToolAutoReplyDialog; }
QT_END_NAMESPACE

class ComToolAutoReplyDialog : public RepeaterDialog {
Q_OBJECT

public:
    explicit ComToolAutoReplyDialog(QWidget *parent = nullptr);
    ~ComToolAutoReplyDialog() override;

private:
    Ui::ComToolAutoReplyDialog *ui;
};


#endif //COMTOOLAUTOREPLYDIALOG_H
