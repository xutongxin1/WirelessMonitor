//
// Created by xtx on 2022/9/7.
//

#ifndef MAIN__CFGCLASS_H_
#define MAIN__CFGCLASS_H_

#include <QSettings>

class CfgClass {
 public:
  CfgClass();

  QString GetMainCfg(const QString &key);

  bool SaveMainCfg(const QString &key, const QString &value);

  QString GetDeviceCfg(int device_num, const QString &key);

    bool SaveDeviceCfg(int num, const QString &key, const QString &value);

  std::vector<QSettings *> config_device_ini_;

  int device_num_;
private:
  QSettings *config_main_ini_;


    static QSettings *OpenCfg(const QString &path);

};

#endif //MAIN__CFGCLASS_H_
