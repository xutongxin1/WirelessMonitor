//
// Created by xtx on 2023/11/27.
//

#ifndef WIRELESSMONITOR_COMPONENTS_IICDEVICEBASIC_IICDEVICEEACH_H_
#define WIRELESSMONITOR_COMPONENTS_IICDEVICEBASIC_IICDEVICEEACH_H_

#include "RepeaterWidget.h"
#include "PythonWorkWithI2C.h"
QT_BEGIN_NAMESPACE
namespace Ui { class IICDeviceEach; }
QT_END_NAMESPACE
class PythonWork;
class IICDeviceEach : public RepeaterWidget {
 Q_OBJECT

 public:
    explicit IICDeviceEach(QWidget *parent = nullptr);
    ~IICDeviceEach() override;

 private:
    Ui::IICDeviceEach *ui_;
    PythonWork *python_work_;
};

#endif //WIRELESSMONITOR_COMPONENTS_IICDEVICEBASIC_IICDEVICEEACH_H_
