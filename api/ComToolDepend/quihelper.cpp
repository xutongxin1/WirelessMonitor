#include "quihelper.h"
#include "qnetworkinterface.h"
#include "qnetworkproxy.h"

#define TIMEMS qPrintable(QTime::currentTime().toString("HH:mm:ss zzz"))
int QuiHelper::GetScreenIndex() {
    //需要对多个屏幕进行处理
    int screen_index = 0;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    int screen_count = qApp->screens().count();
#else
    int screen_count = qApp->desktop()->screen_count();
#endif

    if (screen_count > 1) {
        //找到当前鼠标所在屏幕
        QPoint pos = QCursor::pos();
        for (int i = 0; i < screen_count; ++i) {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
            if (qApp->screens().at(i)->geometry().contains(pos)) {
#else
                if (qApp->desktop()->screenGeometry(i).contains(pos)) {
#endif
                screen_index = i;
                break;
            }
        }
    }
    return screen_index;
}

QRect QuiHelper::GetScreenRect(bool available) {
    QRect rect;
    int screen_index = QuiHelper::GetScreenIndex();
    if (available) {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        rect = qApp->screens().at(screen_index)->availableGeometry();
#else
        rect = qApp->desktop()->availableGeometry(screen_index);
#endif
    } else {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        rect = qApp->screens().at(screen_index)->geometry();
#else
        rect = qApp->desktop()->screenGeometry(screen_index);
#endif
    }
    return rect;
}

int QuiHelper::DeskWidth() {
    return GetScreenRect().width();
}

int QuiHelper::DeskHeight() {
    return GetScreenRect().height();
}

QSize QuiHelper::DeskSize() {
    return GetScreenRect().size();
}

QWidget *QuiHelper::center_base_form_ = 0;
void QuiHelper::SetFormInCenter(QWidget *form) {
    int form_width = form->width();
    int form_height = form->height();

    //如果=0表示采用系统桌面屏幕为参照
    QRect rect;
    if (center_base_form_ == 0) {
        rect = GetScreenRect();
    } else {
        rect = center_base_form_->geometry();
    }

    int desk_width = rect.width();
    int desk_height = rect.height();
    QPoint movePoint(desk_width / 2 - form_width / 2 + rect.x(), desk_height / 2 - form_height / 2 + rect.y());
    form->move(movePoint);
}

void QuiHelper::ShowForm(QWidget *form) {
    SetFormInCenter(form);
    form->show();

    //判断宽高是否超过了屏幕分辨率,超过了则最大化显示
    //qDebug() << TIMEMS << form->size() << DeskSize();
    if (form->width() + 20 > DeskWidth() || form->height() + 50 > DeskHeight()) {
        QMetaObject::invokeMethod(form, "showMaximized", Qt::QueuedConnection);
    }
}

QString QuiHelper::AppName() {
    //没有必要每次都获取,只有当变量为空时才去获取一次
    static QString name;
    if (name.isEmpty()) {
        name = qApp->applicationFilePath();
        //下面的方法主要为了过滤安卓的路径 lib程序名_armeabi-v7a
        QStringList list = name.split("/");
        name = list.at(list.count() - 1).split(".").at(0);
        name.replace("_armeabi-v7a", "");
    }

    return name;
}

QString QuiHelper::AppPath() {
    static QString path;
    if (path.isEmpty()) {
#ifdef Q_OS_ANDROID
        //默认安卓根目录
        path = "/storage/emulated/0";
        //带上程序名称作为目录 前面加个0方便排序
        path = path + "/0" + appName();
#else
        path = qApp->applicationDirPath();
#endif
    }

    return path;
}

QStringList QuiHelper::GetLocalIPs() {
    static QStringList ips;
    if (ips.count() == 0) {
#ifdef Q_OS_WASM
        ips << "127.0.0.1";
#else
        QList<QNetworkInterface> netInterfaces = QNetworkInterface::allInterfaces();
            foreach (const QNetworkInterface &netInterface, netInterfaces) {
                //移除虚拟机和抓包工具的虚拟网卡
                QString humanReadableName = netInterface.humanReadableName().toLower();
                if (humanReadableName.startsWith("vmware network adapter")
                    || humanReadableName.startsWith("npcap loopback adapter")) {
                    continue;
                }

                //过滤当前网络接口
                bool flag = (netInterface.flags()
                    == (QNetworkInterface::IsUp | QNetworkInterface::IsRunning | QNetworkInterface::CanBroadcast
                        | QNetworkInterface::CanMulticast));
                if (!flag) {
                    continue;
                }

                QList<QNetworkAddressEntry> addrs = netInterface.addressEntries();
                    foreach (QNetworkAddressEntry addr, addrs) {
                        //只取出IPV4的地址
                        if (addr.ip().protocol() != QAbstractSocket::IPv4Protocol) {
                            continue;
                        }

                        QString ip4 = addr.ip().toString();
                        if (ip4 != "127.0.0.1") {
                            ips << ip4;
                        }
                    }
            }
#endif
    }

    return ips;
}

QList<QColor> QuiHelper::colors_ = QList<QColor>();
QList<QColor> QuiHelper::GetColorList() {
    //备用颜色集合 可以自行添加
    if (colors_.count() == 0) {
        colors_ << QColor(0, 176, 180) << QColor(0, 113, 193) << QColor(255, 192, 0);
        colors_ << QColor(72, 103, 149) << QColor(185, 87, 86) << QColor(0, 177, 125);
        colors_ << QColor(214, 77, 84) << QColor(71, 164, 233) << QColor(34, 163, 169);
        colors_ << QColor(59, 123, 156) << QColor(162, 121, 197) << QColor(72, 202, 245);
        colors_ << QColor(0, 150, 121) << QColor(111, 9, 176) << QColor(250, 170, 20);
    }

    return colors_;
}

QStringList QuiHelper::GetColorNames() {
    QList<QColor> colors = GetColorList();
    QStringList colorNames;
        foreach (QColor color, colors) {
            colorNames << color.name();
        }
    return colorNames;
}

QColor QuiHelper::GetRandColor() {
    QList<QColor> colors = GetColorList();
    int index = GetRandValue(0, colors.count(), true);
    return colors.at(index);
}

void QuiHelper::InitRand() {
    //初始化随机数种子
    QTime t = QTime::currentTime();
    srand(t.msec() + t.second() * 1000);
}

float QuiHelper::GetRandFloat(float min, float max) {
    double diff = fabs(max - min);
    double value = (double) (rand() % 100) / 100;
    value = min + value * diff;
    return value;
}

double QuiHelper::GetRandValue(int min, int max, bool contans_min, bool contans_max) {
    int value;
#if (QT_VERSION <= QT_VERSION_CHECK(5, 10, 0))
    //通用公式 a是起始值,n是整数的范围
    //int value = a + rand() % n;
    if (contans_min) {
        if (contans_max) {
            value = min + 0 + (rand() % (max - min + 1));
        } else {
            value = min + 0 + (rand() % (max - min + 0));
        }
    } else {
        if (contans_max) {
            value = min + 1 + (rand() % (max - min + 0));
        } else {
            value = min + 1 + (rand() % (max - min - 1));
        }
    }
#else
    if (contans_min) {
        if (contans_max) {
            value = QRandomGenerator::global()->bounded(min + 0, max + 1);
        } else {
            value = QRandomGenerator::global()->bounded(min + 0, max + 0);
        }
    } else {
        if (contans_max) {
            value = QRandomGenerator::global()->bounded(min + 1, max + 1);
        } else {
            value = QRandomGenerator::global()->bounded(min + 1, max + 0);
        }
    }
#endif
    return value;
}

QStringList QuiHelper::GetRandPoint(int count, float main_lng, float main_lat, float dot_lng, float dot_lat) {
    //随机生成点坐标
    QStringList points;
    for (int i = 0; i < count; ++i) {
        //0.00881415 0.000442928
#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
        float lngx = QRandomGenerator::global()->bounded(dot_lng);
        float latx = QRandomGenerator::global()->bounded(dot_lat);
#else
        float lngx = getRandFloat(dot_lng / 10, dot_lng);
        float latx = getRandFloat(dot_lat / 10, dot_lat);
#endif
        //需要先用精度转换成字符串
        QString lng2 = QString::number(main_lng + lngx, 'f', 8);
        QString lat2 = QString::number(main_lat + latx, 'f', 8);
        QString point = QString("%1,%2").arg(lng2).arg(lat2);
        points << point;
    }

    return points;
}

QString QuiHelper::GetUuid() {
    QString uuid = QUuid::createUuid().toString();
    uuid.replace("{", "");
    uuid.replace("}", "");
    return uuid;
}

void QuiHelper::NewDir(const QString &dir_name) {
    QString strDir = dir_name;

    //如果路径中包含斜杠字符则说明是绝对路径
    //linux系统路径字符带有 /  windows系统 路径字符带有 :/
    if (!strDir.startsWith("/") && !strDir.contains(":/")) {
        strDir = QString("%1/%2").arg(QuiHelper::AppPath()).arg(strDir);
    }

    QDir dir(strDir);
    if (!dir.exists()) {
        dir.mkpath(strDir);
    }
}

void QuiHelper::Sleep(int msec) {
    if (msec <= 0) {
        return;
    }

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QThread::msleep(msec);
#else
    QTime endTime = QTime::currentTime().addMSecs(msec);
    while (QTime::currentTime() < endTime) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
#endif
}

void QuiHelper::SetStyle() {
    //打印下所有内置风格的名字
    qDebug() << TIMEMS << "QStyleFactory::keys" << QStyleFactory::keys();
    //设置内置风格
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    qApp->setStyle(QStyleFactory::create("Fusion"));
#else
    qApp->setStyle(QStyleFactory::create("Cleanlooks"));
#endif

    //设置指定颜色
    QPalette palette;
    palette.setBrush(QPalette::Window, QColor("#F0F0F0"));
    qApp->setPalette(palette);
}

QFont QuiHelper::AddFont(const QString &font_file, const QString &font_name) {
    //判断图形字体是否存在,不存在则加入
    QFontDatabase fontDb;
    if (!fontDb.families().contains(font_name)) {
        int fontId = fontDb.addApplicationFont(font_file);
        QStringList listName = fontDb.applicationFontFamilies(fontId);
        if (listName.count() == 0) {
            qDebug() << QString("load %1 error").arg(font_name);
        }
    }

    //再次判断是否包含字体名称防止加载失败
    QFont font;
    if (fontDb.families().contains(font_name)) {
        font = QFont(font_name);
#if (QT_VERSION >= QT_VERSION_CHECK(4, 8, 0))
        font.setHintingPreference(QFont::PreferNoHinting);
#endif
    }

    return font;
}

void QuiHelper::SetFont(int font_size) {
#ifdef rk3399
    return;
#endif
    //安卓套件在有些手机上默认字体不好看需要主动设置字体
    //网页套件需要主动加载中文字体才能正常显示中文
#if (defined Q_OS_ANDROID) || (defined Q_OS_WASM)
    QString fontFile = ":/font/DroidSansFallback.ttf";
    QString fontName = "Droid Sans Fallback";
    qApp->setFont(addFont(fontFile, fontName));
    return;
#endif

#ifdef __arm__
    font_size = 25;
#endif
#ifdef Q_OS_ANDROID
    font_size = 15;
#endif

    QFont font;
    font.setFamily("MicroSoft Yahei");
    font.setPixelSize(font_size);
    qApp->setFont(font);
}

void QuiHelper::SetCode(bool utf_8) {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    //如果想要控制台打印信息中文正常就注释掉这个设置
    if (utf_8) {
        QTextCodec *codec = QTextCodec::codecForName("utf-8");
        QTextCodec::setCodecForLocale(codec);
    }
#else
#if _MSC_VER
    QTextCodec *codec = QTextCodec::codecForName("gbk");
#else
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
#endif
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);
#endif
}

void QuiHelper::SetTranslator(const QString &qm_file) {
    //过滤下不存在的就不用设置了
    if (!QFile(qm_file).exists()) {
        return;
    }

    QTranslator *translator = new QTranslator(qApp);
    if (translator->load(qm_file)) {
        qApp->installTranslator(translator);
    }
}

void QuiHelper::InitAll(bool utf_8, bool style, int font_size) {
    //初始化随机数种子
    QuiHelper::InitRand();
    //设置编码
    QuiHelper::SetCode(utf_8);
    //设置样式风格
    if (style) {
        QuiHelper::SetStyle();
    }
    //设置字体
    QuiHelper::SetFont(font_size);
    //设置翻译文件支持多个
    QuiHelper::SetTranslator(":/qm/widgets.qm");
    QuiHelper::SetTranslator(":/qm/qt_zh_CN.qm");
    QuiHelper::SetTranslator(":/qm/designer_zh_CN.qm");
    //设置不使用本地系统环境代理配置
    QNetworkProxyFactory::setUseSystemConfiguration(false);
}

void QuiHelper::InitMain(bool on) {
    //设置是否应用操作系统设置比如字体
    QApplication::setDesktopSettingsAware(on);

#ifdef Q_OS_ANDROID
#if (QT_VERSION >= QT_VERSION_CHECK(5,6,0))
    //开启高分屏缩放支持
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
#else
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    //不应用任何缩放
    QApplication::setAttribute(Qt::AA_Use96Dpi);
#endif
#endif

#if (QT_VERSION >= QT_VERSION_CHECK(5,14,0))
    //高分屏缩放策略
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::Floor);
#endif

#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
    //设置opengl模式 AA_UseDesktopOpenGL(默认) AA_UseOpenGLES AA_UseSoftwareOpenGL
    //在一些很旧的设备上或者对opengl支持很低的设备上需要使用AA_UseOpenGLES表示禁用硬件加速
    //如果开启的是AA_UseOpenGLES则无法使用硬件加速比如ffmpeg的dxva2
    //QApplication::setAttribute(Qt::AA_UseOpenGLES);
    //设置opengl共享上下文
    QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
#endif
}

QVector<int> QuiHelper::msg_types_ = QVector<int>() << 0 << 1 << 2 << 3 << 4;
QVector<QString> QuiHelper::msg_keys_ = QVector<QString>() << "发送" << "接收" << "解析" << "错误" << "提示";
QVector<QColor>QuiHelper::msg_colors_ =
    QVector<QColor>() << QColor("#3BA372") << QColor("#EE6668") << QColor("#9861B4") << QColor("#FA8359")
                      << QColor("#22A3A9");
QString QuiHelper::AppendMsg(QTextEdit *text_edit,
                             int type,
                             const QString &data,
                             int max_count,
                             int &current_count,
                             bool clear,
                             bool pause) {
    if (clear) {
        text_edit->clear();
        current_count = 0;
        return QString();
    }

    if (pause) {
        return QString();
    }

    if (current_count >= max_count) {
        text_edit->clear();
        current_count = 0;
    }

    //不同类型不同颜色显示
    QString strType;
    int index = msg_types_.indexOf(type);
    if (index >= 0) {
        strType = msg_keys_.at(index);
        text_edit->setTextColor(msg_colors_.at(index));
    }

    //过滤回车换行符
    QString strData = data;
    strData.replace("\r", "");
    strData.replace("\n", "");
    strData = QString("时间[%1] %2: %3").arg(TIMEMS).arg(strType).arg(strData);
    text_edit->append(strData);
    current_count++;
    return strData;
}

void QuiHelper::SetFramelessForm(QWidget *widget_main, bool tool, bool top, bool menu) {
    widget_main->setProperty("form", true);
    widget_main->setProperty("canMove", true);

    //根据设定逐个追加属性
#ifdef __arm__
    widget_main->setWindowFlags(Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint);
#else
    widget_main->setWindowFlags(Qt::FramelessWindowHint);
#endif
    if (tool) {
        widget_main->setWindowFlags(widget_main->windowFlags() | Qt::Tool);
    }
    if (top) {
        widget_main->setWindowFlags(widget_main->windowFlags() | Qt::WindowStaysOnTopHint);
    }
    if (menu) {
        //如果是其他系统比如neokylin会产生系统边框
#ifdef Q_OS_WIN
        widget_main->setWindowFlags(
            widget_main->windowFlags() | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint);
#endif
    }
}

int QuiHelper::ShowMessageBox(const QString &info, int type, int close_sec, bool exec) {
    int result = 0;
    if (type == 0) {
        ShowMessageBoxInfo(info, close_sec, exec);
    } else if (type == 1) {
        ShowMessageBoxError(info, close_sec, exec);
    } else if (type == 2) {
        result = ShowMessageBoxQuestion(info);
    }

    return result;
}

void QuiHelper::ShowMessageBoxInfo(const QString &info, int close_sec, bool exec) {
    QMessageBox box(QMessageBox::Information, "提示", info);
    box.setStandardButtons(QMessageBox::Yes);
    box.setButtonText(QMessageBox::Yes, QString("确 定"));
    box.exec();
    //QMessageBox::information(0, "提示", info, QMessageBox::Yes);
}

void QuiHelper::ShowMessageBoxError(const QString &info, int close_sec, bool exec) {
    QMessageBox box(QMessageBox::Critical, "错误", info);
    box.setStandardButtons(QMessageBox::Yes);
    box.setButtonText(QMessageBox::Yes, QString("确 定"));
    box.exec();
    //QMessageBox::critical(0, "错误", info, QMessageBox::Yes);
}

int QuiHelper::ShowMessageBoxQuestion(const QString &info) {
    QMessageBox box(QMessageBox::Question, "询问", info);
    box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    box.setButtonText(QMessageBox::Yes, QString("确 定"));
    box.setButtonText(QMessageBox::No, QString("取 消"));
    return box.exec();
    //return QMessageBox::question(0, "询问", info, QMessageBox::Yes | QMessageBox::No);
}

void QuiHelper::InitDialog(QFileDialog *dialog, const QString &title, const QString &accept_name,
                           const QString &dir_name, bool native, int width, int height) {
    //设置标题
    dialog->setWindowTitle(title);
    //设置标签文本
    dialog->setLabelText(QFileDialog::Accept, accept_name);
    dialog->setLabelText(QFileDialog::Reject, "取消(&C)");
    dialog->setLabelText(QFileDialog::LookIn, "查看");
    dialog->setLabelText(QFileDialog::FileName, "名称");
    dialog->setLabelText(QFileDialog::FileType, "类型");

    //设置默认显示目录
    if (!dir_name.isEmpty()) {
        dialog->setDirectory(dir_name);
    }

    //设置对话框宽高
    if (width > 0 && height > 0) {
#ifdef Q_OS_ANDROID
        bool horizontal = (QuiHelper::deskWidth() > QuiHelper::deskHeight());
        if (horizontal) {
            width = QuiHelper::deskWidth() / 2;
            height = QuiHelper::deskHeight() - 50;
        } else {
            width = QuiHelper::deskWidth() - 10;
            height = QuiHelper::deskHeight() / 2;
        }
#endif
        dialog->setFixedSize(width, height);
    }

    //设置是否采用本地对话框
    dialog->setOption(QFileDialog::DontUseNativeDialog, !native);
    //设置只读可以取消右上角的新建按钮
    //dialog->setReadOnly(true);
}

QString QuiHelper::GetDialogResult(QFileDialog *dialog) {
    QString result;
    if (dialog->exec() == QFileDialog::Accepted) {
        result = dialog->selectedFiles().first();
    }
    return result;
}

QString QuiHelper::GetSavedFileName(const QString &filter, const QString &dir_name, const QString &file_name,
                                    bool native, int width, int height) {
    QFileDialog dialog;
    InitDialog(&dialog, "打开文件", "选择(&S)", dir_name, native, width, height);

    //设置文件类型
    if (!filter.isEmpty()) {
        dialog.setNameFilter(filter);
    }

    //设置默认文件名称
    dialog.selectFile(file_name);
    return GetDialogResult(&dialog);
}

QString QuiHelper::GetSelectFileName(const QString &filter, const QString &dir_name, const QString &file_name,
                                     bool native, int width, int height) {
    QFileDialog dialog;
    InitDialog(&dialog, "保存文件", "保存(&S)", dir_name, native, width, height);

    //设置文件类型
    if (!filter.isEmpty()) {
        dialog.setNameFilter(filter);
    }

    //设置默认文件名称
    dialog.selectFile(file_name);
    //设置模态类型允许输入
    dialog.setWindowModality(Qt::WindowModal);
    //设置置顶显示
    dialog.setWindowFlags(dialog.windowFlags() | Qt::WindowStaysOnTopHint);
    return GetDialogResult(&dialog);
}

QString QuiHelper::GetExistingDirectory(const QString &dir_name, bool native, int width, int height) {
    QFileDialog dialog;
    InitDialog(&dialog, "选择目录", "选择(&S)", dir_name, native, width, height);
    dialog.setOption(QFileDialog::ReadOnly);
    //设置只显示目录
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    dialog.setFileMode(QFileDialog::DirectoryOnly);
#endif
    dialog.setOption(QFileDialog::ShowDirsOnly);
    return GetDialogResult(&dialog);
}

QString QuiHelper::GetXorEncryptDecrypt(const QString &value, char key) {
    //矫正范围外的数据
    if (key < 0 || key >= 127) {
        key = 127;
    }

    //大概从5.9版本输出的加密密码字符串前面会加上 @String 字符
    QString result = value;
    if (result.startsWith("@String")) {
        result = result.mid(8, result.length() - 9);
    }

    int count = result.count();
    for (int i = 0; i < count; ++i) {
        result[i] = QChar(result.at(i).toLatin1() ^ key);
    }
    return result;
}

uchar QuiHelper::GetOrCode(const QByteArray &data) {
    int len = data.length();
    uchar result = 0;
    for (int i = 0; i < len; ++i) {
        result ^= data.at(i);
    }

    return result;
}

uchar QuiHelper::GetCheckCode(const QByteArray &data) {
    int len = data.length();
    uchar temp = 0;
    for (uchar i = 0; i < len; ++i) {
        temp += data.at(i);
    }

    return temp % 256;
}

void QuiHelper::InitTableView(QTableView *tableView, int rowHeight, bool headVisible, bool edit, bool stretchLast) {
    //设置弱属性用于应用qss特殊样式
    tableView->setProperty("model", true);
    //取消自动换行
    tableView->setWordWrap(false);
    //超出文本不显示省略号
    tableView->setTextElideMode(Qt::ElideNone);
    //奇数偶数行颜色交替
    tableView->setAlternatingRowColors(false);
    //垂直表头是否可见
    tableView->verticalHeader()->setVisible(headVisible);
    //选中一行表头是否加粗
    tableView->horizontalHeader()->setHighlightSections(false);
    //最后一行拉伸填充
    tableView->horizontalHeader()->setStretchLastSection(stretchLast);
    //行标题最小宽度尺寸
    tableView->horizontalHeader()->setMinimumSectionSize(0);
    //行标题最小高度,等同于和默认行高一致
    tableView->horizontalHeader()->setFixedHeight(rowHeight);
    //默认行高
    tableView->verticalHeader()->setDefaultSectionSize(rowHeight);
    //选中时一行整体选中
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    //只允许选择单个
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);

    //表头不可单击
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
    tableView->horizontalHeader()->setSectionsClickable(false);
#else
    tableView->horizontalHeader()->setClickable(false);
#endif

    //鼠标按下即进入编辑模式
    if (edit) {
        tableView->setEditTriggers(QAbstractItemView::CurrentChanged | QAbstractItemView::DoubleClicked);
    } else {
        tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    }
}

void QuiHelper::OpenFile(const QString &file_name, const QString &msg) {
#ifdef __arm__
    return;
#endif
    //文件不存在则不用处理
    if (!QFile(file_name).exists()) {
        return;
    }
    if (QuiHelper::ShowMessageBoxQuestion(msg + "成功, 确定现在就打开吗?") == QMessageBox::Yes) {
        QString url = QString("file:///%1").arg(file_name);
        QDesktopServices::openUrl(QUrl(url, QUrl::TolerantMode));
    }
}

bool QuiHelper::CheckIniFile(const QString &iniFile) {
    //如果配置文件大小为0,则以初始值继续运行,并生成配置文件
    QFile file(iniFile);
    if (file.size() == 0) {
        return false;
    }

    //如果配置文件不完整,则以初始值继续运行,并生成配置文件
    if (file.open(QFile::ReadOnly)) {
        bool ok = true;
        while (!file.atEnd()) {
            QString line = file.readLine();
            line.replace("\r", "");
            line.replace("\n", "");
            QStringList list = line.split("=");

            if (list.count() == 2) {
                QString key = list.at(0);
                QString value = list.at(1);
                if (value.isEmpty()) {
                    qDebug() << TIMEMS << "ini node no value" << key;
                    ok = false;
                    break;
                }
            }
        }

        if (!ok) {
            return false;
        }
    } else {
        return false;
    }

    return true;
}

QString QuiHelper::CutString(const QString &text, int len, int left, int right, const QString &mid) {
    //如果是文件名则取文件名的前字符+末尾字符+去掉拓展名
    QString result = text.split(".").first();
    if (result.length() > len) {
        result = QString("%1%2%3").arg(result.left(left)).arg(mid).arg(result.right(right));
    }
    return result;
}
