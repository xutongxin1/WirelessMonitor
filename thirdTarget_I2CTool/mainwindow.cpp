#include "mainwindow.h"

#include <FluLabel.h>

#include "./ui_mainwindow.h"
#include "ConfigClass.h"
#include "IICDeviceEach.h"
#include "VersionConfig.h"
#include "FluPushButton.h"
#include "ui_IICDeviceEach.h"

int record_DeviceNum = 0, record_WinNum = 0;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui_(new Ui::MainWindow) {
    ui_->setupUi(this);
    // ui_->centralwidget->setStyleSheet(".RepeaterWidget{border-image: url(config/backgroud.png);}");
    auto tmp_widget = new IICDeviceEach(nullptr);
    connect(tmp_widget,
            &RepeaterWidget::OrderShowSnackbar,
            this,
            &MainWindow::ReceiveOrderShowSnackbar); //绑定弹出提示的有关事件
    tmp_widget->setStyleSheet("#LRSplitter{border-image: url(config/backgroud.png);}");
    ui_->stackedWidget->setCurrentIndex(
        ui_->stackedWidget->addWidget(tmp_widget));
    snackbar_->setBackgroundColor(QColor(173,216,230,127));
    snackbar_->setAutoHideDuration(300);
    snackbar_->setTextColor(QColor(0,0,0));
    this->setWindowIcon(QIcon("./config/Icon.ico"));
    this->setWindowTitle("ComTool串口助手"+QString(V_BUILD_TIME)+"_"+QString(V_GIT_INFO));
    // auto tmp=new FluDropDownButton();
    // tmp_widget->ui_->label_5->setLabelStyle(FluLabelStyle::BodyStrongTextBlockStyle);
    // auto repoLabel = new FluLabel(this);
    // repoLabel->setLabelStyle(FluLabelStyle::BodyStrongTextBlockStyle);
    // repoLabel->setText("To clone thepository");
}

MainWindow::~MainWindow() {
    delete ui_;
}

void MainWindow::ReceiveOrderShowSnackbar(const QString &message) {
    snackbar_->addMessage(message);
}
