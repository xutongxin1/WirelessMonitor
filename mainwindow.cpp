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

    DeviceNum = Cfg->DeviceNum;
    for (int i = 1; i <= DeviceNum; i++) {
        DeviceSelect[i] = new SideBarButton(i, Cfg);
        drawerLayout->addWidget(DeviceSelect[i]);
        connect(DeviceSelect[i]->Button, &QPushButton::pressed, this, [=]
        {
            MainWindow::DeviceExchange(i);
        });
    }


    DeviceWindowsInit();

//    ComTool *tmp=new ComTool();

//    connect(DeviceSelect[0], SIGNAL(clicked()), this, SLOT());

    connect(ui->settingButton, SIGNAL(pressed()), m_drawer, SLOT(openDrawer()));
    DeviceExchange(1);
//    ui->FunctionWindow->setCurrentIndex(ui->FunctionWindow->addWidget(tmp));
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
    DevicesWindowsInfo.emplace_back();//空占位

    DevicesInfo.emplace_back();//Main窗口
    DevicesInfo[0].TabIndex = 0;//在ui内默认创建,必定是0
    for (int i = 1; i <= DeviceNum; i++) {
        //创建Tab栏,初始化DevicesInfo内数据
        auto *NewTab = new QtMaterialTabs();
        struct DevicesInfo tmp{.windowsNum = Cfg->GetMainCfg(
                "/Device " + QString::number(i) + "/win").toInt(), .TabIndex =ui->TabStackedWidget->addWidget(
                NewTab), .TabWidget=NewTab};
        DevicesInfo.push_back(tmp);
        DevicesWindowsInfo.emplace_back();//创建行
        for (int j = 1; j <= DevicesInfo[i].windowsNum; j++) {
            int WinType=Cfg->GetDeviceCfg(i, "/Win" + QString::number(j) + "/type").toInt();
            if(WinType==0)continue;
            DevicesWindowsInfo[i].emplace_back();//0位置空占位
            DevicesWindowsInfo[i].emplace_back();
            switch (WinType) {
                case 1:
                    DevicesWindowsInfo[i][j].type = ChannelConfiguration;//结构体初始化
                    DevicesWindowsInfo[i][j].widget = new class ChannelConfiguration(i, Cfg);
                    DevicesWindowsInfo[i][j].index = ui->FunctionWindow->addWidget(DevicesWindowsInfo[i][j].widget);
                    DevicesInfo[i].TabWidget->addTab("通道配置");//添加tab栏
                    break;
                case 2:
                    DevicesWindowsInfo[i][j].type = XCOM;//结构体初始化
                    DevicesWindowsInfo[i][j].widget =new ComTool();
                    DevicesWindowsInfo[i][j].index = ui->FunctionWindow->addWidget(DevicesWindowsInfo[i][j].widget);
                    DevicesInfo[i].TabWidget->addTab("本地串口监视器");//添加tab栏
                    break;

            }
        }

        //tab栏绑定
        connect(NewTab,&QtMaterialTabs::currentChanged,this,[=](int num)
        {
            //这里的num从0开始，所以要+1
            DeviceWindowsExchange(i,num+1);
        });

    }


}

/*!
 * 设备切换,被侧边栏切换
 * @param num 设备号
 */
void MainWindow::DeviceExchange(int Device_Num) {
    ui->TabStackedWidget->setCurrentIndex(DevicesInfo[Device_Num].TabIndex);
    DeviceWindowsExchange(DeviceNum,1);
}

/*!
 * 设备内Tab窗口切换
 * @param DeviceNum 设备号
 * @param WinNum 窗口号
 */
void MainWindow::DeviceWindowsExchange(int Device_Num, int WinNum) {
//    if(DevicesWindowsInfo[Device_Num][WinNum].index==0)
//    {
//        ErrorHandle("尝试打开不存在的窗口");
//    }
    qDebug("尝试切换到%d设备%d窗口",Device_Num,WinNum);
    ui->FunctionWindow->setCurrentIndex(DevicesWindowsInfo[Device_Num][WinNum].index);
}