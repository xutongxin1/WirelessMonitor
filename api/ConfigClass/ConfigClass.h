//
// Created by xtx on 2022/9/7.
//

#ifndef MAIN__CFGCLASS_H_
#define MAIN__CFGCLASS_H_

#include <QSettings>

class ConfigClass : public QSettings {

//  QString GetMainCfg(const QString &key);
//
//  bool SaveMainCfg(const QString &key, const QString &value);
//
//  QString GetDeviceCfg(int device_num, const QString &key);
//
//    bool SaveDeviceCfg(int num, const QString &key, const QString &value);
 public:
    ConfigClass(const QString &file_name, Format format, QObject *parent = nullptr);

};

#endif //MAIN__CFGCLASS_H_
