#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qcustomplot.h"
#include "qtmaterialdrawer.h"
#include "CfgClass.h"
#include "qtmaterialtabs.h"
#include <string>
using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
enum WindowsType
{
    None,
    ChannelConfiguration
};
struct WindowsInfo//窗口配置信息结构体
{
    QWidget *widget;
    int index;
    WindowsType type;
};
struct DevicesInfo//设备配置信息结构体
{
    int windowsNum;
    int TabIndex;
    QtMaterialTabs *TabWidget;
};
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    vector<vector<WindowsInfo>> DevicesWindowsInfo;//设备下窗口信息
    vector<DevicesInfo> DevicesInfo;//设备信息
    int DeviceNum;//设备数量



private:
    Ui::MainWindow *ui;
    QCustomPlot *customPlot;
    QtMaterialDrawer *m_drawer;
    QWidget *DeviceSelect[100];//侧边栏设备按钮指针
    CfgClass *Cfg;
    void ErrorHandle(const QString& reason);
    void DeviceWindowsInit();
    void DeviceExchange(int DeviceNum);
    void DeviceWindowsExchange(int DeviceNum, int WinNum);
};
#endif // MAINWINDOW_H
