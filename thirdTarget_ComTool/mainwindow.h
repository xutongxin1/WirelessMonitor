#ifndef MAIN__MAINWINDOW_H_
#define MAIN__MAINWINDOW_H_

#include <QMainWindow>

#include "qtmaterialsnackbar.h"
#include "qtmaterialtabs.h"


using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
 Q_OBJECT

 public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;
    QtMaterialSnackbar *snackbar_ = new QtMaterialSnackbar(this);
    void ReceiveOrderShowSnackbar(const QString &message);
 private:
    Ui::MainWindow *ui_;

    QString version_;

};

#endif // MAIN__MAINWINDOW_H_
