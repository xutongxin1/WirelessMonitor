#ifndef MAIN__MAINWINDOW_H_
#define MAIN__MAINWINDOW_H_

#include <QMainWindow>
#include "qcustomplot.h"
#include "qtmaterialdrawer.h"
#include "CfgClass.h"
#include "qtmaterialtabs.h"
#include "SideBarButton/SideBarButton.h"
#include <string>
#include <QTcpSocket>
#include "structH.h"

using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
 Q_OBJECT

 public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

    vector<vector<WindowsInfo>> devices_windows_info_;//设备下窗口信息
    vector<DevicesInfo> devices_info_;//设备信息
    int device_count_;//设备数量

 public slots:
    void ReciveOrderExchangeWindow(int device_num, int windows_num);
 private:
    int nowDevice_;
    int nowWindows_;
    Ui::MainWindow *ui_;
    QCustomPlot *custom_plot_;
    QtMaterialDrawer *m_drawer_;
    SideBarButton *device_select_[100];//侧边栏设备按钮指针
    CfgClass *cfg_;
    QTimer *new_window_create_timer_ = new QTimer(this);
    ToNewWidget parent_info_;//用于从父窗口传入到子窗口的所有信息的结构体


//    void ErrorHandle(const QString &reason);

    void DeviceWindowsInit();

    void DeviceExchange(int device_num);

    void DeviceWindowsExchange(int device_num, int windows_num = 1, bool is_init = false);

//    void NewWindowCreate();
    void GetConstructConfig();
    void SaveConstructConfig();

};

#endif // MAIN__MAINWINDOW_H_
