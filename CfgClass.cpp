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

/*!
 * 获取主配置文件内的键值
 * @param key 键
 * @return 值
 */
QString CfgClass::GetMainCfg(const QString &key) {
    if (configMainIni->contains(key)) {
        QString result = configMainIni->value(key).toString();
        return result;
    }
    else {
        return nullptr;
    }
}

/*!
 * 向主配置文件写入键值
 * @param key 键
 * @param value 值
 * @return 写入是否成功
 */
bool CfgClass::SaveMainCfg(const QString &key, const QString &value) {
    if (configMainIni->isWritable()) {
        configMainIni->setValue(key, value);
        return true;
    }
    else {
        return false;
    }
}

/*!
 * 打开一个配置文件
 * @param path 配置文件目录
 * @return 配置文件指针
 */
QSettings *CfgClass::OpenCfg(const QString &path) {
    QSettings *config = new QSettings(path, QSettings::IniFormat);
    config->setIniCodec("GBK");
    return config;
}

/*!
 * 获取一个设备配置文件内的键值
 * @param num 设备号
 * @param key 键
 * @return 值
 */
QString CfgClass::GetDeviceCfg(int num, const QString &key) {
    if (configDeviceIni[num]->contains(key)) {
        QString result = configDeviceIni[num]->value(key).toString();
        return result;
    }
    else {
        return nullptr;
    }
}

/*!
 * 向一个设备的配置文件写入键值
 * @param num 设备号
 * @param key 键
 * @param value 值
 * @return 写入是否成功
 */
bool CfgClass::SaveDeviceCfg(int num, const QString &key, const QString &value) {
    if (configDeviceIni[num]->isWritable()) {
        configDeviceIni[num]->setValue(key, value);
        return true;
    }
    else {
        return false;
    }
}
