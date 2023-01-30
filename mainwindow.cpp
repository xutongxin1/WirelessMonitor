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
#include "TCPCom/Tcpcom.h"
#include "Charts/charts_next.h"

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
    //
    m_drawer_ = new QtMaterialDrawer;
    cfg_ = new CfgClass;
    m_drawer_->setParent(ui_->centralwidget);
    m_drawer_->setClickOutsideToClose(true);
    m_drawer_->setOverlayMode(true);
    m_drawer_->setDrawerWidth(250);

    auto *drawer_layout = new QVBoxLayout;
    m_drawer_->setDrawerLayout(drawer_layout);

    device_select_[0] = new SideBarButton();
    drawer_layout->addWidget(device_select_[0]);  // 初始化数据聚合窗口

    device_count_ = cfg_->device_num_;
    for (int i = 1; i <= device_count_; i++) {
        device_select_[i] = new SideBarButton(i, cfg_);
        drawer_layout->addWidget(device_select_[i]);
        connect(device_select_[i]->button_, &QPushButton::pressed, this, [=] { MainWindow::DeviceExchange(i); });
    }
    connect(ui_->settingButton, SIGNAL(pressed()), m_drawer_, SLOT(openDrawer()));

    DeviceWindowsInit();

    DeviceExchange(1);
    //    DeviceWindowsExchange(1, 1);

    //    DataCirculation *tmp = new DataCirculation(1,
    //                                               5,
    //                                               cfg_->config_device_ini_[1],
    //                                               &parent_info_);
  //    ui_->FunctionWindow->setCurrentIndex(ui_->FunctionWindow->addWidget(tmp));

  //    ui_->FunctionWindow->setCurrentIndex(2);
  //    devices_info_[1].tab_widget->setCurrentTab(2);
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
    parent_info_.devices_info = &devices_info_;
    parent_info_.devices_windows_info = &devices_windows_info_;

    devices_windows_info_.emplace_back();  // 空占位

    devices_info_.emplace_back();                                     // Main窗口
    devices_info_[0].tab_index = 0;                                    // 在ui内默认创建,必定是0
    for (int device_num = 1; device_num <= device_count_; device_num++)  // 设备遍历初始化
    {
        // 创建Tab栏,初始化DevicesInfo内数据
        auto *new_tab = new QtMaterialTabs();
        struct DevicesInfo tmp
            {
                .windows_num = cfg_->GetMainCfg("/Device " + QString::number(device_num) + "/win").toInt(),
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
            int win_type = cfg_->GetDeviceCfg(device_num, "/Win" + QString::number(win_num) + "/type").toInt();
            if (win_type == 0) {
                continue;
            }
            devices_windows_info_[device_num].emplace_back();  // 0位置空占位
            devices_windows_info_[device_num].emplace_back();
            RepeaterWidget *tmp_widget = nullptr;
            switch (win_type) {
                case 1:devices_windows_info_[device_num][win_num].type = CHANNEL_CONFIGURATION;  // 结构体初始化
                    tmp_widget = new ChannelConfiguration(
                        device_num, cfg_->config_device_ini_[0], cfg_->config_device_ini_[device_num], &parent_info_);
                    devices_windows_info_[device_num][win_num].widget = tmp_widget;
                    connect(tmp_widget,
                            &RepeaterWidget::OrderExchangeWindow,
                            this,
                            &MainWindow::ReciveOrderExchangeWindow);//绑定切换窗口的有关事件
                    devices_windows_info_[device_num][win_num].index =
                        ui_->FunctionWindow->addWidget(devices_windows_info_[device_num][win_num].widget);
                    devices_info_[device_num].tab_widget->addTab("通道配置");  // 添加tab栏
                    break;
                case 50:devices_windows_info_[device_num][win_num].type = XCOM;  // 结构体初始化
                    devices_windows_info_[device_num][win_num].widget =
                        new ComTool(device_num, win_num, cfg_->config_device_ini_[device_num], &parent_info_);
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
                        new ChartsNext(device_num, win_num, cfg_->config_device_ini_[device_num], &parent_info_);
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
                    devices_windows_info_[device_num][win_num].widget =
                        new DataCirculation(device_num, win_num, cfg_->config_device_ini_[device_num], &parent_info_);
                    devices_windows_info_[device_num][win_num].index =
                        ui_->FunctionWindow->addWidget(devices_windows_info_[device_num][win_num].widget);
                    devices_info_[device_num].tab_widget->addTab("数据流过滤器配置");  // 添加tab栏
                    break;
                case 201:devices_windows_info_[device_num][win_num].type = TCP_BRIDGE_CONFIGURATION;  // 结构体初始化

                    tmp_widget =
                        new TCPBridgeConfiguration(device_num,
                                                   win_num,
                                                   cfg_->config_device_ini_[device_num],
                                                   &parent_info_);
                    devices_windows_info_[device_num][win_num].widget = tmp_widget;
                    connect(tmp_widget,
                            &RepeaterWidget::OrderExchangeWindow,
                            this,
                            &MainWindow::ReciveOrderExchangeWindow);//绑定切换窗口的有关事件
                    devices_windows_info_[device_num][win_num].index =
                        ui_->FunctionWindow->addWidget(devices_windows_info_[device_num][win_num].widget);
                    devices_info_[device_num].tab_widget->addTab("串口桥配置");  // 添加tab栏
                    break;
                case 202:devices_windows_info_[device_num][win_num].type = TCP_COM;  // 结构体初始化
                    devices_windows_info_[device_num][win_num].widget =
                        new TcpCom(device_num, win_num, cfg_->config_device_ini_[device_num], &parent_info_);
                    devices_windows_info_[device_num][win_num].index =
                        ui_->FunctionWindow->addWidget(devices_windows_info_[device_num][win_num].widget);
                    devices_info_[device_num].tab_widget->addTab("串口桥数据监视器");  // 添加tab栏
                    break;

                default:break;
            }
        }

        //        Charts test;  //图标界面测试
        //        test.show();
        // tab栏绑定
        connect(new_tab, &QtMaterialTabs::currentChanged, this, [=](int num) {
          // 这里的num从0开始，所以要+1
          DeviceWindowsExchange(device_num, num + 1);
        });

        connect(new_window_create_timer_, &QTimer::timeout, this, [=] {

        });
        connect(new_window_create_timer_, &QTimer::timeout, this, [=] {

        });
    }
}

/*!
 * 设备切换,被侧边栏切换
 * @param num 设备号
 */
void MainWindow::DeviceExchange(int device_num) {
  ui_->TabStackedWidget->setCurrentIndex(devices_info_[device_num].tab_index);
  DeviceWindowsExchange(device_num, 1);
}

/*!
 * 设备内Tab窗口切换
 * @param device_num 设备号
 * @param win_num 窗口号
 */
void MainWindow::DeviceWindowsExchange(int device_num, int win_num) {
    //    if(devices_windows_info_[Device_Num][win_num].index==0)
    //    {
    //        ErrorHandle("尝试打开不存在的窗口");
    //    }
    qDebug("try to switch %d device, %d windows", device_num, win_num);
    //    devices_info_[device_num_].tab_widget->setTabActive(win_num);
#if DEBUG
#else
    if (devices_info_[device_num].config_step < win_num) {
        //TODO:添加提醒用户完成上一步骤
        qDebug("Switch Failed");
        devices_info_[device_num].tab_widget->setCurrentTab(devices_info_[device_num].current_window - 1,
                                                            false);//把高亮回到该窗口（但受限于库底层，我无法解决它）
        return;
    }
#endif
    ui_->FunctionWindow->setCurrentIndex(devices_windows_info_[device_num][win_num].index);
    devices_info_[device_num].current_window = win_num;
}

void MainWindow::NewWindowCreate() {
}
void MainWindow::ReciveOrderExchangeWindow(int device, int windows_num) {
    DeviceWindowsExchange(device, windows_num);
}
