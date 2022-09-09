//
// Created by xtx on 2022/9/7.
//

#include "CfgClass.h"

CfgClass::CfgClass() {
    configMainIni = new QSettings("./config/main.ini", QSettings::IniFormat);
    configMainIni->setIniCodec("GBK");
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

bool CfgClass::SaveMainCfg(QString key, QString value) {
    if (configMainIni->isWritable()) {
        configMainIni->setValue(key, value);
        return true;
    }
    else {
        return false;
    }
}


