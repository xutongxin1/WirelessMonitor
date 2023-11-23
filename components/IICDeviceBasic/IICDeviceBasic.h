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
    explicit IICDeviceBasic(int device_num, int win_num, QSettings *cfg, ToNewWidget *parent_info,
                            QWidget *parent = nullptr);
    ~IICDeviceBasic() override;
    void GetConstructConfig() override;

 private:
    Ui::IICDeviceBasic *ui_;

    void ChooseBoardConfig();

    QString last_board_ini_folder_;
};

#endif //MAIN_COMPONENTS_IICDEVICEBASIC_IICDEVICEBASIC_H_
