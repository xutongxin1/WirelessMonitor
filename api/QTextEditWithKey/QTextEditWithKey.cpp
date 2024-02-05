//
// Created by BLKDASH on 2023/11/3.
//
#include "QTextEditWithKey.h"

QTextEditWithKey::QTextEditWithKey(QWidget *parent)
        : QTextEdit{parent}
{

}

QTextEditWithKey::~QTextEditWithKey()
{

}

void QTextEditWithKey::keyReleaseEvent(QKeyEvent *event)
{
    if((event->key() >= Qt::Key_A && event->key() <= Qt::Key_Z) || (event->key() >= Qt::Key_0 && event->key() <= Qt::Key_9) || (event->key() == Qt::Key_Space) || (event->key() == Qt::Key_Enter) || (event->key() == Qt::Key_Backspace))
    {
        emit released(true);
    }

    QTextEdit::keyReleaseEvent(event);
}