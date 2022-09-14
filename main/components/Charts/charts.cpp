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
    //timer->start(200);//每200ms重绘一次折线图
    connect(timer,SIGNAL(timeout()),this,SLOT(ReadyShowLine()));

    ui->widget->resize(600, 600);

    // add two new graphs and set their look:
    ui->widget->addGraph();
    ui->widget->graph(0)->setPen(QPen(Qt::blue)); // line color blue for first graph
    ui->widget->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20))); // first graph will be filled with translucent blue
    ui->widget->addGraph();
    ui->widget->graph(1)->setPen(QPen(Qt::red)); // line color red for second graph
// generate some points of data (y0 for first, y1 for second graph):
    QVector<double> x(251), y0(251), y1(251);
    for (int i=0; i<251; ++i)
    {
        x[i] = i;
        y0[i] = qExp(-i/150.0)*qCos(i/10.0); // exponentially decaying cosine
        y1[i] = qExp(-i/150.0);              // exponential envelope
    }
// configure right and top axis to show ticks but no labels:
// (see QCPAxisRect::setupFullAxesBox for a quicker method to do this)
    ui->widget->xAxis2->setVisible(true);
    ui->widget->xAxis2->setTickLabels(false);
    ui->widget->yAxis2->setVisible(true);
    ui->widget->yAxis2->setTickLabels(false);
// make left and bottom axes always transfer their ranges to right and top axes:
    connect(ui->widget->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->widget->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->widget->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->widget->yAxis2, SLOT(setRange(QCPRange)));
// pass data points to graphs:
    ui->widget->graph(0)->setData(x, y0);
    ui->widget->graph(1)->setData(x, y1);
// let the ranges scale themselves so graph 0 fits perfectly in the visible area:
    ui->widget->graph(0)->rescaleAxes();
// same thing for graph 1, but only enlarge ranges (in case graph 1 is smaller than graph 0):
    ui->widget->graph(1)->rescaleAxes(true);
// Note: we could have also just called customPlot->rescaleAxes(); instead
// Allow user to drag axis ranges with mouse, zoom with mouse wheel and select graphs by clicking:
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
    float out_y = ui->widget->yAxis->coordToPixel(line_y_val);

    QString str,strToolTip;
    str = QString::number(x_val,10,3);
    strToolTip += "Time: ";
    strToolTip += str;
    strToolTip += "\n";

    str = QString::number(line_y_val,10,3);
    strToolTip += "ADC: ";
    strToolTip += str;
    strToolTip += "\n";

    QToolTip::showText(mapToGlobal(QPoint(out_x,out_y)),strToolTip,ui->widget);

}
