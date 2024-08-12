//
// Created by xtx on 2023/11/26.
//

#include "PythonWorkWithI2C.h"
PythonWorkWithI2C::PythonWorkWithI2C() : PythonWork() {

    PythonLoadFileForI2C(R"(C:\GitProject\QT\thirdTarget_I2CTool\config\AHT10\AHT10.py)");//测试用
}
bool PythonWorkWithI2C::PythonLoadFileForI2C(const QString &file_path) {
    return PythonLoadFile(file_path, "I2CWork");
}
QString PythonWorkWithI2C::PythonRunUserDefeForI2CWithAddr(const QString &addr, const QString &val) {
    return PythonRunUserDef("fuction_" + addr, val);
}
QString PythonWorkWithI2C::ReciveDataFromI2C(const QString &addr, const QString &val) {
    PythonValueSave("data_" + addr, val);
    return PythonRunUserDefeForI2CWithAddr(addr, val);
}





