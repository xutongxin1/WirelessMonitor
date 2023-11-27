//
// Created by xtx on 2023/11/23.
//

#ifndef WIRELESSMONITOR_API_PYTHONWORK_PYTHONWORK_H_
#define WIRELESSMONITOR_API_PYTHONWORK_PYTHONWORK_H_

//防止与QT的slots宏冲突
#pragma push_macro("slots")
#undef slots
#include <boost/python.hpp>
#include <Python.h>
#pragma pop_macro("slots")

#include <QApplication>
using namespace boost::python;
class PythonWork {

 public:
    PythonWork();
    ~PythonWork();
    object class_object_;
    object main_namespace_;

    //以下函数为绑定使用
    virtual QString ReciveDataFromI2C(const QString &addr, const QString &val) { return {}; };//接收I2C数据
 protected:
    bool PythonValueSave(const QString &val_name, const QString &val);//帮助用户预存变量
    QString PythonRunUserDef(const QString &def_name, const QString &val);//运行用户自定义函数
    bool PythonLoadFile(const QString &file_path, const QString &class_name);//加载用户自定义文件


};

#endif //WIRELESSMONITOR_API_PYTHONWORK_PYTHONWORK_H_
