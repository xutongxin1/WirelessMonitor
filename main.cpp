#include "mainwindow.h"
#include "iostream"
#include <QApplication>

///日志消息的处理函数
void LogMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &message) {
    //获取格式化的日志信息
    QString type_str = qFormatLogMessage(type, context, message);
    //可以根据日志的级别进行过滤
//    QString levelText;
    switch (type) {
        case QtDebugMsg:
            //是否启动高详细度日志
//            type_str = QString("%1 (%2:%3, %4)\n").arg(type_str, context.file).arg(context.line).arg(context.function);
//            levelText = "debug_";
            break;
        case QtInfoMsg:
//            levelText = "Info";
            break;
        case QtWarningMsg:
            type_str = QString("%1 \n(%2:%3, %4)\n").arg(type_str,
                                                         context.file).arg(context.line).arg(context.function);
//            levelText = "Warning";
            break;
        case QtCriticalMsg:
            type_str = QString("%1 \n(%2:%3, %4)\n").arg(type_str,
                                                         context.file).arg(context.line).arg(context.function);
//            levelText = "Critical";
            break;
        case QtFatalMsg:
            type_str = QString("%1 \n(%2:%3, %4)\n").arg(type_str,
                                                         context.file).arg(context.line).arg(context.function);
//            levelText = "Fatal";
            break;

    }

    QDateTime current_time = QDateTime::currentDateTime();
    QString file_name = current_time.toString("yyyyMMdd") + "_DataLog.log";
    QString str_current_path = QCoreApplication::applicationDirPath() + "/log/" + file_name;
    QDir parent_path(QCoreApplication::applicationDirPath() + "/log/");
    if (!parent_path.exists()) {
        parent_path.mkpath(QCoreApplication::applicationDirPath() + "/log");//创建文件夹
    }

    QFile file(str_current_path);
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream text_stream(&file);
//    text_stream.setCodec(QTextCodec::codecForName("UTF8"));//设置编码格式
    text_stream << type_str << "\r\n";
    std::cout << type_str.toStdString().data() << std::endl;
}

int main(int argc, char *argv[]) {
    qSetMessagePattern(
        "%{time_ yyyy-MM-dd hh:mm:ss} [%{type}]%{if-warning}[%{function}]%{endif}%{if-fatal}[%{function}--%{line}]%{endif}:%{message}");
    qInstallMessageHandler(LogMessageHandler);//安装日志驱动
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
