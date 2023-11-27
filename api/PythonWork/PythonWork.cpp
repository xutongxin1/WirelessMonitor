//
// Created by xtx on 2023/11/23.
//
#define BOOST_NUMPY_STATIC_LIB
#include "PythonWork.h"
#include <cstdlib>
#include <iostream>
#include <QFileInfo>

int setenv(const char *name, const char *value, int overwrite) {
    int errcode = 0;
    if (!overwrite) {
        size_t envsize = 0;
        errcode = getenv_s(&envsize, NULL, 0, name);
        if (errcode || envsize) { return errcode; }
    }
    return _putenv_s(name, value);
}


PythonWork::PythonWork() {
    setenv("PYTHONPATH", R"(C:\GitProject\QT\cmake-build-debug-visual-studio\config\I2C)", 1);
    setenv("PYTHONHOME", R"(C:\Users\xtx\.vcpkg-clion\vcpkg\packages\python3_x64-windows\tools\python3)", 1);
    try {
        Py_Initialize();

//        object my_python_class_module = import("MPU6050");
//
//        object Work = my_python_class_module.attr("I2CWork")();
//        object main_namespace = Work.attr("__dict__");
//        object test = exec("a=11",main_namespace);

//        Work.attr("fuction_28")();
    }

    catch (const error_already_set &) {
        PyErr_Print();
        qCritical() << "Python Initialize error";

    }
}
bool PythonWork::PythonValueSave(const QString &val_name, const QString &val) {
    try {
        exec((val_name + "=" + val).toStdString().c_str(), main_namespace_);
        return true;
    }
    catch (const error_already_set &) {
        PyErr_Print();
        qCritical()
            << "PythonValueSave error";
        return false;
    }
}
QString PythonWork::PythonRunUserDef(const QString &def_name, const QString &val) {
    try {
        object ret = class_object_.attr(def_name.toStdString().c_str())(val.toStdString().c_str());
        std::string tmp = extract<std::string>(ret);//意外报错，但是不影响编译与功能运行
        return QString::fromStdString(tmp);
    }
    catch (const error_already_set &) {
        PyErr_Print();
        qInfo()
            << "PythonRunUserDef not worked";
        return {};
    }
}
bool PythonWork::PythonLoadFile(const QString &file_path, const QString &class_name) {
    //获取名称，路径
    QFileInfo file_info = QFileInfo(file_path);
    setenv("PYTHONPATH", file_info.absolutePath().toStdString().c_str(), 1);//对中文支持存疑
    try {
        object my_python_class_module = import(file_info.baseName().toStdString().c_str());//需要不包含.py的文件名
        class_object_ = my_python_class_module.attr(class_name.toStdString().c_str())();
        main_namespace_ = class_object_.attr("__dict__");//声明命名空间,存储变量实例
        return true;
    }
    catch (const error_already_set &) {
        PyErr_Print();
        qCritical()
            << "PythonLoadFile error";
        return false;
    }
}
PythonWork::~PythonWork() {
    Py_Finalize();
}
