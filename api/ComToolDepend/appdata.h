#ifndef MAIN_API_COMTOOLDEPEND_APPDATA_H_
#define MAIN_API_COMTOOLDEPEND_APPDATA_H_

#include "head.h"

class AppData {
 public:
  //全局变量
  static QStringList intervals_;
  static QStringList datas_;
  static QStringList keys_;
  static QStringList values_;

  //读取发送数据列表
  static QString send_file_name_;
  static void ReadSendData();

  //读取转发数据列表
  static QString device_file_name_;
  static void ReadDeviceData();

  //保存数据到文件
  static void SaveData(const QString &data);

  //添加网卡IP地址到下拉框
  static void LoadIp(QComboBox *cbox);
};

#endif // MAIN_API_COMTOOLDEPEND_APPDATA_H_
