//
// Created by xutongxin on 2023/11/17.
//

#ifndef MAIN_COMPONENTS_IICDEVICEBASIC_IICDEVICEBASIC_H_
#define MAIN_COMPONENTS_IICDEVICEBASIC_IICDEVICEBASIC_H_

#include "RepeaterWidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class IICDeviceBasic; }
QT_END_NAMESPACE

class IICDeviceBasic : public RepeaterWidget {
    Q_OBJECT

 public:
    explicit IICDeviceBasic(QWidget *parent = nullptr);
    ~IICDeviceBasic() override;

 private:
    Ui::IICDeviceBasic *ui_;
};

#endif //MAIN_COMPONENTS_IICDEVICEBASIC_IICDEVICEBASIC_H_
