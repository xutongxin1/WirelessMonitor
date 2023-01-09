#ifndef MAIN_API_COMTOOLDEPEND_QUIHELPER_H_
#define MAIN_API_COMTOOLDEPEND_QUIHELPER_H_

#include "head.h"

class QuiHelper {
 public:
    //获取当前鼠标所在屏幕索引+尺寸
    static int GetScreenIndex();
    static QRect GetScreenRect(bool available = true);

    //获取桌面宽度高度+居中显示
    static int DeskWidth();
    static int DeskHeight();
    static QSize DeskSize();

    //居中显示窗体
    //定义标志位指定是以桌面为参照还是主程序界面为参照
    static QWidget *center_base_form_;
    static void SetFormInCenter(QWidget *form);
    static void ShowForm(QWidget *form);

    //程序文件名称+当前所在路径
    static QString AppName();
    static QString AppPath();

    //获取本地网卡IP集合
    static QStringList GetLocalIPs();

    //获取内置颜色集合
    static QList<QColor> colors_;
    static QList<QColor> GetColorList();
    static QStringList GetColorNames();
    //随机获取颜色集合中的颜色
    static QColor GetRandColor();

    //初始化随机数种子
    static void InitRand();
    //获取随机小数
    static float GetRandFloat(float min, float max);
    //获取随机数,指定最小值和最大值
    static double GetRandValue(int min, int max, bool contans_min = false, bool contans_max = false);
    //获取范围值随机经纬度集合
    static QStringList GetRandPoint(int count, float main_lng, float main_lat, float dot_lng, float dot_lat);

    //获取uuid
    static QString GetUuid();
    //可执行文件目录下新建目录
    static void NewDir(const QString &dir_name);
    //延时
    static void Sleep(int msec);

    //设置Qt自带样式
    static void SetStyle();
    //设置字体
    static QFont AddFont(const QString &font_file, const QString &font_name);
    static void SetFont(int font_size = 12);
    //设置编码
    static void SetCode(bool utf_8 = true);
    //设置翻译文件
    static void SetTranslator(const QString &qm_file);

    //一次性设置所有包括编码样式字体等
    static void InitAll(bool utf_8 = true, bool style = true, int font_size = 13);
    //初始化main函数最前面执行的一段代码
    static void InitMain(bool on = true);

    //插入消息
    static QVector<int> msg_types_;
    static QVector<QString> msg_keys_;
    static QVector<QColor> msg_colors_;
    static QString AppendMsg(QTextEdit *text_edit, int type, const QString &data,
                             int max_count, int &current_count,
                             bool clear = false, bool pause = false);

    //设置无边框
    static void SetFramelessForm(QWidget *widget_main, bool tool = false, bool top = false, bool menu = true);

    //弹出框
    static int ShowMessageBox(const QString &info, int type = 0, int close_sec = 0, bool exec = false);
    //弹出消息框
    static void ShowMessageBoxInfo(const QString &info, int close_sec = 0, bool exec = false);
    //弹出错误框
    static void ShowMessageBoxError(const QString &info, int close_sec = 0, bool exec = false);
    //弹出询问框
    static int ShowMessageBoxQuestion(const QString &info);

    //为什么还要自定义对话框因为可控宽高和汉化对应文本等
    //初始化对话框文本
    static void InitDialog(QFileDialog *dialog, const QString &title, const QString &accept_name,
                           const QString &dir_name, bool native, int width, int height);
    //拿到对话框结果
    static QString GetDialogResult(QFileDialog *dialog);
    //选择文件对话框
    static QString GetSelectFileName(const QString &filter = QString(),
                                     const QString &dir_name = QString(),
                                     const QString &file_name = QString(),
                                     bool native = false, int width = 900, int height = 600);
    //保存文件对话框
    static QString GetSavedFileName(const QString &filter = QString(),
                                    const QString &dir_name = QString(),
                                    const QString &file_name = QString(),
                                    bool native = false, int width = 900, int height = 600);
    //选择目录对话框
    static QString GetExistingDirectory(const QString &dir_name = QString(),
                                        bool native = false, int width = 900, int height = 600);

    //异或加密-只支持字符,如果是中文需要将其转换base64编码
    static QString GetXorEncryptDecrypt(const QString &value, char key);
    //异或校验
    static uchar GetOrCode(const QByteArray &data);
    //计算校验码
    static uchar GetCheckCode(const QByteArray &data);

    //初始化表格
    static void InitTableView(QTableView *table_view, int row_height = 25,
                              bool head_visible = false, bool edit = false,
                              bool stretch_last = true);
    //打开文件带提示框
    static void OpenFile(const QString &file_name, const QString &msg);

    //检查ini配置文件
    static bool CheckIniFile(const QString &ini_file);

    //首尾截断字符串显示
    static QString CutString(const QString &text, int len, int left, int right, const QString &mid = "...");
};

#endif // MAIN_API_COMTOOLDEPEND_QUIHELPER_H_
