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
    setenv("PYTHONPATH", R"(C:\GitProject\WirelessMonitor\thirdTarget_I2CTool\config\AHT10)", 1);
    // setenv("PYTHONPATH", R"(C:/Path/miniconda3/DLLs)", 0);
    setenv("PYTHONHOME", "C:/Path/miniconda3", 1);

    try {
        Py_Initialize();

        server.listen(QHostAddress::Any, 19230);
        qInfo() << "Python Initialize success";
        qDebug()<< "Server 127.0.0.1 listening on port 19230";

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
        exec((val_name + "=\"" + val + "\"").toStdString().c_str(), main_namespace_);
        return true;
    }
    catch (const error_already_set &) {
        PyErr_Print();
        qCritical()
            << "PythonValueSave error";
        return false;
    }
}
QString PythonWork::PythonRunUserDef(const QString &def_name) {
    try {
        // object ret = class_object_.attr(def_name.toStdString().c_str());
        // // std::string tmp = extract<std::string>(ret);//意外报错，但是不影响编译与功能运行
        // QString tmp=QString::fromStdString(boost::python::extract<std::string>(boost::python::str(ret())));
        // // return QString::fromStdString(tmp);
        // return tmp;

        // 获取方法对象
        boost::python::object method = class_object_.attr(def_name.toStdString().c_str());

        // 调用方法并获取返回值
        boost::python::object ret = method();  // 这里调用方法并获取返回值

        // 将返回值转换为字符串
        std::string tmp = boost::python::extract<std::string>(boost::python::str(ret));

        // 返回 QString 类型的字符串
        return QString::fromStdString(tmp);

    }
    catch (const error_already_set &) {
        PyErr_Print();
        qInfo()
            << "PythonRunUserDef not worked";
        return {};
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
        // Py_SetPath(L"C:\\Path\\miniconda3;C:/GitProject/QT/thirdTarget_I2CTool/config/AHT10/lib");
        // object socket_module = import("_socket");
        // PyRun_SimpleString("import sys; sys.path.append('C:/Path/miniconda3/DLLs')");
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

bool PythonWork::PythonExec(const QString &val_name) {
    try {
        exec(val_name.toStdString().c_str(), main_namespace_);
        return true;
    }
    catch (const error_already_set &) {
        PyErr_Print();
        qCritical()
            << "PythonValueSave error";
        return false;
    }
}