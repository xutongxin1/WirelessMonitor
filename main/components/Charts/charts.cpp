#include "charts.h"
#include "ui_charts.h"

Charts::Charts(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Charts)
{
    ui->setupUi(this);

    QWidget *paintArea = new QWidget;
    QCustomPlot *myPlot = new QCustomPlot(paintArea);
    myPlot->setFixedSize(480,300);
    //blue line
    myPlot->addGraph();
    myPlot->graph(0)->setPen(QPen(Qt::blue));
    //xAxis
    myPlot->axisRect()->setupFullAxesBox();
    myPlot->xAxis->setRange(1, 1, Qt::AlignRight);
    myPlot->yAxis->setRange(30, 30, Qt::AlignRight);
    myPlot->xAxis->setLabel("I(A)");
    myPlot->yAxis->setLabel("U(V)");

}

Charts::~Charts()
{
    delete ui;
}
