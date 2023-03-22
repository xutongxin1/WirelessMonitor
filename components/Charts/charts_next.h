#ifndef ChartsNext_H
#define ChartsNext_H

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
class charts_next;
}

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

class ChartsNext : public RepeaterWidget {
 Q_OBJECT

    //friend bool AddDate(QString addname, const QVector<double> &addDate, ChartsNext *Chart);
    friend class MainWindow;

    friend class ChartThread;

 signals:

    void updataok();

    void monitor(const QVector<double> &addDate);

 public:
    explicit ChartsNext(int device_num,
                        int win_num,
                        QSettings *cfg,
                        ToNewWidget *parent_info,
                        QWidget *parent = nullptr);

    ~ChartsNext();

    double Buff[100];    //数据缓冲数组


    //！！！公开函数！！！
    bool RegisterDataPoint(const QString &point_name);

    bool AntiRegisterDataPoint(const QString &point_name);
    bool AntiRegisterAllDataPoint();

    [[maybe_unused]] bool IsDataPointRegistter(const QString &addname);

    bool AddDataAuto(const QString &point_name, double data);

    bool AddDataWithProgramTime(const QString &point_name, double data, double program_time);
    bool AddDataWithProgramTime(const QString &point_name,
                                double data,
                                const QDateTime &time);

    bool AddDataWithDateTime(const QString &point_name, double data, QDateTime *date_time);

//    bool updateData2(QString addname, double ChangeTime , double ChangeData);



    void test(const QVector<double> &addDate);

    void UpdateDataPoolIndex();

    void SetProgramTime();

    void LoadInfo(QString name_first,QString name_second);
    QTimer *paint_timer_;

 public slots:

    void UpdateLine();

    void myMoveEvent(QMouseEvent *event);
    //本例中用于修改实时数据，并调用ShowLine函数

 private slots:

    void on_pushButton_clicked();

    void on_pushButton_yincang_clicked();
//    void keep_monitor();

    void selectionChanged();

 private:
    Ui::charts_next *ui_chart_;

    QCustomPlot *custom_plot_;

    QList<DataNode> data_pool_;//数据池

    QHash<QString, DataNodeIndex> data_pool_index_;//指针索引，加快添加数据时的速度

    TimeType chart_time_type_ = PROGRAM_TIME;

    int flag;
    double timer_count = 0.0;
    bool checked = 1;

    bool timeHasInit = false;

    long double program_begin_time_;
    //DataReceiverNext *thread;
//    Thread *thread;



};

#endif // ChartsNext_H
