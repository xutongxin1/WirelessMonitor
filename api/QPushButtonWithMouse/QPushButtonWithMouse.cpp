//
// Created by xtx on 2023/7/16.
//

#include "QPushButtonWithMouse.h"
void QPushButtonWithMouse::mousePressEvent(QMouseEvent *event) {
    QtMaterialFlatButton::mousePressEvent(event);
    if (event->button() == Qt::MiddleButton) {
        emit MidClicked();
    }
}
QPushButtonWithMouse::QPushButtonWithMouse(QWidget *parent) : QtMaterialRaisedButton(parent) {

}
