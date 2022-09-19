#ifndef CHARTS_H
#define CHARTS_H

#include <QWidget>
#include "qcustomplot.h"
#include "RepeaterWidget.h"

namespace Ui {
class Charts;
}

class Charts : public RepeaterWidget
{
    Q_OBJECT

public:
    explicit Charts(QWidget *parent = nullptr);
    ~Charts();

    double Buff[100];    //数据缓冲数组

    //实时数据，嵌入式系统中，可将下位机传上来的数据存于此变量中
    unsigned char CurrentData;

    void ShowLine(QCustomPlot *customPlot);//显示折线图
    void lChartsApi();//对外的Api接口

public slots:

    void ReadyShowLine();
    void myMoveEvent(QMouseEvent *event);
    //本例中用于修改实时数据，并调用ShowLine函数

private slots:

    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

private:
    Ui::Charts *ui;
    int flag;
    double timer_count=0.0;
    bool checked=0;
    QTimer *timerChart;
};

#endif // CHARTS_H
