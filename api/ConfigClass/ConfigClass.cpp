//
// Created by xtx on 2022/9/7.
//

#include "ConfigClass.h"

ConfigClass::ConfigClass(const QString &file_name, QSettings::Format format, QObject *parent) : QSettings(
    "config/" + file_name,
    format,
    parent) {

}
