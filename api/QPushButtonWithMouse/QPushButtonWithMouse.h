//
// Created by xtx on 2023/7/16.
//

#ifndef WIRELESSMONITOR_API_QPUSHBUTTONWITHMOUSE_QPUSHBUTTONWITHMOUSE_H_
#define WIRELESSMONITOR_API_QPUSHBUTTONWITHMOUSE_QPUSHBUTTONWITHMOUSE_H_

#include "qtmaterialraisedbutton.h"
#include <QMouseEvent>
class QPushButtonWithMouse : public QtMaterialRaisedButton {
 public:
    void mousePressEvent(QMouseEvent *event);
    explicit QPushButtonWithMouse(QWidget *parent);

 Q_OBJECT
 signals:

    void MidClicked();

};

#endif //WIRELESSMONITOR_API_QPUSHBUTTONWITHMOUSE_QPUSHBUTTONWITHMOUSE_H_
