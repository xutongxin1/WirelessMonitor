#ifndef CHARTS_H
#define CHARTS_H

#include <QWidget>
#include "qcustomplot.h"
#include "RepeaterWidget.h"
#include <QObject>
#include <QThread>
#include <QMutex>


/*
enum DataType{ SYS_TIME , USER_TIME };
typedef struct DataNode {
    QString name;
    QList<double> *double_list;
    QList<QPair<double,double>> *data_list;
    int flag = 0;//判断是否画图不加数据,0——不画图，1——显示图，2——隐藏图但是会有数据
    DataType data_node_type;
    //long long d_size = 0;//记录数据存储大小
    //long long count = 0;//记录每个数据已经画了多少个了。要注意防止溢出！！！
} Datanode;

QList<Datanode> DataPairs;  //QList方便与图例顺序对应.是负责后台更新维护显示数据的，因为图标显示需要double数组

QHash<QString,Datanode> Data_pools;是中间数据池，用容器去维护。

这是因为每当删除一个Graph，则Graph数量进行减一，即假设有两个Graph，分别为Graph0和Graph1，当删除Graph0时Graph1变为了Graph0。
*/


namespace Ui {
    class Charts;
}


class DataReceiver : public QThread {
Q_OBJECT
public:
    //获取DataReceiver单例实例
    static DataReceiver *getInstance(void);

    explicit DataReceiver(QObject *parent = nullptr);

    //~DataReceiver();
    void stop();

protected:
    void run() override;

private:
    QMutex mutex;

signals:

    void oneDataReady();
};

/*
class Thread:public QThread
{
    Q_OBJECT
public:
    Thread();
    ~Thread();
    void stop();
protected:
    void run();
private:
    QMutex mutex;
    volatile bool stopped;
signals:
    void oneDataReady();
};
*/

class Charts : public RepeaterWidget {
Q_OBJECT

//friend bool AddDate(QString addname, const QVector<double> &addDate, Charts *Chart);
    friend class MainWindow;

    friend class ChartThread;

signals:

    void updataok();

    void monitor(const QVector<double> &addDate);

public:
    explicit Charts(int DeviceNum, int winNum, QSettings *cfg, ToNewWidget *parentInfo, QWidget *parent = nullptr);

    ~Charts();

    double Buff[100];    //数据缓冲数组

    //实时数据，嵌入式系统中，可将下位机传上来的数据存于此变量中
    unsigned char CurrentData;


    //！！！公开函数！！！
    bool registerData(const QString &addName, DataType datatype = USER_TIME);

    bool antiRegisterData(QString addName);

  [[maybe_unused]] bool checkRegister(QString addname);

    bool updateData(const QString &addName, double data);

    bool updateData(const QString &addName, double ChangeTime, double data);

    bool updateData(const QString &addName, QTime ChangeTime, double data);

//    bool updateData2(QString addname, double ChangeTime , double ChangeData);



    void test(const QVector<double> &addDate);

    QTime startedTime;

public slots:

    void ShowLine(QCustomPlot *customPlot);//显示折线图

    void ReadyShowLine();

    void myMoveEvent(QMouseEvent *event);
    //本例中用于修改实时数据，并调用ShowLine函数

 private slots:

    void on_pushButton_clicked();

    void on_pushButton_add_clicked();

    void on_pushButton_yincang_clicked();
//    void keep_monitor();

    void selectionChanged();

 private:
    Ui::Charts *uiChart;

    int flag;
    double timer_count = 0.0;
    bool checked = 1;
    QTimer *timerChart;

    bool timeHasInit = false;
    //DataReceiver *thread;
//    Thread *thread;
};


#endif // CHARTS_H
