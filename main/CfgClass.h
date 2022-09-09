//
// Created by xtx on 2022/9/7.
//

#ifndef QT_CFGCLASS_H
#define QT_CFGCLASS_H
#include <QSettings>

class CfgClass {
public:
    CfgClass();
    QString GetMainCfg(QString key);
    bool SaveMainCfg(QString key,QString value);
private:
    QSettings *configMainIni;
};


#endif //QT_CFGCLASS_H
