#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qcustomplot.h"
#include "qtmaterialdrawer.h"
#include "CfgClass.h"
#include "qtmaterialtabs.h"
#include "SideBarButton/SideBarButton.h"
#include <string>
#include <QTcpSocket>
#include "Charts/charts.h"
#include "structH.h"

using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow {
Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

    vector<vector<WindowsInfo>> DevicesWindowsInfo;//设备下窗口信息
    vector<DevicesInfo> DevicesInfo;//设备信息
    int DeviceCount;//设备数量



private:
    Ui::MainWindow *ui;
    QCustomPlot *customPlot;
    QtMaterialDrawer *m_drawer;
    SideBarButton *DeviceSelect[100];//侧边栏设备按钮指针
    CfgClass *Cfg;
    QTimer *NewWindowCreateTimer = new QTimer(this);
    ToNewWidget parentInfo;


    void ErrorHandle(const QString &reason);

    void DeviceWindowsInit();

    void DeviceExchange(int DeviceNum);

    void DeviceWindowsExchange(int DeviceNum, int WinNum);

    void NewWindowCreate();


};

#endif // MAINWINDOW_H
