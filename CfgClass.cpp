//
// Created by xtx on 2022/9/7.
//

#include "CfgClass.h"

CfgClass::CfgClass() {
    config_main_ini_ = OpenCfg("./config/main.ini");
    QString note = this->GetMainCfg(QString("/Device/Num"));
    device_num_ = note.toInt();//获取设备数量

    config_device_ini_.push_back(config_main_ini_);
    for (int i = 1; i <= device_num_; i++) {
        config_device_ini_.push_back(OpenCfg("./config/Device" + QString::number(i) + ".ini"));
    }

}

/*!
 * 获取主配置文件内的键值
 * @param key 键
 * @return 值
 */
QString CfgClass::GetMainCfg(const QString &key) {
  if (config_main_ini_->contains(key)) {
    QString result = config_main_ini_->value(key).toString();
    return result;
  } else {
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
  if (config_main_ini_->isWritable()) {
    config_main_ini_->setValue(key, value);
    return true;
  } else {
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
    config->setIniCodec("UTF-8");
    return config;
}

/*!
 * 获取一个设备配置文件内的键值
 * @param device_num 设备号
 * @param key 键
 * @return 值
 */
QString CfgClass::GetDeviceCfg(int device_num, const QString &key) {
  if (config_device_ini_[device_num]->contains(key)) {
    QString result = config_device_ini_[device_num]->value(key).toString();
    return result;
  } else {
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
  if (config_device_ini_[num]->isWritable()) {
    config_device_ini_[num]->setValue(key, value);
    return true;
  } else {
    return false;
  }
}
