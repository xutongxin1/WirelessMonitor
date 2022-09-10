#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "qcustomplot.h"
#include "qtmaterialappbar.h"
#include "qtmaterialdrawer.h"
#include "SideBarButton/SideBarButton.h"
#include "ChannelConfiguration/channelconfiguration.h"
#include "qtmaterialscrollbar.h"
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

    DeviceNum=Cfg->DeviceNum;
    for (int i = 1; i <= DeviceNum; i++) {
        DeviceSelect[i] = new SideBarButton(i, Cfg);
        drawerLayout->addWidget(DeviceSelect[i]);
    }


    DeviceWindowsInit();


//    connect(DeviceSelect[0], SIGNAL(clicked()), this, SLOT());

    connect(ui->settingButton, SIGNAL(pressed()), m_drawer, SLOT(openDrawer()));
//    ui->FunctionTab->addTab()


}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::ErrorHandle(const QString& reason) {
    int ret = QMessageBox::warning(this, QStringLiteral("Error!"), reason, QMessageBox::Cancel | QMessageBox::Ok);
    std::exit(0);
}

void MainWindow::DeviceWindowsInit() {
    WindowsInfo.emplace_back();
    for(int i=1; i <= DeviceNum; i++){
        DevicesInfo.push_back({});
        struct DevicesInfo tmp{.windowsNum = Cfg->GetMainCfg("/Device "+QString::number(i)+"/win").toInt()};
        DevicesInfo.push_back(tmp);
        for(int j=1;j<=DevicesInfo[i].windowsNum;j++)
        {
            switch(Cfg->GetDeviceCfg(i,"/Win"+QString::number(i)+"/type").toInt())
            {
                case 1:
                    WindowsInfo.emplace_back();
                    WindowsInfo[i].emplace_back();
                    WindowsInfo[i].emplace_back();
                    WindowsInfo[i][j].type=ChannelConfiguration;
                    WindowsInfo[i][j].widget=new class ChannelConfiguration(i, Cfg);
                    WindowsInfo[i][j].index=ui->FunctionWindow->addWidget(WindowsInfo[i][j].widget);
            }
        }

    }


}
