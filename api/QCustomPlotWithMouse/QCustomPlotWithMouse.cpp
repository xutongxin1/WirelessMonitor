//
// Created by xtx on 2023/7/21.
//

#include "QCustomPlotWithMouse.h"
void QCustomPlotWithMouse::wheelEvent(QWheelEvent *event) {
    if(this->graph()) {
        this->graph()->setAntialiased(false);//关闭抗锯齿
    }
//    this->replot(QCustomPlot::rpQueuedReplot);

    emit(RwheelEvent());

    QCustomPlot::wheelEvent(event);

    timer->start(500);
}
void QCustomPlotWithMouse::mousePressEvent(QMouseEvent *event) {
    if(this->graph()){
        this->graph()->setAntialiased(false);//关闭抗锯齿
    }
//    this->replot(QCustomPlot::rpQueuedReplot);
    timer->stop();
//    emit(RmousePressEvent());
    QCustomPlot::mousePressEvent(event);
}

void QCustomPlotWithMouse::mouseReleaseEvent(QMouseEvent *event) {
    emit(RmouseReleaseEvent());
    QCustomPlot::mouseReleaseEvent(event);
    timer->start(500);
}
