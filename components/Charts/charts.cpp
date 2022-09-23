#include "charts.h"
#include "ui_charts.h"
#include <string.h>
#include <QDebug>

//打开通道不能移动和放缩，默认和关闭可以

//graph.setPen,setName。每个曲线都会独占一个graph

Charts::Charts(QWidget *parent) :
    RepeaterWidget(parent),
    uiChart(new Ui::Charts)
{
    uiChart->setupUi(this);

    //先清空缓冲区
    memset(Buff,'\0',sizeof (Buff));
    CurrentData=0;
    flag=0;

    timerChart = new QTimer(this);
    timerChart->setInterval(200);
    connect(timerChart,SIGNAL(timeout()),this,SLOT(ReadyShowLine()));

// Allow user to drag axis ranges with mouse, zoom with mouse wheel and select graphs by clicking:
    lChartsApi();
    //chart配置
    uiChart->widget->xAxis->setLabel("Time");
    uiChart->widget->yAxis->setLabel("ADC");
    uiChart->widget->xAxis->setRange(0,100);
    uiChart->widget->yAxis->setRange(0,100);
    uiChart->widget->legend->setVisible(true);

    uiChart->widget->xAxis2->setVisible(true);
    uiChart->widget->xAxis2->setTickLabels(false);
    uiChart->widget->yAxis2->setVisible(true);
    uiChart->widget->yAxis2->setTickLabels(false);
    connect(uiChart->widget->xAxis, SIGNAL(rangeChanged(QCPRange)), uiChart->widget->xAxis2, SLOT(setRange(QCPRange)));
    connect(uiChart->widget->yAxis, SIGNAL(rangeChanged(QCPRange)), uiChart->widget->yAxis2, SLOT(setRange(QCPRange)));

    uiChart->widget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    connect(uiChart->widget,SIGNAL(mouseMove(QMouseEvent *)),this,SLOT(myMoveEvent(QMouseEvent *)));


}

Charts::~Charts()
{
    delete uiChart;
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

    ShowLine(uiChart->widget);

}


void Charts::myMoveEvent(QMouseEvent *event)
{
    //获取鼠标坐标，相对父窗体坐标
    int x_pos = event->pos().x();
    int y_pos = event->pos().y();
    qDebug()<<"event->pos()"<<event->pos();

    //鼠标坐标转化为CustomPlot内部坐标
    float x_val = uiChart->widget->xAxis->pixelToCoord(x_pos);
    float y_val = uiChart->widget->yAxis->pixelToCoord(y_pos);
    float line_y_val=0;
    //获得x轴坐标位置对应的曲线上y的值
    for (int i = 0; i < uiChart->widget->graphCount(); ++i)
    {
        QCPGraph * graph = uiChart->widget->graph(i);
        if (graph ->selected()) {
            line_y_val= uiChart->widget->graph(i)->data()->at(x_val)->value;
        }
    }

    //曲线的上点坐标位置，用来显示QToolTip提示框
    float out_x = uiChart->widget->xAxis->coordToPixel(x_val);
    float out_y = uiChart->widget->yAxis->coordToPixel(y_val);
//    float out_value = uiChart->widget->yAxis->coordToPixel(line_y_val);

    QString str,strToolTip;
    str = QString::number(x_val,10,3);
    strToolTip += "Time: ";
    strToolTip += str;
    strToolTip += "\n";

    str = QString::number(y_val,10,3);
    strToolTip += "ADC: ";
    strToolTip += str;
    strToolTip += "\n";

    QToolTip::showText(mapToGlobal(QPoint(out_x,out_y)),strToolTip,uiChart->widget);

}


void Charts::lChartsApi()
{
    //搜索有多少变量需要画图，然后加图层
    uiChart->widget->addGraph();
}

void Charts::on_pushButton_2_clicked()
{
    //点击鼠标然后删除
    for (int i = 0; i < uiChart->widget->graphCount(); ++i)
    {
        QCPGraph * graph = uiChart->widget->graph(i);
        if (graph ->selected()) {
            uiChart->widget->removeGraph(i);//销毁
        }
    }
    uiChart->widget->replot();//重绘图形
}

void Charts::on_pushButton_clicked()
{
    if(checked == 0)
    {
        //开启，不可以放缩和移动
        uiChart->pushButton->setText("关闭通道");
        timerChart->start();//每200ms重绘一次折线图
        uiChart->widget->setInteractions(QCP::iNone);
        checked = 1;
    }
    else
    {
        //关闭，可以放缩和移动
        uiChart->pushButton->setText("开启通道");
        timerChart->stop();
        uiChart->widget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
        checked = 0;
    }
}

void Charts::on_pushButton_add_clicked()
{
    //获取下拉栏目前的内容，搜索有没有对应的变量名
    QString Data_Search = uiChart->comboBox->currentText();
    if(DataMap.contains(Data_Search))   //存在返回true
    {
        if(DataMap.value(Data_Search).flag == 0)//如果没有画图，开始画图
        {
            uiChart->widget->addGraph();
        }
    }

}

//Add属于自动识别，因此不用ui界面互动
bool AddDate(QString addname, double *addDate, RepeaterWidget Chart)
{
    if(Chart.GetChartDataMap().contains(addname))   //存在返回true
    {
        //加入错误,返回0
        return 0;
    }
    else
    {
        Datanode temp;
        temp.DataBuff = addDate;
        temp.flag = 0;
        Chart.GetChartDataMap().insert(addname,temp);//插入数据
        Chart.GetChartUi()->comboBox->addItem(addname);//combox插入项
        return 1;
    }
}

void Charts::on_pushButton_yincang_clicked()
{
    //点击鼠标然后删除
    for (int i = 0; i < uiChart->widget->graphCount(); ++i)
    {
        QCPGraph * graph = uiChart->widget->graph(i);
        if (graph ->selected()) {
            uiChart->widget->removeGraph(i);//销毁
        }
    }
    uiChart->widget->replot();//重绘图形
}
