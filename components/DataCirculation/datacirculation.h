//
// Created by xtx on 2022/11/18.
//

#ifndef MAIN_DATACIRCULATION_H
#define MAIN_DATACIRCULATION_H

#include "RepeaterWidget.h"
#include "Charts/charts.h"


QT_BEGIN_NAMESPACE
namespace Ui { class DataCirculation; }
QT_END_NAMESPACE
class DataCirculation : public RepeaterWidget {
Q_OBJECT

public:
    explicit DataCirculation(int DeviceNum, int winNum, QSettings *cfg, ToNewWidget *parentInfo,
                             QWidget *parent = nullptr);

    ~DataCirculation() override;

    enum CirculationMode {
        CirculationMode_Direction = 0,
        CirculationMode_CommaSeparated,
        CirculationMode_KeyValue,
        CirculationMode_Scanf,
        CirculationMode_Regularity,
        CirculationMode_Python,
    } circulationMode = CirculationMode_Direction;
    enum ProcessMode {
        ProcessMode_None = 0,
        ProcessMode_Circulation,
        ProcessMode_Output,
    } processMode = ProcessMode_None;
    enum DateFlowMode {
        DateFlowMode_Chart = 0,
        DateFlowMode_Output,
    } dateFlowMode = DateFlowMode_Chart;
    enum OutputMode {
        OutputMode_TCPServer = 0,
        OutputMode_POST,
    } outputMode = OutputMode_TCPServer;

    void GetConstructConfig() override;

    void SaveConstructConfig() override;

    void RefreshBox();

private:
    Ui::DataCirculation *ui;

    void TestCirculationMode();

    void StartCirculation();

    void DoCirculation(const QByteArray &data, QTime dataTime = QTime());

    Charts *chartWindow;

    struct value {
        QString name;
        QString rule;
    };

    std::list<value> values;
};


#endif //MAIN_DATACIRCULATION_H
