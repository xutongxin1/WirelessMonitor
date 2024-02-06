#include "mainwindow.h"

#include "./ui_mainwindow.h"
#include "ChannelConfiguration/channelconfiguration.h"
#include "ComTool/Comtool.h"
#include "DataCirculation/datacirculation.h"
#include "qcustomplot.h"
#include "qtmaterialappbar.h"
#include "qtmaterialdrawer.h"
#include "SideBarButton/SideBarButton.h"
#include "TCPBridgeConfiguration/tcpbridgeconfiguration.h"
#include "TCPCom/TCPCom.h"
#include "Charts/charts_next.h"
#include "IICDeviceBasic/IICDeviceBasic.h"

int record_DeviceNum = 0, record_WinNum = 0;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui_(new Ui::MainWindow) {
    ui_->setupUi(this);
    //    QVBoxLayout *layout = new QVBoxLayout;
    //    this->setLayout(layout);
    //    QWidget *widgetTmp = new QWidget;
    //    layout->addWidget(widgetTmp);
    //
    //    QWidget *canvas = new QWidget;
    //    canvas->setStyleSheet("QWidget { background: white; }");
    //    layout->addWidget(canvas);
    //
    //    layout->setContentsMargins(20, 20, 20, 20);
    //
    //    layout = new QVBoxLayout;
    //    canvas->setLayout(layout);
    //    canvas->setMaximumHeight(300);
    //
    //手动初始化侧边栏（不是在ui文件里初始化的）


#ifdef BEAUTIFY
    ui_->centralwidget->setStyleSheet(".QWidget{border-image: url(config/backgroud.png);}");
    ui_->FunctionBar->setBackgroundColor(QColor(245, 180, 202, 0));
#endif
    m_drawer_ = new QtMaterialDrawer;

    InitConfig();

    m_drawer_->setParent(ui_->centralwidget);
    m_drawer_->setClickOutsideToClose(true);
    m_drawer_->setOverlayMode(true);
    m_drawer_->setDrawerWidth(250);
    m_drawer_->setAutoRaise(true);

    snackbar_->setAutoHideDuration(1000);


    //初始化侧边栏的布局
    auto *drawer_layout = new QVBoxLayout;
    m_drawer_->setDrawerLayout(drawer_layout);


    //往侧边栏添加第一个按钮（数据聚合窗口）
    device_select_[0] = new SideBarButton();
    drawer_layout->addWidget(device_select_[0]);  // 初始化数据聚合窗口

    //绑定每个按钮的点击事件
    for (int i = 1; i <= device_count_; i++) {
        device_select_[i] = new SideBarButton(i, config_device_ini_[i]);
        drawer_layout->addWidget(device_select_[i]);
        connect(device_select_[i]->button_, &QPushButton::pressed, this, [=] {
          DeviceWindowsExchange(i);
        });
    }
    connect(ui_->settingButton, SIGNAL(pressed()), m_drawer_, SLOT(openDrawer()));

    GetConstructConfig();
    version_=config_main_ini_->value("/Device/Version").toString();
    if(VERSION!=version_)
    {
        config_main_ini_->setValue("/Device/Version",VERSION);
    }
    this->setWindowTitle(this->windowTitle()+" "+version_);
    this->setWindowIcon(QIcon("./config/Icon.ico"));

    DeviceWindowsInit();

    if (now_windows_ != 0 && now_device_ != 0) {
        DeviceWindowsExchange(now_windows_, now_device_, true);
    }


    //DeviceExchange(1);
    //    DeviceWindowsExchange(1, 1);

//    IICDeviceBasic *tmp = new IICDeviceBasic(1,
//                                               5,
//                                               config_device_ini_[1],
//                                               &parent_info_);
//    ui_->FunctionWindow->setCurrentIndex(ui_->FunctionWindow->addWidget(tmp));

    //    ui_->FunctionWindow->setCurrentIndex(2);
    //    devices_info_[1].tab_widget->setCurrentTab(2);
    //ui_->FunctionBar->setStyleSheet("background-color: rgba(255, 255, 255, 50);");
}

MainWindow::~MainWindow() {
    delete ui_;
}

/*!
 * 错误处理函数,会弹出窗口并结束程序
 * @param reason 出错提示
 */
//void MainWindow::ErrorHandle(const QString &reason) {
//  int ret = QMessageBox::warning(this, QStringLiteral("Error!"), reason, QMessageBox::Cancel | QMessageBox::Ok);
//  std::exit(0);
//}

/*!
 * 窗口结构体初始化
 */
void MainWindow::DeviceWindowsInit() {
    parent_info_.main_window=this;
    parent_info_.devices_info = &devices_info_;
    parent_info_.devices_windows_info = &devices_windows_info_;

    devices_windows_info_.emplace_back();  // 空占位
    devices_info_.emplace_back();                                     // Main窗口
    devices_info_[0].tab_index = 0;                                    // 在ui内默认创建,必定是0
    for (int device_num = 1; device_num <= device_count_; device_num++)  // 设备遍历初始化
    {
        // 创建Tab栏,初始化DevicesInfo内数据
        auto *new_tab = new QtMaterialTabs();//上方切换栏
        struct DevicesInfo tmp
            {
                .connect_mode=config_device_ini_[device_num]->value("/All/connect_mode").toInt(),
                .windows_num = config_device_ini_[device_num]->value("/All/win").toInt(),
                .tab_index = ui_->TabStackedWidget->addWidget(new_tab), .tab_widget = new_tab
            };
        devices_info_.push_back(tmp);
        devices_windows_info_.emplace_back();  // 创建行

        // 创建socket对象
        devices_info_[device_num].tcp_info_handler[1] = new TCPInfoHandle;
        devices_info_[device_num].tcp_info_handler[2] = new TCPInfoHandle;
        devices_info_[device_num].tcp_info_handler[3] = new TCPInfoHandle;
        devices_info_[device_num].tcp_command_handler = new TCPCommandHandle;  // 都创建一个socket对象吧，防止空指针
        for (int win_num = 1; win_num <= devices_info_[device_num].windows_num; win_num++)  // 窗口遍历初始化
        {
            int win_type = config_device_ini_[device_num]->value("/Win" + QString::number(win_num) + "/type").toInt();
            if (win_type == 0) {
                continue;
            }
            devices_windows_info_[device_num].emplace_back();  // 0位置空占位
            devices_windows_info_[device_num].emplace_back();
            RepeaterWidget *tmp_widget = nullptr;
            switch (win_type) {
                case 1:devices_windows_info_[device_num][win_num].type = CHANNEL_CONFIGURATION;  // 结构体初始化
                    tmp_widget = new ChannelConfiguration(
                        device_num, config_device_ini_[0], config_device_ini_[device_num], &parent_info_);
                    devices_windows_info_[device_num][win_num].widget = tmp_widget;
                    ConnectSingal(tmp_widget);
                    devices_windows_info_[device_num][win_num].index =
                        ui_->FunctionWindow->addWidget(devices_windows_info_[device_num][win_num].widget);
                    devices_info_[device_num].tab_widget->addTab("通道配置");  // 添加tab栏
                    break;
                case 50:devices_windows_info_[device_num][win_num].type = COM_TOOL;  // 结构体初始化
                    tmp_widget =
                        new ComTool(device_num, win_num, config_device_ini_[device_num], &parent_info_);
                    ConnectSingal(tmp_widget);
                    devices_windows_info_[device_num][win_num].widget = tmp_widget;
                    devices_windows_info_[device_num][win_num].index =
                        ui_->FunctionWindow->addWidget(devices_windows_info_[device_num][win_num].widget);
                    devices_info_[device_num].tab_widget->addTab("本地串口监视器");  // 添加tab栏
                    break;
                case 51: {  // 记录下相应的变量，方便提取类的成员变量charts
                    record_DeviceNum = device_num;
                    record_WinNum = win_num;

                    devices_windows_info_[device_num][win_num].type = MAIN_CHART;  // 结构体初始化
                    ChartsNext
                        *test_1 =
                        new ChartsNext(device_num, win_num, config_device_ini_[device_num], &parent_info_);
                    devices_windows_info_[device_num][win_num].widget = test_1;
                    devices_windows_info_[device_num][win_num].index =
                        ui_->FunctionWindow->addWidget(devices_windows_info_[device_num][win_num].widget);
                    devices_info_[device_num].tab_widget->addTab("数据波形图");  // 添加tab栏

                    // 测试数据添加
                    // lulu_test
                    QVector<double> a;
                    test_1->RegisterDataPoint("test");
                    break;
                }
                case 52:devices_windows_info_[device_num][win_num].type = DATA_CIRCULATION;  // 结构体初始化

                    tmp_widget =
                        new DataCirculation(device_num, win_num, config_device_ini_[device_num], &parent_info_);
                    ConnectSingal(tmp_widget);
                    devices_windows_info_[device_num][win_num].widget = tmp_widget;
                    devices_windows_info_[device_num][win_num].index =
                        ui_->FunctionWindow->addWidget(devices_windows_info_[device_num][win_num].widget);
                    devices_info_[device_num].tab_widget->addTab("数据流过滤器配置");  // 添加tab栏
                    break;
                case 201:devices_windows_info_[device_num][win_num].type = TCP_BRIDGE_CONFIGURATION;  // 结构体初始化

                    tmp_widget =
                        new TCPBridgeConfiguration(device_num,
                                                   win_num,
                                                   config_device_ini_[device_num],
                                                   &parent_info_);
                    devices_windows_info_[device_num][win_num].widget = tmp_widget;
                    ConnectSingal(tmp_widget);

                    devices_windows_info_[device_num][win_num].index =
                        ui_->FunctionWindow->addWidget(devices_windows_info_[device_num][win_num].widget);
                    devices_info_[device_num].tab_widget->addTab("串口桥配置");  // 添加tab栏
                    break;
                case 202:devices_windows_info_[device_num][win_num].type = TCP_COM;  // 结构体初始化
                    devices_windows_info_[device_num][win_num].widget =
                        new TCPCom(device_num, win_num, config_device_ini_[device_num], &parent_info_);
                    devices_windows_info_[device_num][win_num].index =
                        ui_->FunctionWindow->addWidget(devices_windows_info_[device_num][win_num].widget);
                    devices_info_[device_num].tab_widget->addTab("串口桥数据监视器");  // 添加tab栏
                    break;
                case 701:devices_windows_info_[device_num][win_num].type = IIC_Basic;  // 结构体初始化

                    tmp_widget = new IICDeviceBasic(device_num, win_num, config_device_ini_[device_num], &parent_info_);
                    devices_windows_info_[device_num][win_num].widget = tmp_widget;
                    ConnectSingal(tmp_widget);
                    devices_windows_info_[device_num][win_num].index =
                        ui_->FunctionWindow->addWidget(devices_windows_info_[device_num][win_num].widget);
                    devices_info_[device_num].tab_widget->addTab("I2C基础数据监视器");  // 添加tab栏
                    break;

                default:qCritical() << "配置了未知的窗口类型";
                    break;
            }
        }
#ifdef BEAUTIFY
        new_tab->setBackgroundColor(QColor(123, 169, 199, 50));
#endif

        //        Charts test;  //图标界面测试
        //        test.show();
        // tab栏绑定
        connect(new_tab, &QtMaterialTabs::currentChanged, this, [=, this](int num) {
          // 这里的num从0开始，所以要+1
          DeviceWindowsExchange(device_num, num + 1);
        });

//        connect(new_window_create_timer_, &QTimer::timeout, this, [=] {
//
//        });
//        connect(new_window_create_timer_, &QTimer::timeout, this, [=] {
//
//        });
    }
}

/*!
 * 设备切换,被侧边栏切换
 * @param num 设备号
 */
//void MainWindow::DeviceExchange(int device_num) {
//
//  DeviceWindowsExchange(device_num, 1);
//}

/*!
 * 设备内Tab窗口切换
 * @param device_num 设备号
 * @param win_num 窗口号
 */
void MainWindow::DeviceWindowsExchange(int device_num, int windows_num, bool is_init) {
    if (devices_windows_info_[device_num][windows_num].index == -1) {
        qCritical("尝试打开不存在的窗口");
        return;
    }
    qDebug("try to switch %d device, %d windows", device_num, windows_num);
    if (now_device_ != device_num || is_init) {
        ui_->TabStackedWidget->setCurrentIndex(devices_info_[device_num].tab_index);
        now_device_ = device_num;
    }
    now_windows_ = windows_num;
#if DEBUG
#else
    if (devices_info_[device_num].config_step < win_num) {
        //TODO:添加提醒用户完成上一步骤
        qDebug("Switch Failed");
        devices_info_[device_num].tab_widget->setCurrentTab(devices_info_[device_num].current_window - 1,
                                                            false);//把高亮回到该窗口
        QMessageBox::information(this, tr("提示"), tr("请先完成上一步骤的配置吧"));
        return;
    }
#endif
    ui_->FunctionWindow->setCurrentIndex(devices_windows_info_[device_num][windows_num].index);
    devices_info_[device_num].current_window = windows_num;
    if (is_init) {
        devices_info_[device_num].tab_widget->setCurrentTab(windows_num - 1,
                                                            false);//调整上面tab的视觉效果
    }
}

void MainWindow::ReceiveOrderExchangeWindow(int device_num, int windows_num) {
    DeviceWindowsExchange(device_num, windows_num);
    devices_info_[device_num].tab_widget->setCurrentTab(windows_num - 1,
                                                        false);//调整上面tab的视觉效果
}

void MainWindow::GetConstructConfig() {
    now_device_ = config_main_ini_->value("/Device/LastDevice").toInt();
    now_windows_ = config_main_ini_->value("/Device/LastWindows").toInt();
}

void MainWindow::SaveConstructConfig() {
    config_main_ini_->setValue("/Device/LastDevice", QString::number(now_device_));
    config_main_ini_->setValue("/Device/LastWindows", QString::number(now_windows_));
}
void MainWindow::ConnectSingal(RepeaterWidget *tmp_widget) const {
    connect(tmp_widget,
            &RepeaterWidget::OrderExchangeWindow,
            this,
            &MainWindow::ReceiveOrderExchangeWindow);//绑定切换窗口的有关事件
    connect(tmp_widget,
            &RepeaterWidget::OrderShowSnackbar,
            this,
            &MainWindow::ReceiveOrderShowSnackbar);//绑定弹出提示的有关事件

}
void MainWindow::ReceiveOrderShowSnackbar(const QString &message) {
    snackbar_->addMessage(message);
}

void MainWindow::InitConfig() {
    config_main_ini_ = new ConfigClass("main.ini", QSettings::IniFormat);
    device_count_ = config_main_ini_->value("/Device/device_num").toInt();
    config_device_ini_.emplace_back();
    for (int i = 1; i <= device_count_; i++) {
        config_device_ini_.push_back(new ConfigClass("Device" + QString::number(i) + ".ini",
                                                     QSettings::IniFormat));
//        QSettings *tmp=new QSettings("config/Device" + QString::number(i) + ".ini",
//                                       QSettings::IniFormat);
//        config_device_ini_[i]->setValue("/ee/connect_mode",123);
//        config_device_ini_[i]->sync();

    }

}
