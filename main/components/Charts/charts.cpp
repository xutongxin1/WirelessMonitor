#include "charts.h"
#include "ui_charts.h"
#include <string.h>

Charts::Charts(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Charts)
{
    ui->setupUi(this);

    //先清空缓冲区
    memset(Buff,'\0',sizeof (Buff));
    CurrentData=0;
    flag=0;
    QTimer *timer = new QTimer(this);
    timer->start(200);//每200ms重绘一次折线图
    connect(timer,SIGNAL(timeout()),this,SLOT(ReadyShowLine()));


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

       if(flag<=99){  //当图形中不足100个点时，进入到if句里处理

           Buff[flag]=CurrentData;//将新数据存入缓冲区

           for(int i=0;i<=flag;i++){

               Xvalue[i]=i;

               Yvalue[i]=Buff[i];

              }


           //分别赋值X轴，Y轴值，产生多少个实时值，就赋值多少个点

               flag++;

               customPlot->addGraph();

               customPlot->graph(0)->setPen(QPen(Qt::red));

               customPlot->graph(0)->setData(Xvalue,Yvalue);

               customPlot->xAxis->setLabel("Time");

               customPlot->yAxis->setLabel("ADC");

               customPlot->xAxis->setRange(0,100);

               customPlot->yAxis->setRange(0,100);

               customPlot->replot();//重绘图形

               return;

           }

      //当实时数据超过100个时，进行以下处理

       for(int i=0;i<99;i++)

       {

           Buff[i]=Buff[i+1];

       }

       Buff[99]=CurrentData;

       //缓冲区整体左移，Buff[0]丢弃，Buff[99]接收新数据

       for(int i=0;i<100;i++)

       {

           Xvalue[i] = flag-(99-i);

           Yvalue[i] =Buff[i];

       }//X,Y轴赋满100个值，其中X轴要跟着增加

       customPlot->addGraph();

       customPlot->graph(0)->setPen(QPen(Qt::red));

       customPlot->graph(0)->setData(Xvalue,Yvalue);



       customPlot->xAxis->setLabel("Time");

       customPlot->yAxis->setLabel("ADC");



       customPlot->xAxis->setRange(0+flag-99,100+flag-99);

       //X坐标轴跟着平移

       customPlot->yAxis->setRange(0,100);

       customPlot->replot();//重绘图形

       flag++;

}

void Charts::ReadyShowLine()

{

    CurrentData=CurrentData+5;

    if(CurrentData>=80) CurrentData=0;//产生锯齿波，最大值是75

    ShowLine(ui->widget);

}
