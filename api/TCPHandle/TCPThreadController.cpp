//
// Created by xtx on 2022/9/29.
//

#include "TCPThreadController.h"
#include "TCPCommandHandle.h"

TCPThreadController::TCPThreadController(QObject *parent) : QObject(parent) {
    auto *handler = new TCPCommandHandle;
    handler->moveToThread(&WorkThread);


}
