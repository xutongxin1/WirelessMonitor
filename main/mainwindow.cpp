#include "mainwindow.h"
#include "./ui/ui_mainwindow.h"
#include "qcustomplot.h"
#include "qtmaterialappbar.h"
#include "qtmaterialdrawer.h"
#include "statebutton.h"


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
    MainCfg = new CfgClass;
    m_drawer->setParent(ui->centralwidget);
    m_drawer->setClickOutsideToClose(true);
    m_drawer->setOverlayMode(true);
    m_drawer->setDrawerWidth(250);

    QVBoxLayout *drawerLayout = new QVBoxLayout;
    m_drawer->setDrawerLayout(drawerLayout);

    QString Note = MainCfg->GetMainCfg(QString("/Device/num"));
    if (Note != nullptr) {
        for (int i = 0; i < Note.toInt(); i++) {
            DeviceSelect[i] = new stateButton(i + 1, MainCfg);
            drawerLayout->addWidget(DeviceSelect[i]);
        }
    }


    connect(ui->settingButton, SIGNAL(pressed()), m_drawer, SLOT(openDrawer()));
//    ui->FunctionTab->addTab()


}

MainWindow::~MainWindow() {
    delete ui;
}
