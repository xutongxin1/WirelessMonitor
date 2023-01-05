//
// Created by xtx on 2022/9/7.
//

#ifndef QT_CFGCLASS_H
#define QT_CFGCLASS_H

#include <QSettings>

class CfgClass {
public:
    CfgClass();

    QString GetMainCfg(const QString &key);

    bool SaveMainCfg(const QString &key, const QString &value);

    QString GetDeviceCfg(int DeviceNum, const QString &key);

    bool SaveDeviceCfg(int num, const QString &key, const QString &value);

    std::vector<QSettings*> configDeviceIni;

    int DeviceNum;
private:
    QSettings *configMainIni;


    static QSettings *OpenCfg(const QString &path);

};


#endif //QT_CFGCLASS_H
