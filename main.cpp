#include "mainwindow.h"
#include "iostream"
#include <QApplication>
#include "Charts/charts.h"

//日志消息的处理函数
void logMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &message) {
    //获取格式化的日志信息
    QString typeStr = qFormatLogMessage(type, context, message);
    //可以根据日志的级别进行过滤
//    QString levelText;
    switch (type) {
        case QtDebugMsg:
            typeStr=QString("%1 \n(%2:%3, %4)\n").arg(typeStr).arg(context.file).arg(context.line).arg(context.function);
//            levelText = "Debug";
            break;
        case QtInfoMsg:
//            levelText = "Info";
            break;
        case QtWarningMsg:
//            levelText = "Warning";
            break;
        case QtCriticalMsg:
            typeStr=QString("%1 \n(%2:%3, %4)\n").arg(typeStr).arg(context.file).arg(context.line).arg(context.function);
//            levelText = "Critical";
            break;
        case QtFatalMsg:
            typeStr=QString("%1 \n(%2:%3, %4)\n").arg(typeStr).arg(context.file).arg(context.line).arg(context.function);
//            levelText = "Fatal";
            break;
    }
//    static QDateTime current_date_time =QDateTime::currentDateTime();
//    static QString current_date =current_date_time.toString("yyyy.MM.dd-hh:mm:ss.zzz");
    QFile file("./running.log");
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream textStream(&file);
    textStream << typeStr << endl;
    std::cout << typeStr.toStdString().data() << std::endl;
}

int main(int argc, char *argv[]) {
    qSetMessagePattern(
            "%{time yyyy-MM-dd hh:mm:ss} [%{type}]%{if-warning}[%{function}]%{endif}%{if-fatal}[%{function}--%{line}]%{endif}:%{message}");
    qInstallMessageHandler(logMessageHandler);//安装日志驱动
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
