//
// Created by xtx on 2022/9/7.
//

#include "CfgClass.h"

CfgClass::CfgClass() {
    configMainIni = OpenCfg("./config/main.ini");
    QString Note = this->GetMainCfg(QString("/Device/Num"));
    DeviceNum = Note.toInt();//获取设备数量

    configDeviceIni.push_back(configMainIni);
    for (int i = 1; i <= DeviceNum; i++) {
        configDeviceIni.push_back(OpenCfg("./config/Device" + QString::number(i) + ".ini"));
    }

}

QString CfgClass::GetMainCfg(const QString &key) {
    if (configMainIni->contains(key)) {
        QString result = configMainIni->value(key).toString();
        return result;
    }
    else {
        return nullptr;
    }
}

bool CfgClass::SaveMainCfg(const QString &key, const QString &value) {
    if (configMainIni->isWritable()) {
        configMainIni->setValue(key, value);
        return true;
    }
    else {
        return false;
    }
}

QSettings *CfgClass::OpenCfg(const QString &path) {
    QSettings *config = new QSettings(path, QSettings::IniFormat);
    config->setIniCodec("GBK");
    return config;
}

QString CfgClass::GetDeviceCfg(int num, const QString &key) {
    if (configDeviceIni[num]->contains(key)) {
        QString result = configDeviceIni[num]->value(key).toString();
        return result;
    }
    else {
        return nullptr;
    }
}

bool CfgClass::SaveDeviceCfg(int num, const QString &key, const QString &value) {
    if (configDeviceIni[num]->isWritable()) {
        configDeviceIni[num]->setValue(key, value);
        return true;
    }
    else {
        return false;
    }
}
