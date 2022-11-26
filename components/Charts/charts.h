#ifndef CHARTS_H
#define CHARTS_H

#include <QWidget>
#include "qcustomplot.h"
#include "RepeaterWidget.h"

/*
typedef struct DataNode {
    QString name;
    double *DataBuff;
    int flag = 0;//判断是否画图不加数据,0——不画图，1——显示图，2——隐藏图但是会有数据
    int num;//在graph里对应的名次，需要实时更新(暂时荒废)
    long long size = 0;//记录数据大小
    long long count = 0;//记录每个数据已经画了多少个了。要注意防止溢出！！！
} Datanode;

QList<Datanode> DataPairs;  //QList方便与图例顺序对应.是负责后台更新维护显示数据的，因为图标显示需要double数组

QHash<QString,QVector<double>> Data_pools;是中间数据池，用容器去维护。

这是因为每当删除一个Graph，则Graph数量进行减一，即假设有两个Graph，分别为Graph0和Graph1，当删除Graph0时Graph1变为了Graph0。
*/


namespace Ui {
    class Charts;
}

class Charts : public RepeaterWidget {
Q_OBJECT

//friend bool AddDate(QString addname, const QVector<double> &addDate, Charts *Chart);
    friend class MainWindow;

    friend class ChartThread;

signals:

    void monitor(const QVector<double> &addDate);

public:
    explicit Charts(int DeviceNum, int winNum, QSettings *cfg, ToNewWidget *parentInfo, QWidget *parent = nullptr);

    ~Charts();

    double Buff[100];    //数据缓冲数组

    //实时数据，嵌入式系统中，可将下位机传上来的数据存于此变量中
    unsigned char CurrentData;

    void ShowLine(QCustomPlot *customPlot);//显示折线图




    //！！！公开函数！！！
    bool registerData(const QString& addname, const QVector<double> &addDate = QVector<double>());

    bool antiRegisterData(QString addName);

    bool checkRegister(QString addname);

    bool updateData(QString addname, double ChangeDate);

    void selectionChanged();

    void test(const QVector<double> &addDate);

public slots:

    void ReadyShowLine();

    void myMoveEvent(QMouseEvent *event);
    //本例中用于修改实时数据，并调用ShowLine函数

private slots:

    void on_pushButton_clicked();

    void on_pushButton_add_clicked();

    void on_pushButton_yincang_clicked();
//    void keep_monitor();

private:
    Ui::Charts *uiChart;
    QList<Datanode> DataPairs;  //QList方便与图例顺序对应
    int flag;
    double timer_count = 0.0;
    bool checked = 0;
    QTimer *timerChart;

};


#endif // CHARTS_H
