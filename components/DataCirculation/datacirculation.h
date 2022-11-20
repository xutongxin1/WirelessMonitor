//
// Created by xtx on 2022/11/18.
//

#ifndef MAIN_DATACIRCULATION_H
#define MAIN_DATACIRCULATION_H

#include "RepeaterWidget.h"


QT_BEGIN_NAMESPACE
namespace Ui { class DataCirculation; }
QT_END_NAMESPACE

class DataCirculation : public RepeaterWidget {
    Q_OBJECT

public:
    explicit DataCirculation(QWidget *parent = nullptr);

    ~DataCirculation() override;
    enum CirculationMode{
        CirculationMode_Direction=0,
        CirculationMode_CommaSeparated,
        CirculationMode_KeyValue,
        CirculationMode_Scanf,
        CirculationMode_Regularity,
        CirculationMode_Python,
    };
private:
    Ui::DataCirculation *ui;
    void TestCirculationMode();
};


#endif //MAIN_DATACIRCULATION_H
