//
// Created by xtx on 2022/9/29.
//

#ifndef MAIN_API_TCPHANDLE_TCPTHREADCONTROLLER_H_
#define MAIN_API_TCPHANDLE_TCPTHREADCONTROLLER_H_

#include <QObject>
#include <QThread>

class TCPThreadController : public QObject {
 public:
    TCPThreadController(QObject *parent);
    bool is_connected_;
    bool is_sending_package_;
    bool is_finish_last_work_;
    QThread work_thread_;
};

#endif //MAIN_API_TCPHANDLE_TCPTHREADCONTROLLER_H_
