#include "appdata.h"
#include "quihelper.h"

QStringList AppData::intervals_ = QStringList();
QStringList AppData::datas_ = QStringList();
QStringList AppData::keys_ = QStringList();
QStringList AppData::values_ = QStringList();

QString AppData::send_file_name_ = "send.txt";
void AppData::ReadSendData() {
    //读取发送数据列表
    AppData::datas_.clear();
    QString file_name = QString("%1/%2").arg(QuiHelper::AppPath()).arg(AppData::send_file_name_);
    QFile file(file_name);
    if (file.size() > 0 && file.open(QFile::ReadOnly | QIODevice::Text)) {
        while (!file.atEnd()) {
            QString line = file.readLine();
            line = line.trimmed();
            line = line.replace("\r", "");
            line = line.replace("\n", "");
            if (!line.isEmpty()) {
                AppData::datas_.append(line);
            }
        }

        file.close();
    }

    //没有的时候主动添加点免得太空
    if (AppData::datas_.count() == 0) {
        AppData::datas_ << "Hello";
    }
}

QString AppData::device_file_name_ = "device.txt";
void AppData::ReadDeviceData() {
    //读取转发数据列表
    AppData::keys_.clear();
    AppData::values_.clear();
    QString file_name = QString("%1/%2").arg(QuiHelper::AppPath()).arg(AppData::device_file_name_);
    QFile file(file_name);
    if (file.size() > 0 && file.open(QFile::ReadOnly | QIODevice::Text)) {
        while (!file.atEnd()) {
            QString line = file.readLine();
            line = line.trimmed();
            line = line.replace("\r", "");
            line = line.replace("\n", "");
            if (!line.isEmpty()) {
                QStringList list = line.split(";");
                QString key = list.at(0);
                QString value;
                for (int i = 1; i < list.count(); i++) {
                    value += QString("%1;").arg(list.at(i));
                }

                //去掉末尾分号
                value = value.mid(0, value.length() - 1);
                AppData::keys_.append(key);
                AppData::values_.append(value);
            }
        }

        file.close();
    }
}

void AppData::SaveData(const QString &data) {
    if (data.length() <= 0) {
        return;
    }

    QString file_name = QString("%1/%2.txt").arg(QuiHelper::AppPath()).arg(STRDATETIME);
    QFile file(file_name);
    if (file.open(QFile::WriteOnly | QFile::Text)) {
        file.write(data.toUtf8());
        file.close();
    }
}

void AppData::LoadIp(QComboBox *cbox) {
    //获取本机所有IP
    static QStringList ips;
    if (ips.count() == 0) {
#ifdef emsdk
        ips << "127.0.0.1";
#else
        QList<QNetworkInterface> net_interfaces = QNetworkInterface::allInterfaces();
            foreach (const QNetworkInterface &net_interface, net_interfaces) {
                //移除虚拟机和抓包工具的虚拟网卡
                QString human_readable_name = net_interface.humanReadableName().toLower();
                if (human_readable_name.startsWith("vmware network adapter")
                    || human_readable_name.startsWith("npcap loopback adapter")) {
                    continue;
                }

                //过滤当前网络接口
                bool flag = (net_interface.flags()
                    == (QNetworkInterface::IsUp | QNetworkInterface::IsRunning | QNetworkInterface::CanBroadcast
                        | QNetworkInterface::CanMulticast));
                if (flag) {
                    QList<QNetworkAddressEntry> addrs = net_interface.addressEntries();
                        foreach (QNetworkAddressEntry addr, addrs) {
                            //只取出IPV4的地址
                            if (addr.ip().protocol() == QAbstractSocket::IPv4Protocol) {
                                QString ip_4 = addr.ip().toString();
                                if (ip_4 != "127.0.0.1") {
                                    ips << ip_4;
                                }
                            }
                        }
                }
            }
#endif
    }

    cbox->clear();
    cbox->addItems(ips);
    if (!ips.contains("127.0.0.1")) {
        cbox->addItem("127.0.0.1");
    }
}
