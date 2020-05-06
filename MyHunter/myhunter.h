#ifndef MYHUNTER_H
#define MYHUNTER_H

#include <QObject>
#include "drivermanager.h"
#include <QString>
#include <QStandardItemModel>
#include <QThread>
#include <QModelIndex>
#include <winioctl.h>
#include "../common/protocol.h"
#include "tableviewdialog.h"

// 字符转换宏
#ifdef UNICODE
#define QStringToTCHAR(x)     (wchar_t*) (x).utf16()
#define PQStringToTCHAR(x)    (wchar_t*) (x)->utf16()
#define TCHARToQString(x)     QString::fromUtf16((ushort *)(x))
#define TCHARToQStringN(x,y)  QString::fromUtf16((x),(y))
#else
#define QStringToTCHAR(x)     (x).local8Bit().constData()
#define PQStringToTCHAR(x)    (x)->local8Bit().constData()
#define TCHARToQString(x)     QString::fromLocal8Bit((x))
#define TCHARToQStringN(x,y)  QString::fromLocal8Bit((x),(y))
#endif

// model列宏
#define M_PROCESS_NAME 0        // 进程名
#define M_PROCESS_PID 1         // 进程ID
#define M_PROCESS_PPID 2        // 父进程ID
#define M_PROCESS_IMAGE_PATH 3  // 映像路径
#define M_PROCESS_EPROCESS 4    // EPROCESS
#define M_PROCESS_PRIVILEGE 5   // 特权级

class MyHunter : public QObject
{
    Q_OBJECT
public:
    explicit MyHunter(QObject *parent = nullptr);
    virtual ~MyHunter();

    bool initialization();      // 初始化
    bool uninitialization();    // 反初始化

    QStandardItemModel *get_tableView_model() const;    // 获取tableView模型
    QStandardItemModel *get_child_tableView_model() const;  // 获取子窗口的tableView模型
    void tableView_model_clear();       // 清空tableView模型数据
    void tableView_model_clearall();    // 清空tableView模型所有数据（包括表头）
    void tableView_child_model_clear();         // 清空子窗口tableView的模型数据
    void tableView_child_model_clearall();      // 清空子窗口的tableView模型所有数据（包括表头）

    void set_tableView_header(QStringList &string_list);            // 设置tableView模型表头
    void set_child_tableView_header(QStringList &string_list);      // 设置子窗口的tableView模型表头

    QString get_module_index_text(QStandardItemModel *module, QModelIndex model_index, unsigned col = 0);    // 获取指定模型当前选择的项的第col项文本


    static unsigned __stdcall get_gdt(void *arg);       // 获取GDT数据
    void deal_gdt_data(BYTE *buf, DWORD item_count, DWORD cpu_core);    // 处理GDT数据

    static unsigned __stdcall get_idt(void *arg);       // 获取IDT数据
    void deal_idt_data(BYTE *buf, DWORD item_count, DWORD cpu_core);    // 处理IDT数据

    static unsigned __stdcall get_ssdt(void *arg);          // 获取SSDT
    static unsigned __stdcall get_shadow_ssdt(void *arg);   // 获取ShadowSSDT
    static unsigned __stdcall get_driver_module(void *arg); // 获取驱动模块

    static unsigned __stdcall get_process_list(void *arg);      // 获取进程列表
    static unsigned __stdcall get_process_module_list(void *arg);   // 获取进程模块列表
    static unsigned __stdcall get_process_thread_list(void *arg);   // 获取进程线程列表
    static unsigned __stdcall get_process_handle_list(void *arg);   // 获取进程句柄列表


    // 参数相关
    DWORD param_pid = 0;

protected:
    DWORD get_segdes_base(SegmentDescriptor *segment_descriptor);       // 获取段描述符中的段基址
    DWORD get_segdes_limit(SegmentDescriptor *segment_descriptor, bool is_comput = false);      // 获取段描述符中的段界限

signals:
    void tableview_append(QStringList labels);
    void child_tableview_append(QStringList labels);
    void child_wnd_set_modle();

public slots:


private:
    DriverManager *driver_manager_ = nullptr;       // 驱动管理器
    const QString driver_name_ = MY_DRIVER_FILE;    // 驱动文件名

    QStandardItemModel *model_tableView_ = nullptr; // tableView模型
    QStandardItemModel *model_child_tableView_ = nullptr;   // 子窗口的tableView模型

    
    int cpu_core_count_ = 0;    // cpu核心数量
};

#endif // MYHUNTER_H
