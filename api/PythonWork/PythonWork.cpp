//
// Created by xtx on 2023/11/23.
//
#define BOOST_NUMPY_STATIC_LIB
#include "PythonWork.h"
#include <cstdlib>
#include <iostream>

int setenv(const char *name, const char *value, int overwrite) {
    int errcode = 0;
    if (!overwrite) {
        size_t envsize = 0;
        errcode = getenv_s(&envsize, NULL, 0, name);
        if (errcode || envsize) { return errcode; }
    }
    return _putenv_s(name, value);
}

using namespace boost::python;
PythonWork::PythonWork() {
    setenv("PYTHONPATH", R"(C:\GitProject\QT\cmake-build-debug-visual-studio\config\I2C)", 1);
    setenv("PYTHONHOME", R"(C:\Users\xtx\.vcpkg-clion\vcpkg\packages\python3_x64-windows\tools\python3)", 1);
    try {
        Py_Initialize();

        object my_python_class_module = import("MPU6050");


        object Work = my_python_class_module.attr("Work")();
        object main_namespace = Work.attr("__dict__");
        object test = exec("a=11",main_namespace);

        Work.attr("fuction_28")();
    }

    catch (const error_already_set &) {
        PyErr_Print();

    }
}
