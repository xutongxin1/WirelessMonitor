#include "charts.h"
#include "ui_charts.h"
#include <string.h>
#include <QDebug>

//打开通道不能移动和放缩，默认和关闭可以
/*颜色笔可选颜色，默认为红
        black,
        white,
        darkGray,
        gray,
        lightGray,
        red,
        green,
        blue,
        cyan,
        magenta,
        yellow,
        darkRed,
        darkGreen,
        darkBlue,
        darkCyan,
        darkMagenta,
        darkYellow,
        transparent
*/
//graph.setPen,setName。每个曲线都会独占一个graph

Charts::Charts(QWidget *parent) :
    RepeaterWidget(parent),
    uiChart(new Ui::Charts)
{
    uiChart->setupUi(this);

    timerChart = new QTimer(this);
    timerChart->setInterval(200);
    connect(timerChart,SIGNAL(timeout()),this,SLOT(ReadyShowLine()));

// Allow user to drag axis ranges with mouse, zoom with mouse wheel and select graphs by clicking:
    //chart配置
    uiChart->widget->xAxis->setLabel("Time/秒");
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
    /*如果你想图形更加精细，可以多定义些点
    QVector<double> Xvalue(100);
    QVector<double> Yvalue(100);
           Buff[flag]=CurrentData;//将新数据存入缓冲区
           for(int i=0;i<=flag;i++){
               Xvalue[i]=i*0.2;
               Yvalue[i]=Buff[i];
              }
     */

     //查找那些变量需要显示并且显示出来
     for(int i=0;i < ( DataPairs.size() );i++)
     {
         //记录每个变量的画图次数
        int tempCount = DataPairs.at(i).count;
        if( (DataPairs.at(i).flag) == 1 )
        {
            customPlot->graph(i)->setPen(QPen(Qt::red));
            customPlot->graph(i)->addData(timer_count, (DataPairs.at(i).DataBuff[tempCount]) );
            customPlot->graph(i)->setVisible(true);
            customPlot->graph(i)->rescaleAxes(true); //自动调成范围，只能放大。想要缩小把true去掉
            DataPairs[i].count++;
            qDebug()<<"red1"<<i<<endl;
        }
        else if( (DataPairs.at(i).flag) == 2 )
        {
            customPlot->graph(i)->addData(timer_count, (DataPairs.at(i).DataBuff[tempCount]) );
            customPlot->graph(i)->setVisible(false);
            DataPairs[i].count++;
            qDebug()<<"red2"<<i<<endl;
        }
     }
     customPlot->replot();//重绘图形

}

void Charts::ReadyShowLine()
{
    timer_count+=0.2;

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
        qDebug()<<"开始画图"<<endl;
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
    for(int i=0;i < ( DataPairs.size() );i++)
    {
        if( (DataPairs.at(i).name) == Data_Search)//存在返回true
        {

               DataPairs[i].flag = 1;
               uiChart->widget->graph(i)->setVisible(true);//显示

        }

    }
    uiChart->widget->replot();//重绘图形
}

/*****
 * Add是给外界的接口作用是增加可以绘图的变量，因此不用ui界面互动。
 * 如果识别到就先提前加图层准备画图
*****/
bool Charts::AddDate(QString addname, const QVector<double> &addDate)
{
    int size = addDate.size();
    if( ( DataPairs.size() ) == 0 )//如果是空链表
    {
        Datanode temp;
        temp.name = addname;
        temp.DataBuff = new double[size];
        temp.flag = 0;
        for(int i=0;i<addDate.size();i++)
        {
            temp.DataBuff[i]=addDate.at(i);
        }
        temp.num = 0;
        DataPairs.append(temp);//插入数据
        uiChart->comboBox->addItem(addname);//combox插入项
        uiChart->widget->addGraph();//加图层准备画图
        qDebug()<<"emptyadd"<<endl;
        return 1;
    }
    else{   //有数据
    for(int i=0;i < ( DataPairs.size() );i++)
    {
        if( (DataPairs.at(i).name) == addname)//存在返回true
        {
            //加入错误,返回0
            qDebug()<<"出错点1"<<endl;
            return 0;
        }
        else//如果识别到就先提前加图层准备画图
        {
            Datanode temp;
            for(int i=0;i<addDate.size();i++)
            {
                temp.DataBuff[i]=addDate[i];
            }
            temp.flag = 0;
            temp.num = i;
            temp.name = addname;
            DataPairs.append(temp);//插入数据
            uiChart->comboBox->addItem(addname);//combox插入项
            uiChart->widget->addGraph();//加图层准备画图
            return 1;
        }
    }}
    qDebug()<<(DataPairs.size())<<endl;
    qDebug()<<"出错点2"<<endl;
    return 0;
}

void Charts::on_pushButton_yincang_clicked()
{
    //点击鼠标然后隐藏
    for (int i = 0; i < uiChart->widget->graphCount(); ++i)
    {
        QCPGraph * graph = uiChart->widget->graph(i);
        if (graph ->selected()) {
            DataPairs[i].flag = 2;
            uiChart->widget->graph(i)->setVisible(false);//隐藏
        }
    }
    uiChart->widget->replot();//重绘图形
}

void Charts::test(const QVector<double> &addDate)
{
    double *temp;
    int size = addDate.size();
    temp = new double[size];

    for(int i=0;i<addDate.size();i++)
    {
        temp[i]=addDate.at(i);
    }
    qDebug()<<temp[3]<<endl;
}
