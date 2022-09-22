//
// Created by xtx on 2022/9/15.
//

#ifndef QT_REPEATERWIDGET_H
#define QT_REPEATERWIDGET_H


#include <QFrame>
#include <QtWidgets/QPushButton>
#include <QSettings>

typedef std::map<std::string, QPushButton> QPushButtonMap;

class RepeaterWidget : public QWidget {
public:
//    QPushButtonMap Button;
    explicit RepeaterWidget(QWidget *parent = nullptr);
    ~RepeaterWidget();
    virtual void GetObjectFromUI(QPushButtonMap &result);
    virtual void GetConstructConfig();
    virtual void SaveConstructConfig();

    QString GroupName;
    QString ConfigFilePath;
    QSettings *cfg;
};


#endif //QT_REPEATERWIDGET_H
