#include "charts.h"
#include "ui_charts.h"
#include <string.h>

//打开通道不能移动和放缩，默认和关闭可以
QTimer *timer;
double timer_count=0.0;

Charts::Charts(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Charts)
{
    ui->setupUi(this);

    //先清空缓冲区
    memset(Buff,'\0',sizeof (Buff));
    CurrentData=0;
    flag=0;

    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(ReadyShowLine()));

// Allow user to drag axis ranges with mouse, zoom with mouse wheel and select graphs by clicking:
    lChartsApi();
    //chart配置
    ui->widget->xAxis->setLabel("Time");
    ui->widget->yAxis->setLabel("ADC");
    ui->widget->xAxis->setRange(0,100);
    ui->widget->yAxis->setRange(0,100);

    ui->widget->xAxis2->setVisible(true);
    ui->widget->xAxis2->setTickLabels(false);
    ui->widget->yAxis2->setVisible(true);
    ui->widget->yAxis2->setTickLabels(false);
    connect(ui->widget->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->widget->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->widget->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->widget->yAxis2, SLOT(setRange(QCPRange)));

    ui->widget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    connect(ui->widget,SIGNAL(mouseMove(QMouseEvent *)),this,SLOT(myMoveEvent(QMouseEvent *)));


}

Charts::~Charts()
{
    delete ui;
}

void Charts::ShowLine(QCustomPlot *customPlot)

{

    QVector<double> Xvalue(100);
    QVector<double> Yvalue(100);


    //无论如何，折线图一次能展示的区域总是有限的,这里一次最多绘制100个点

    //如果你想图形更加精细，可以多定义些点

           Buff[flag]=CurrentData;//将新数据存入缓冲区

           for(int i=0;i<=flag;i++){

               Xvalue[i]=i*0.2;

               Yvalue[i]=Buff[i];

              }



           //分别赋值X轴，Y轴值，产生多少个实时值，就赋值多少个点。XY坐标轴跟着平移

               flag++;
               customPlot->graph(0)->setPen(QPen(Qt::red));
               customPlot->graph(0)->setData(Xvalue,Yvalue);
               customPlot->graph(0)->rescaleAxes(true); //自动调成范围，只能放大。想要缩小把true去掉



       customPlot->replot();//重绘图形

}

void Charts::ReadyShowLine()

{

    timer_count+=0.2;

    CurrentData=CurrentData+1;

    if(CurrentData>=80) CurrentData=0;//产生锯齿波，最大值是75

    ShowLine(ui->widget);

}


void Charts::myMoveEvent(QMouseEvent *event)
{
    //获取鼠标坐标，相对父窗体坐标
    int x_pos = event->pos().x();
    int y_pos = event->pos().y();
    qDebug()<<"event->pos()"<<event->pos();

    //鼠标坐标转化为CustomPlot内部坐标
    float x_val = ui->widget->xAxis->pixelToCoord(x_pos);
    float y_val = ui->widget->yAxis->pixelToCoord(y_pos);
    float line_y_val=0;
    //获得x轴坐标位置对应的曲线上y的值
    for (int i = 0; i < ui->widget->graphCount(); ++i)
    {
        QCPGraph * graph = ui->widget->graph(i);
        if (graph ->selected()) {
            line_y_val= ui->widget->graph(i)->data()->at(x_val)->value;
        }
    }

    //曲线的上点坐标位置，用来显示QToolTip提示框
    float out_x = ui->widget->xAxis->coordToPixel(x_val);
    float out_y = ui->widget->yAxis->coordToPixel(y_val);
//    float out_value = ui->widget->yAxis->coordToPixel(line_y_val);

    QString str,strToolTip;
    str = QString::number(x_val,10,3);
    strToolTip += "Time: ";
    strToolTip += str;
    strToolTip += "\n";

    str = QString::number(y_val,10,3);
    strToolTip += "ADC: ";
    strToolTip += str;
    strToolTip += "\n";

    QToolTip::showText(mapToGlobal(QPoint(out_x,out_y)),strToolTip,ui->widget);

}

void Charts::on_pushButton_clicked(bool checked)
{
    if(checked == 1)
    {
        //开启，不可以放缩和移动
        ui->pushButton->setText("关闭通道");
        timer->start(200);//每200ms重绘一次折线图
        ui->widget->setInteractions(QCP::iNone);
    }
    else
    {
        //关闭，可以放缩和移动
        ui->pushButton->setText("开启通道");
        timer->stop();
        ui->widget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    }
}

void Charts::lChartsApi()
{
    //搜索有多少变量需要画图，然后加图层
    ui->widget->addGraph();
}

void Charts::on_pushButton_2_clicked()
{
    //点击鼠标然后删除
    for (int i = 0; i < ui->widget->graphCount(); ++i)
    {
        QCPGraph * graph = ui->widget->graph(i);
        if (graph ->selected()) {
            ui->widget->removeGraph(i);//销毁
        }
    }
    ui->widget->replot();//重绘图形
}
