#ifndef CHARTS_H
#define CHARTS_H

#include <QWidget>

namespace Ui {
class Charts;
}

class Charts : public QWidget
{
    Q_OBJECT

public:
    explicit Charts(QWidget *parent = nullptr);
    ~Charts();

private:
    Ui::Charts *ui;
};

#endif // CHARTS_H
