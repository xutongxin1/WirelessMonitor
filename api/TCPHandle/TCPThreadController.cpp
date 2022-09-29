//
// Created by xtx on 2022/9/29.
//

#include "TCPThreadController.h"
#include "TCPHandle.h"

TCPThreadController::TCPThreadController(QObject *parent) : QObject(parent) {
    auto *handler = new TCPHandle;
    handler->moveToThread(&WorkThread);


}
