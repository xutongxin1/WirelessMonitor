//
// Created by xtx on 2023/11/26.
//

#ifndef WIRELESSMONITOR_API_PYTHONWORK_PYTHONWORKWITHI2C_H_
#define WIRELESSMONITOR_API_PYTHONWORK_PYTHONWORKWITHI2C_H_

#include "PythonWork.h"
class PythonWorkWithI2C : public PythonWork {
 public:
    PythonWorkWithI2C();

    bool PythonLoadFileForI2C(const QString &file_path);//加载用户自定义文件
    QString PythonRunUserDefForI2C(const QString &addr, const QString &val);//运行用户自定义函数
    QString ReciveDataFromI2C(const QString &addr, const QString &val) override;//接收I2C数据
};

#endif //WIRELESSMONITOR_API_PYTHONWORK_PYTHONWORKWITHI2C_H_
