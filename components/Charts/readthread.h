#ifndef READTHREAD_H
#define READTHREAD_H

#include <QThread>

class ReadThread : public QThread
{
public:
    ReadThread();

    void run();
};

#endif // READTHREAD_H
