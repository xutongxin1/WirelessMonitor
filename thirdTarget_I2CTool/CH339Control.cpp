//
// Created by xtx on 24-8-3.
//

#include "CH339Control.h"
#include <QHttpServerResponse>
bool convertHexStringToUInt8Array(const QString &input, uint8_t *output, int maxLength, int &actualLength) {
    actualLength = 0;
    memset(output, 0, maxLength);
    // 正则表达式匹配类似 "0x00 0x04" 的模式
    QRegularExpression regex(R"(\b0x[0-9a-fA-F]{2}\b)");
    QRegularExpressionMatchIterator matchIterator = regex.globalMatch(input);

    while (matchIterator.hasNext()) {
        QRegularExpressionMatch match = matchIterator.next();
        if (match.hasMatch()) {
            // 提取匹配到的字符串，并转换为uint8_t
            bool ok;
            uint8_t value = match.captured().toUInt(&ok, 16); // 将字符串转为16进制数
            if (!ok || actualLength >= maxLength) {
                return false;
            }
            output[actualLength++] = value;
        } else {
            return false; // 如果发现无法转换的字符串，直接返回false
        }
    }

    return actualLength > 0; // 至少要有一个成功转换的字节
}

bool convertSingleHexStringToUInt8(const QString &input, uint8_t &output) {
    // 正则表达式匹配类似 "0x00" 的模式
    QRegularExpression regex(R"(\b0x[0-9a-fA-F]{2}\b)");
    QRegularExpressionMatch match = regex.match(input);

    // 如果正则表达式没有匹配到，或者匹配超过一个
    if (!match.hasMatch() || input.count(regex) != 1) {
        return false;
    }

    // 转换为uint8_t
    bool ok;
    output = match.captured().toUInt(&ok, 16);

    // 检查转换是否成功
    return ok;
}

//枚举设备
ULONG IICDeviceEach::EnumDevice() {
    ULONG i, oLen, DevCnt = 0;
    USB_DEVICE_DESCRIPTOR DevDesc = {0};
    CHAR tem[256] = "";
    mDeviceInforS DevInfor = {0};

    ui_->DeviceComboBox->clear();
    // SendDlgItemMessage(SpiI2cGpioDebugHwnd,IDC_ObjList,CB_RESETCONTENT,0,0);
    for (i = 0; i < 16; i++) {
        if (CH347OpenDevice(i) != INVALID_HANDLE_VALUE) {
            oLen = sizeof(USB_DEVICE_DESCRIPTOR);
            CH347GetDeviceInfor(i, &DevInfor);

            if (DevInfor.ChipMode == 3) //模式3此接口为JTAG/I2C
                continue;

            // std::cout<<(tem,"%d# %s",i,DevInfor.FuncDescStr);
            ui_->DeviceComboBox->addItem(DevInfor.FuncDescStr);
            // SendDlgItemMessage(SpiI2cGpioDebugHwnd,IDC_ObjList,CB_ADDSTRING,0,(LPARAM)(LPCTSTR)tem);
            // memcpy(&SpiI2cDevInfor[DevCnt],&DevInfor,sizeof(DevInfor));
            DevCnt++;
        }
        CH347CloseDevice(i);
    }
    if (DevCnt) {
        // SendDlgItemMessage(SpiI2cGpioDebugHwnd,IDC_ObjList,CB_SETCURSEL,0,0);
        // SetFocus(GetDlgItem(SpiI2cGpioDebugHwnd,IDC_ObjList));
    }
    return DevCnt;
}

//打开设备
BOOL IICDeviceEach::OpenDevice() {
    //获取设备序号
    uint8_t SpiI2cGpioDevIndex = ui_->DeviceComboBox->currentIndex();
    if (SpiI2cGpioDevIndex == CB_ERR) {
        qDebug() << ("打开设备失败,请先选择设备");
        return false;
    }
    isOpened = (CH347OpenDevice(SpiI2cGpioDevIndex) != INVALID_HANDLE_VALUE);
    CH347SetTimeout(SpiI2cGpioDevIndex, 500, 500);
    qDebug() << (">>Open the device...%s", isOpened ? "Success" : "Failed");
    if (isOpened) {
        ui_->DeviceComboBox->setEnabled(false);
        ui_->ConnectCH->setText("断开连接");
        disconnect(ui_->ConnectCH, &QPushButton::clicked, this, &IICDeviceEach::OpenDevice);
        connect(ui_->ConnectCH, &QPushButton::clicked, this, &IICDeviceEach::CloseDevice);

        bool DevIsOpened = CH347I2C_Set(SpiI2cGpioDevIndex, 0);
        qDebug() << DevIsOpened;
        return true;
    }
    return false;
}

//关闭设备
BOOL IICDeviceEach::CloseDevice() {
    CH347CloseDevice(ui_->DeviceComboBox->currentIndex());
    // if (IntIsEnable)
    //     SendDlgItemMessage(SpiI2cGpioDebugHwnd, IDC_DisableIntNotify,BM_CLICK, 0, 0);
    // IntIsEnable = FALSE;
    // DevIsOpened = FALSE;
    // DbgPrint(">>Close the Device");

    ui_->ConnectCH->setText("连接设备");
    ui_->DeviceComboBox->setEnabled(true);
    disconnect(ui_->ConnectCH, &QPushButton::clicked, this, &IICDeviceEach::CloseDevice);
    connect(ui_->ConnectCH, &QPushButton::clicked, this, &IICDeviceEach::OpenDevice);
    return TRUE;
}

// 添加此处对齐SPI初始化操作
BOOL IICDeviceEach::CH347InitI2C() {
    BOOL RetVal = FALSE;
    BOOL isStrentch = FALSE;
    ULONG iMode; // I2C模式
    ULONG I2CDelayMs = 50;

    // iMode = SendDlgItemMessage(SpiI2cGpioDebugHwnd, IDC_I2CCfg_Clock,CB_GETCURSEL, 0, 0);
    // isStrentch = SendDlgItemMessage(SpiI2cGpioDebugHwnd, IDC_I2CCfg_SclStretch,CB_GETCURSEL, 0, 0) ? FALSE : TRUE;
    // I2CDelayMs = GetDlgItemInt(SpiI2cGpioDebugHwnd, IDC_I2CCfg_Delay,NULL,FALSE);
    switch (ui_->SpeedChooseCombo->currentIndex()) {
        case 0:
            iMode = 00;
            break;
        default:
        case 1:
            iMode = 01;
            break;
        case 2:
            iMode = 10;
            break;
        case 3:
            iMode = 11;
            break;
    }
    qDebug() << ui_->DeviceComboBox->currentIndex();
    RetVal = CH347I2C_Set(ui_->DeviceComboBox->currentIndex(), iMode);
    qDebug("CH347I2C Set clock %s", RetVal ? "succ" : "failure");

    RetVal = CH347I2C_SetStretch(ui_->DeviceComboBox->currentIndex(), false);
    qDebug("CH347 I2C set stetching %s", RetVal ? "succ" : "failure");

    if (I2CDelayMs > 0)
        RetVal = CH347I2C_SetDelaymS(ui_->DeviceComboBox->currentIndex(), I2CDelayMs);

    qDebug("CH347InitI2C %s", RetVal ? "succ" : "failure");

    uint8_t Data[2] = {0x08, 0x00};
    uint8_t rec[6] = {0};
    CH347ReadI2C(0x71, 0x00, rec, 1);

    CH347WriteI2C(0x70, 0xe1, Data, 2);
    QThread::msleep(500);
    CH347ReadI2C(0x71, 0x00, rec, 1);

    uint8_t Data2[2] = {0x33, 0x00};
    CH347WriteI2C(0x70, 0xac, Data2, 2);
    QThread::msleep(500);
    CH347ReadI2C(0x71, 0x00, rec, 5);

    uint8_t databuff[6];
    databuff[0] = rec[1];
    databuff[1] = rec[2];
    databuff[2] = rec[3];
    databuff[3] = rec[4];
    databuff[4] = 0x00;
    uint32_t SRH = (databuff[0] << 12) + (databuff[1] << 4) + (databuff[2] >> 4);
    uint32_t ST = ((databuff[2] & 0X0f) << 16) + (databuff[3] << 8) + (databuff[4]);
    float humidity = (int) (SRH * 100.0 / 1024 / 1024 + 0.5);
    float temperature = ((int) (ST * 2000.0 / 1024 / 1024 + 0.5)) / 10.0 - 50;
    qDebug() << humidity << temperature;
    return RetVal;
}

BOOL IICDeviceEach::CH347WriteI2C(uint8_t DeviceAddr, uint8_t DataAddr, uint8_t *Data, uint8_t DataLen) {
    if (!isOpened) {
        return false;
    }
    UCHAR InBuf[1] = {0};
    uint8_t OutBuf[512] = {0};
    // 合并 DeviceAddr
    memcpy(&OutBuf, &DeviceAddr, 1);
    // 合并 DataAddr
    memcpy(&OutBuf[1], &DataAddr, 1);
    // 合并 Data
    memcpy(&OutBuf[2], Data, DataLen);
    qDebug() << "CH347WriteI2C" << ui_->DeviceComboBox->currentIndex();
    CH347StreamI2C(ui_->DeviceComboBox->currentIndex(), DataLen, OutBuf, 0, InBuf);

    return true;
}

BOOL IICDeviceEach::CH347ReadI2C(uint8_t DeviceReadAddr, uint8_t DataAddr, uint8_t *ReadData, uint8_t ReadDataLen) {
    if (!isOpened) {
        return false;
    }
    uint8_t OutBuf[256] = {0};
    // 合并 DeviceAddr
    memcpy(&OutBuf, &DeviceReadAddr, 1);
    if (DataAddr != 0) {
        // 合并 DataAddr
        memcpy(&OutBuf + 1, &DataAddr, 1);
        CH347StreamI2C(ui_->DeviceComboBox->currentIndex(), 2, OutBuf, ReadDataLen, ReadData);
    } else {
        CH347StreamI2C(ui_->DeviceComboBox->currentIndex(), 1, OutBuf, ReadDataLen, ReadData);
    }

    return true;
}
void IICDeviceEach::AddRoute() {
    python_work_->server.route("/WriteI2C/<arg>/<arg>/<arg>/<arg>",
                               [&](const QString &DeviceReadAddrT, const QString &DataAddrT, const QString &DataT,
                                   const qint32 DataLenT) {
                                   uint8_t DeviceReadAddr = 0;
                                   uint8_t DataAddr = 0;
                                   uint8_t Data[512] = {0};
                                   int DataLen = 0;
                                   if (!convertSingleHexStringToUInt8(DeviceReadAddrT, DeviceReadAddr)) {
                                       return "Invalid DeviceWriteAddr";
                                   }
                                   if (!convertSingleHexStringToUInt8(DataAddrT, DataAddr)) {
                                       return "Invalid DataAddr";
                                   }
                                   if (!convertHexStringToUInt8Array(DataT, Data, 512, DataLen)) {
                                       return "Invalid Data";
                                   }
                                   if (DataLen != DataLenT) {
                                       return "Invalid DataLen";
                                   }
                                   if (!this->CH347WriteI2C(DeviceReadAddr, DataAddr, Data, DataLen)) {
                                       return "WriteI2C failed";
                                   }
                                   return "WriteI2C success";
                               });
    python_work_->server.route("/ReadI2C/<arg>/<arg>/<arg>",
                               [&](const QString &DeviceReadAddrT, const QString &DataAddrT,
                                   const qint32 DataLen) {
                                   uint8_t DeviceReadAddr = 0;
                                   uint8_t DataAddr = 0;
                                   uint8_t Data[512] = {0};
                                   QString ret;
                                   if (!convertSingleHexStringToUInt8(DeviceReadAddrT, DeviceReadAddr)) {
                                       ret = "Invalid DeviceReadAddr";
                                   } else if (!convertSingleHexStringToUInt8(DataAddrT, DataAddr)) {
                                       ret = "Invalid DataAddr";
                                   } else if (!this->CH347ReadI2C(DeviceReadAddr, DataAddr, Data, DataLen)) {
                                       ret = "ReadI2C failed";
                                   } else {
                                       for (int i = 0; i < DataLen; i++) {
                                           ret += QString("0x%1-").arg(Data[i], 2, 16, QChar('0'));
                                       }
                                       ret.chop(1);
                                   }
                                   return ret;
                               });
}
