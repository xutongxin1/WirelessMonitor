#include "mainwindow.h"

#include "./ui_mainwindow.h"
#include "Comtool.h"


int record_DeviceNum = 0, record_WinNum = 0;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui_(new Ui::MainWindow) {
    ui_->setupUi(this);
    // ui_->centralwidget->setStyleSheet(".RepeaterWidget{border-image: url(config/backgroud.png);}");
    auto tmp_widget = new ComTool(1, 1, new ConfigClass("main.ini", QSettings::IniFormat), nullptr);
    connect(tmp_widget,
            &RepeaterWidget::OrderShowSnackbar,
            this,
            &MainWindow::ReceiveOrderShowSnackbar); //绑定弹出提示的有关事件
    tmp_widget->setStyleSheet("#LRSplitter{border-image: url(config/backgroud.png);}");
    ui_->stackedWidget->setCurrentIndex(
        ui_->stackedWidget->addWidget(tmp_widget));
    snackbar_->setBackgroundColor(QColor(173,216,230,127));
    snackbar_->setAutoHideDuration(300);
    this->setWindowIcon(QIcon("./config/Icon.ico"));
}

MainWindow::~MainWindow() {
    delete ui_;
}

void MainWindow::ReceiveOrderShowSnackbar(const QString &message) {
    snackbar_->addMessage(message);
}
