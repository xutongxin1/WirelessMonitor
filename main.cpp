#include "mainwindow.h"

#include <QApplication>
#include "Charts/charts.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    Charts test;  //图标界面测试
    test.show();

    return a.exec();
}
