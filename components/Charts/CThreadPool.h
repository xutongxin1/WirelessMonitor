/*
#ifndef CTHREADPOOL_H
#define CTHREADPOOL_H

#include <QObject>
#include <QThread>
#include <QMutex>


class DataReceiver : public QThread
{
    Q_OBJECT
public:
    //获取DataReceiver单例实例
    static DataReceiver *getInstance(void);

    explicit DataReceiver(QObject *parent = nullptr);

    void stop();

protected:
    void run() override;

private:
    QMutex mutex;

signals:
    void oneDataReady();
};

#endif // DATARECEIVER_H
*/
