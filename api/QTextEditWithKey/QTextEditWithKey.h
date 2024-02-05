//
// Created by BLKDASH on 2023/11/3.
//

#ifndef WIRELESSMONITOR_QTEXTEDITWITHKEY_H
#define WIRELESSMONITOR_QTEXTEDITWITHKEY_H

#include <QTextEdit>
#include <QKeyEvent>
//创建一个父类是QTextEdit的类
class QTextEditWithKey : public QTextEdit
{
Q_OBJECT
public:
    explicit QTextEditWithKey(QWidget *parent = nullptr);
    ~QTextEditWithKey();

protected:
    virtual void keyReleaseEvent(QKeyEvent *event);

signals:
    void released(bool a);
};

#endif //WIRELESSMONITOR_QTEXTEDITWITHKEY_H
