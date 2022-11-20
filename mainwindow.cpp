#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "qcustomplot.h"
#include "qtmaterialappbar.h"
#include "qtmaterialdrawer.h"
#include "SideBarButton/SideBarButton.h"
#include "ChannelConfiguration/channelconfiguration.h"
#include "qtmaterialscrollbar.h"
#include "ComTool/comtool.h"
#include <cstdlib>
#include "TCPBridgeConfiguration/tcpbridgeconfiguration.h"
#include "TCPCom/tcpcom.h"
#include "DataCirculation/datacirculation.h"


int record_DeviceNum = 0, record_WinNum = 0;

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
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
    m_drawer = new QtMaterialDrawer;
    Cfg = new CfgClass;
    m_drawer->setParent(ui->centralwidget);
    m_drawer->setClickOutsideToClose(true);
    m_drawer->setOverlayMode(true);
    m_drawer->setDrawerWidth(250);

    auto *drawerLayout = new QVBoxLayout;
    m_drawer->setDrawerLayout(drawerLayout);

    DeviceSelect[0] = new SideBarButton();
    drawerLayout->addWidget(DeviceSelect[0]);//初始化数据聚合窗口

    DeviceCount = Cfg->DeviceNum;
    for (int i = 1; i <= DeviceCount; i++) {
        DeviceSelect[i] = new SideBarButton(i, Cfg);
        drawerLayout->addWidget(DeviceSelect[i]);
        connect(DeviceSelect[i]->Button, &QPushButton::pressed, this, [=] {
            MainWindow::DeviceExchange(i);
        });
    }
    connect(ui->settingButton, SIGNAL(pressed()), m_drawer, SLOT(openDrawer()));
    DeviceExchange(1);
//    DeviceWindowsExchange(1, 1);


    DeviceWindowsInit();
    //lulu_test
    QVector<double> a;
    for (int i = 0; i < 100; i++) {
        //a[i]=i*30.0;
        a.append(i * 30.0);
    }
    qDebug() << a[50] << endl;
    QVector<double> a1;
    for (int i = 0; i < 100; i++) {
        //a[i]=i*30.0;
        a1.append(i * 40.0);
    }
    qDebug() << a1[50] << endl;
    test1->registerData("test", a);
    test1->registerData("test1", a1);

    
    DataCirculation *tmp = new class DataCirculation();
    ui->FunctionWindow->setCurrentIndex(ui->FunctionWindow->addWidget(tmp));



//    ui->FunctionWindow->setCurrentIndex(2);
//    DevicesInfo[1].TabWidget->setCurrentTab(2);
}

MainWindow::~MainWindow() {
    delete ui;
}

/*!
 * 错误处理函数,会弹出窗口并结束程序
 * @param reason 出错提示
 */
void MainWindow::ErrorHandle(const QString &reason) {
    int ret = QMessageBox::warning(this, QStringLiteral("Error!"), reason, QMessageBox::Cancel | QMessageBox::Ok);
    std::exit(0);
}

/*!
 * 窗口结构体初始化
 */
void MainWindow::DeviceWindowsInit() {
    parentInfo.DevicesInfo = &DevicesInfo;
    parentInfo.DevicesWindowsInfo = &DevicesWindowsInfo;

    DevicesWindowsInfo.emplace_back();//空占位

    DevicesInfo.emplace_back();//Main窗口
    DevicesInfo[0].TabIndex = 0;//在ui内默认创建,必定是0
    for (int DeviceNum = 1; DeviceNum <= DeviceCount; DeviceNum++) //设备遍历初始化
    {
        //创建Tab栏,初始化DevicesInfo内数据
        auto *NewTab = new QtMaterialTabs();
        struct DevicesInfo tmp{.windowsNum = Cfg->GetMainCfg(
                "/Device " + QString::number(DeviceNum) + "/win").toInt(), .TabIndex =ui->TabStackedWidget->addWidget(
                NewTab), .TabWidget=NewTab};
        DevicesInfo.push_back(tmp);
        DevicesWindowsInfo.emplace_back();//创建行

        //创建socket对象
        DevicesInfo[DeviceNum].TCPInfoHandler[1] = new TCPInfoHandle;
        DevicesInfo[DeviceNum].TCPInfoHandler[2] = new TCPInfoHandle;
        DevicesInfo[DeviceNum].TCPInfoHandler[3] = new TCPInfoHandle;
        DevicesInfo[DeviceNum].TCPCommandHandler = new TCPCommandHandle;//都创建一个socket对象吧，防止空指针
        for (int WinNum = 1; WinNum <= DevicesInfo[DeviceNum].windowsNum; WinNum++)//窗口遍历初始化
        {
            int WinType = Cfg->GetDeviceCfg(DeviceNum, "/Win" + QString::number(WinNum) + "/type").toInt();
            if (WinType == 0)continue;
            DevicesWindowsInfo[DeviceNum].emplace_back();//0位置空占位
            DevicesWindowsInfo[DeviceNum].emplace_back();
            switch (WinType) {
                case 1:
                    DevicesWindowsInfo[DeviceNum][WinNum].type = Channel_Configuration;//结构体初始化
                    DevicesWindowsInfo[DeviceNum][WinNum].widget = new ChannelConfiguration(DeviceNum,
                                                                                            Cfg->configDeviceIni[0],
                                                                                            Cfg->configDeviceIni[DeviceNum],
                                                                                            &parentInfo);
                    DevicesWindowsInfo[DeviceNum][WinNum].index = ui->FunctionWindow->addWidget(
                            DevicesWindowsInfo[DeviceNum][WinNum].widget);
                    DevicesInfo[DeviceNum].TabWidget->addTab("通道配置");//添加tab栏
                    break;
                case 50:
                    DevicesWindowsInfo[DeviceNum][WinNum].type = XCOM;//结构体初始化
                    DevicesWindowsInfo[DeviceNum][WinNum].widget = new ComTool(DeviceNum, WinNum,
                                                                               Cfg->configDeviceIni[DeviceNum],
                                                                               &parentInfo);
                    DevicesWindowsInfo[DeviceNum][WinNum].index = ui->FunctionWindow->addWidget(
                            DevicesWindowsInfo[DeviceNum][WinNum].widget);
                    DevicesInfo[DeviceNum].TabWidget->addTab("本地串口监视器");//添加tab栏
                    break;
                case 51:
                    //记录下相应的变量，方便提取类的成员变量charts
                    record_DeviceNum = DeviceNum;
                    record_WinNum = WinNum;

                    DevicesWindowsInfo[DeviceNum][WinNum].type = MainChart;//结构体初始化
                    DevicesWindowsInfo[DeviceNum][WinNum].widget = test1;
                    DevicesWindowsInfo[DeviceNum][WinNum].index = ui->FunctionWindow->addWidget(
                            DevicesWindowsInfo[DeviceNum][WinNum].widget);
                    DevicesInfo[DeviceNum].TabWidget->addTab("数据波形图");//添加tab栏
                    break;
                case 201:
                    DevicesWindowsInfo[DeviceNum][WinNum].type = TCP_Bridge_Configuration;//结构体初始化
                    DevicesWindowsInfo[DeviceNum][WinNum].widget =
                            new TCPBridgeConfiguration(
                                    DeviceNum,
                                    WinNum,
                                    Cfg->configDeviceIni[DeviceNum],
                                    &parentInfo
                            );
                    DevicesWindowsInfo[DeviceNum][WinNum].index = ui->FunctionWindow->addWidget(
                            DevicesWindowsInfo[DeviceNum][WinNum].widget);
                    DevicesInfo[DeviceNum].TabWidget->addTab("串口桥配置");//添加tab栏
                    break;
                case 202:
                    DevicesWindowsInfo[DeviceNum][WinNum].type = TCP_Com;//结构体初始化
                    DevicesWindowsInfo[DeviceNum][WinNum].widget = new TCPCom(DeviceNum, WinNum,
                                                                              Cfg->configDeviceIni[DeviceNum],
                                                                              &parentInfo);
                    DevicesWindowsInfo[DeviceNum][WinNum].index = ui->FunctionWindow->addWidget(
                            DevicesWindowsInfo[DeviceNum][WinNum].widget);
                    DevicesInfo[DeviceNum].TabWidget->addTab("串口桥数据监视器");//添加tab栏
                    break;


                default:
                    break;
            }
        }

        Charts test;  //图标界面测试
        test.show();
        //tab栏绑定
        connect(NewTab, &QtMaterialTabs::currentChanged, this, [=](int num) {
            //这里的num从0开始，所以要+1
            DeviceWindowsExchange(DeviceNum, num + 1);
        });

        connect(NewWindowCreateTimer, &QTimer::timeout, this, [=] {


        });


    }


}

/*!
 * 设备切换,被侧边栏切换
 * @param num 设备号
 */
void MainWindow::DeviceExchange(int DeviceNum) {
    ui->TabStackedWidget->setCurrentIndex(DevicesInfo[DeviceNum].TabIndex);
    DeviceWindowsExchange(DeviceNum, 1);
}

/*!
 * 设备内Tab窗口切换
 * @param DeviceNum 设备号
 * @param WinNum 窗口号
 */
void MainWindow::DeviceWindowsExchange(int DeviceNum, int WinNum) {
//    if(DevicesWindowsInfo[Device_Num][WinNum].index==0)
//    {
//        ErrorHandle("尝试打开不存在的窗口");
//    }
    qDebug("try to switch %d device, %d windows", DeviceNum, WinNum);
//    DevicesInfo[DeviceNum].TabWidget->setTabActive(WinNum);
    ui->FunctionWindow->setCurrentIndex(DevicesWindowsInfo[DeviceNum][WinNum].index);
}

void MainWindow::NewWindowCreate() {
}


