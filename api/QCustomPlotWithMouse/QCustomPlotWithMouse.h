//
// Created by xtx on 2023/7/21.
//

#ifndef WIRELESSMONITOR_API_QCUSTOMPLOTWITHMOUSE_QCUSTOMPLOTWITHMOUSE_H_
#define WIRELESSMONITOR_API_QCUSTOMPLOTWITHMOUSE_QCUSTOMPLOTWITHMOUSE_H_

#include "qcustomplot.h"
class QCustomPlotWithMouse : public QCustomPlot {
 public:
    QCustomPlotWithMouse(QWidget *parent = nullptr) : QCustomPlot(parent) {
        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, [&] {
          timer->stop();
          this->graph()->setAntialiased(true);
          this->replot(QCustomPlot::rpQueuedReplot);
        });
    };
 protected:
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    QTimer *timer;
};

#endif //WIRELESSMONITOR_API_QCUSTOMPLOTWITHMOUSE_QCUSTOMPLOTWITHMOUSE_H_
