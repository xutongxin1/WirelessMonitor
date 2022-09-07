//
// Created by xtx on 2022/9/7.
//

#include "CfgClass.h"

CfgClass::CfgClass() {
    configMainIni = new QSettings("./config/main.ini", QSettings::IniFormat);
}

QString CfgClass::GetMainCfg(QString key) {
    if (configMainIni->contains(key)) {
        QString result = configMainIni->value(key).toString();
        return result;
    }
    else {
        return nullptr;
    }
}


