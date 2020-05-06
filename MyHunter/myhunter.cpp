#include "myhunter.h"
#include <QCoreApplication>
#include <TlHelp32.h>
#include "msgboxunit.h"
#include <QDebug>

MyHunter::MyHunter(QObject *parent) : QObject(parent)
{
    driver_manager_ = new DriverManager();              // 创建驱动管理器
    model_tableView_ = new QStandardItemModel(this);    // 创建tableView模型
    model_child_tableView_ = new QStandardItemModel(this);  // 创建子窗口的tableView模型
    cpu_core_count_ = QThread::idealThreadCount();      // 获取cpu核心数量

    // 连接信号槽
    connect(this, static_cast<void (MyHunter::*)(QStringList)>(&MyHunter::tableview_append),
        this, [=](QStringList labels) {
            // tableView模型添加一行信息
            QList<QStandardItem *> items;
            for(auto &item : labels) {
                items.push_back(new QStandardItem(item));
            }
            model_tableView_->appendRow(items);
        });

    connect(this, static_cast<void (MyHunter::*)(QStringList)>(&MyHunter::child_tableview_append),
        this, [=](QStringList labels) {
            // tableView模型添加一行信息
            QList<QStandardItem *> items;
            for(auto &item : labels) {
                items.push_back(new QStandardItem(item));
            }
            model_child_tableView_->appendRow(items);
        });
}

MyHunter::~MyHunter()
{
    if (driver_manager_)
        delete driver_manager_;
}

bool MyHunter::initialization()
{
    // 获取本模块路径
    QString driver_path = QCoreApplication::applicationDirPath();
    if(driver_path[driver_path.length()] == "/") {
        driver_path += driver_name_;
    } else {
        driver_path += ("/" + driver_name_);
    }

#ifndef _DEBUG
    // 设置驱动管理器路径
    driver_manager_->set_driver(QStringToTCHAR(driver_path));

    // 安装驱动
    if(!driver_manager_->install_driver()) {
        MsgBoxUnit::msgbox_critical(QString("DriverManager Error"), 
            QString("[install_driver] error: %1").arg((ulong)driver_manager_->get_last_error()));
        return false;
    }

    // 启动驱动
    if (!driver_manager_->start_driver()) {
        MsgBoxUnit::msgbox_critical(QString("DriverManager Error"),
            QString("[start_driver] error: %1").arg((ulong)driver_manager_->get_last_error()));
        return false;
    }

    // 连接驱动
    if (!driver_manager_->connect_driver(MY_SYMBOL_FILE)) {
        MsgBoxUnit::msgbox_critical(QString("DriverManager Error"),
            QString("[connect_driver] error: %1").arg((ulong)driver_manager_->get_last_error()));
        return false;
    }
#endif

    return true;
}

bool MyHunter::uninitialization()
{
#ifndef _DEBUG
    // 关闭连接
    driver_manager_->disconnect_driver();

    // 停止驱动
    if(!driver_manager_->stop_driver()) {
        MsgBoxUnit::msgbox_critical(QString("DriverManager Error"),
            QString("[stop_driver] error: %1").arg((ulong)driver_manager_->get_last_error()));
        return false;
    }

    // 卸载驱动
    if (!driver_manager_->uninstall_driver()) {
        MsgBoxUnit::msgbox_critical(QString("DriverManager Error"),
            QString("[uninstall_driver] error: %1").arg((ulong)driver_manager_->get_last_error()));
        return false;
    }
#endif

    return true;
}

QStandardItemModel * MyHunter::get_tableView_model() const
{
    return model_tableView_;
}

QStandardItemModel * MyHunter::get_child_tableView_model() const
{
    return model_child_tableView_;
}

void MyHunter::tableView_model_clear()
{
    model_tableView_->removeRows(0, model_tableView_->rowCount());
}

void MyHunter::tableView_model_clearall()
{
    model_tableView_->clear();
}

void MyHunter::tableView_child_model_clear()
{
    model_child_tableView_->removeRows(0, model_child_tableView_->rowCount());
}

void MyHunter::tableView_child_model_clearall()
{
    model_child_tableView_->clear();
}

void MyHunter::set_tableView_header(QStringList &string_list)
{
    if (!model_tableView_)
        return;
    model_tableView_->setHorizontalHeaderLabels(string_list);
}

void MyHunter::set_child_tableView_header(QStringList &string_list)
{
    if (!model_child_tableView_)
        return;
    model_child_tableView_->setHorizontalHeaderLabels(string_list);
}

QString MyHunter::get_module_index_text(QStandardItemModel *module, QModelIndex model_index, unsigned col)
{
    return module->index(model_index.row(), col).data().toString();
}

unsigned MyHunter::get_gdt(void *arg)
{
    MyHunter *my_hunter = (MyHunter *)arg;
    DriverManager *driver_manager = my_hunter->driver_manager_; // 驱动管理器

    int core_count = my_hunter->cpu_core_count_; // cpu核心数量
    DWORD mark = 0;     // cpu核心的关联掩码
    
    for(int i = 0; i < core_count; i++) {
        // 设置关联掩码
        mark = (1 << i);

        // 向驱动发送控制信息
        DWORD ret_bytes = 0;

        // 获取输出缓冲区大小，输入为限定核心掩码
        DWORD buf_size = 0;
        driver_manager->io_control(CODE_GET_GDT_BUFSIZE, &mark, sizeof(mark), &buf_size, sizeof(buf_size), &ret_bytes);
        if(buf_size != 0) {
            // 申请输出缓冲区
            BYTE *buf = new BYTE[buf_size];

            // 获取GDT，输入为限定核心掩码
            driver_manager->io_control(CODE_GET_GDT, &mark, sizeof(mark), buf, buf_size, &ret_bytes);

            // 处理GDT数据
            if(ret_bytes != 0) {
                my_hunter->deal_gdt_data(buf, ret_bytes / sizeof(SegmentDescriptor), i);
            }

            // 释放输出缓冲区空间
            delete[] buf;
        }
    }


    return 0;
}

void MyHunter::deal_gdt_data(BYTE *buf, DWORD item_count, DWORD cpu_core)
{
    QStringList labels;
    QString core_num = QString("%1").arg((uint)cpu_core);   // 获取cpu核心编号

    // 遍历dgt
    SegmentDescriptor *seg_des_ptr = (SegmentDescriptor *)buf;
    for(DWORD i = 0; i < item_count; i++, seg_des_ptr++) {
        // 判断段描述符是否有效
        if(seg_des_ptr->SegDesHigh.p == 1) {
            labels.clear();

            // 添加cpu核心编号
            labels << core_num;

            // 段选择子
            labels << "0x" + QString("%1").arg((uint)i, 4, 16, QLatin1Char('0')).toUpper();

            //段基址
            labels << "0x" + QString("%1").arg((uint)get_segdes_base(seg_des_ptr), 8, 16, QLatin1Char('0')).toUpper();

            //段界限
            labels << "0x" + QString("%1").arg((uint)get_segdes_limit(seg_des_ptr), 8, 16, QLatin1Char('0')).toUpper();

            // 段粒度
            if(seg_des_ptr->SegDesHigh.g == 1) {
                labels << "Page";
            } else {
                labels << "Byte";
            }

            // 特权级
            labels << QString("%1").arg((uint)seg_des_ptr->SegDesHigh.dpl);

            // 类型
            QString seg_type;
            static const char *type_lab[] = {
                "Data RO",              // 0    Data    Read-Only
                "Data RO Ac",           // 1    Data    Read-Only, accessed
                "Data RW",              // 2    Data    Read/Write
                "Data RW Ac",           // 3    Data    Read/Write, accessed
                "Data RO Ex",           // 4    Data    Read-Only, expand-down
                "Data RO Ex Ac",        // 5    Data    Read-Only, expand-down, accessed
                "Data RW Ex",           // 6    Data    Read/Write, expand-down
                "Data RW Ex Ac",        // 7    Data    Read/Write, expand-down, accessed
                "Code EO",              // 8    Code    Execute-Only
                "Code EO Ac",           // 9    Code    Execute-Only, accessed
                "Code ER",              // 10   Code    Execute/Read
                "Code ER Ac",           // 11   Code    Execute/Read, accessed
                "Code EO Con",          // 12   Code    Execute-Only, conforming
                "Code EO Con Ac",       // 13   Code    Execute-Only, conforming, accessed
                "Code ER Con",          // 14   Code    Execute/Read, conforming
                "Code ER Con Ac",       // 15   Code    Execute/Read, conforming, accessed
            };
            // 判断是存储段还是系统段
            if(seg_des_ptr->SegDesHigh.s == 1) {
                seg_type = type_lab[seg_des_ptr->SegDesHigh.type];
            } else {
                seg_type = "Sys Segment";
            }

            // 执行添加操作
            labels << seg_type;
            emit tableview_append(labels);
        }
    }
}

unsigned MyHunter::get_idt(void *arg)
{
    MyHunter *my_hunter = (MyHunter *)arg;
    DriverManager *driver_manager = my_hunter->driver_manager_; // 驱动管理器

    int core_count = my_hunter->cpu_core_count_; // cpu核心数量
    DWORD mark = 0;     // cpu核心的关联掩码

    for (int i = 0; i < core_count; i++) {
        // 设置关联掩码
        mark = (1 << i);

        // 向驱动发送控制信息
        DWORD ret_bytes = 0;
        BYTE buf[0x800] = { 0 };
        // 获取IDT，输入为限定核心掩码
        driver_manager->io_control(CODE_GET_IDT, &mark, sizeof(mark), buf, 0x800, &ret_bytes);

        // 处理IDT数据
        if(ret_bytes != 0) {
            my_hunter->deal_idt_data(buf, ret_bytes / sizeof(GateDescriptor), i);
        }
    }


    return 0;
}

void MyHunter::deal_idt_data(BYTE *buf, DWORD item_count, DWORD cpu_core)
{
    QStringList labels;
    QString core_num = QString("%1").arg((uint)cpu_core);   // 获取cpu核心编号

    // 遍历IDT
    GateDescriptor *gate_descriptor = (GateDescriptor *)buf;
    for(DWORD i = 0; i < item_count; i++, gate_descriptor++) {
        // 判断门描述符是否存在，判断是否是系统段
        if(gate_descriptor->GateDesHigh.p == 1 && gate_descriptor->GateDesHigh.s == 0) {
            labels.clear();

            // 添加cpu核心编号
            labels << core_num;

            // 获取门序号
            labels << QString("%1").arg((uint)i, 2, 16, QLatin1Char('0')).toUpper();

            // 获取选择子
            USHORT tmp = gate_descriptor->GateDesLow.seg_sel;
            SegmentSelector *seg_sel = (SegmentSelector *)&tmp;
            labels << "0x" + QString("%1").arg((uint)seg_sel->index, 2, 16, QLatin1Char('0')).toUpper();

            // 获取函数地址
            DWORD offset_0_15 = gate_descriptor->GateDesLow.offset_0_15;
            DWORD offset_16_31 = gate_descriptor->GateDesHigh.offset_16_31;
            offset_16_31 <<= 16;
            DWORD offset = offset_0_15 | offset_16_31;
            labels << "0x" + QString("%1").arg((uint)offset, 8, 16, QLatin1Char('0')).toUpper();

            // 执行添加操作
            emit tableview_append(labels);
        }
    }
}

unsigned MyHunter::get_ssdt(void *arg)
{
    MyHunter *my_hunter = (MyHunter *)arg;
    DriverManager *driver_manager = my_hunter->driver_manager_; // 驱动管理器

    // 向驱动发送控制信息
    DWORD ret_bytes = 0;
    ULONG count = 0;

    // 获取SSDT项目个数
    driver_manager->io_control(CODE_SSDT_COUNT, NULL, 0, &count, sizeof(count), &ret_bytes);
    if(ret_bytes) {
        // 分配空间
        SSDTItem *ssdt_items = new SSDTItem[count];

        // 获取SSDT
        driver_manager->io_control(CODE_SSDT, NULL, 0, ssdt_items, sizeof(SSDTItem) * count, &ret_bytes);
        if(ret_bytes) {
            // 处理数据
            for(ULONG i = 0; i < ret_bytes / sizeof(SSDTItem); i++) {
                QStringList labels;
                labels << QString::number(ssdt_items[i].num);
                labels << "0x" + QString("%1").arg((ulong)ssdt_items[i].addr, 8, 16, QLatin1Char('0')).toUpper();
                emit my_hunter->tableview_append(labels);
            }
        }

        delete[] ssdt_items;
    }
    return 0;
}

unsigned MyHunter::get_shadow_ssdt(void *arg)
{
    MyHunter *my_hunter = (MyHunter *)arg;
    DriverManager *driver_manager = my_hunter->driver_manager_; // 驱动管理器

    // 向驱动发送控制信息
    DWORD ret_bytes = 0;
    ULONG count = 0;

    // 获取ShadowSSDT项目个数
    driver_manager->io_control(CODE_SHADOWSSDT_COUNT, NULL, 0, &count, sizeof(count), &ret_bytes);
    if (ret_bytes) {
        // 分配空间
        ShadowSSDTItem *shadowssdt_items = new ShadowSSDTItem[count];

        // 获取ShadowSSDT
        driver_manager->io_control(CODE_SHADOWSSDT, NULL, 0, shadowssdt_items, sizeof(ShadowSSDTItem) * count, &ret_bytes);
        if (ret_bytes) {
            // 处理数据
            for (ULONG i = 0; i < ret_bytes / sizeof(ShadowSSDTItem); i++) {
                QStringList labels;
                labels << QString::number(shadowssdt_items[i].num);
                labels << "0x" + QString("%1").arg((ulong)shadowssdt_items[i].addr, 8, 16, QLatin1Char('0')).toUpper();
                emit my_hunter->tableview_append(labels);
            }
        }

        delete[] shadowssdt_items;
    }
    return 0;
}

unsigned MyHunter::get_driver_module(void *arg)
{
    MyHunter *my_hunter = (MyHunter *)arg;
    DriverManager *driver_manager = my_hunter->driver_manager_; // 驱动管理器

    // 向驱动发送控制信息
    DWORD ret_bytes = 0;
    ULONG count = 0;

    // 获取驱动模块个数
    driver_manager->io_control(CODE_DRIVER_MODULE_COUNT, NULL, 0, &count, sizeof(count), &ret_bytes);
    if(ret_bytes) {
        // 分配空间
        DriverModule *driver_modules = new DriverModule[count];
        DWORD buf_size = sizeof(DriverModule) * count;
        memset(driver_modules, 0, buf_size);

        // 获取驱动模块
        driver_manager->io_control(CODE_DRIVER_MODULE, NULL, 0, driver_modules, buf_size, &ret_bytes);
        if(ret_bytes) {
            // 处理数据
            for(DWORD i = 0; i < ret_bytes / sizeof(DriverModule); i++) {
                QStringList labels;
                // 驱动名
                labels << TCHARToQString(driver_modules[i].name);
                // 基址
                labels << "0x" + QString("%1").arg((uint)driver_modules[i].base, 8, 16, QLatin1Char('0')).toUpper();
                // 大小
                labels << "0x" + QString("%1").arg((uint)driver_modules[i].size, 8, 16, QLatin1Char('0')).toUpper();
                // 驱动路径
                labels << TCHARToQString(driver_modules[i].path);
                emit my_hunter->tableview_append(labels);
            }
        }
        delete[] driver_modules;
    }

    return 0;
}

unsigned MyHunter::get_process_list(void *arg)
{
    MyHunter *my_hunter = (MyHunter *)arg;
    DriverManager *driver_manager = my_hunter->driver_manager_; // 驱动管理器
    QStringList labels;

    // 遍历进程
    HANDLE process_snap;
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);
    process_snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (process_snap == INVALID_HANDLE_VALUE) {
        return 0;
    }

    if (!Process32First(process_snap, &pe32)) {
        return 0;
    }

    // 获取进程信息
    do {
        labels.clear();

        // 判断PID是否为0
        if(pe32.th32ProcessID == 0)
            continue;

        labels << TCHARToQString(pe32.szExeFile);                       // 获取进程名
        labels << QString("%1").arg((uint)pe32.th32ProcessID);          // 获取PID
        labels << QString("%1").arg((uint)pe32.th32ParentProcessID);    // 获取Parent PID

        ULONG ret_bytes = 0;

        // 获取映像路径
        TCHAR image_path[1024] = { 0 };
        driver_manager->io_control(CODE_GET_IMAGE_PATH, &pe32.th32ProcessID, sizeof(pe32.th32ProcessID), image_path, 1024, &ret_bytes);
        if (ret_bytes) {
            // 从r0获取到数据
            labels << TCHARToQString(image_path);
        } else {
            // 没有从r0获取到数据
            QString module_name = TCHARToQString(pe32.szExeFile);;
            // 获取第一个模块
            HANDLE module_snap = INVALID_HANDLE_VALUE;
            MODULEENTRY32 me32;
            me32.dwSize = sizeof(MODULEENTRY32);
            module_snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pe32.th32ProcessID);
            if (module_snap != INVALID_HANDLE_VALUE) {
                if(Module32First(module_snap, &me32) && me32.th32ProcessID == pe32.th32ProcessID) {
                    module_name = TCHARToQString(me32.szExePath);
                }
                ::CloseHandle(module_snap);
            }
            labels << module_name;
        }
            
        // 获取EPROCESS
        ULONG eprocess = 0;
        ret_bytes = 0;
        driver_manager->io_control(CODE_GET_EPROCESS, &pe32.th32ProcessID, sizeof(pe32.th32ProcessID), 
            &eprocess, sizeof(eprocess), &ret_bytes);
        if (ret_bytes != 0)
            labels << "0x" + QString("%1").arg((ulong)eprocess, 8, 16, QLatin1Char('0')).toUpper();
        else
            labels << "Unknown";

        // 获取特权级
        labels << "-";

        // 执行添加操作
        emit my_hunter->tableview_append(labels);
    } while (Process32Next(process_snap, &pe32));
    CloseHandle(process_snap);

    return 0;
}

unsigned MyHunter::get_process_module_list(void *arg)
{
    MyHunter *my_hunter = (MyHunter *)arg;
    DriverManager *driver_manager = my_hunter->driver_manager_; // 驱动管理器

    QStringList labels;

    DWORD module_count = 0, ret_bytes = 0;

    // 获取模块列表元素个数
    driver_manager->io_control(CODE_GET_PROCESS_MODULE_COUNT, &my_hunter->param_pid, sizeof(my_hunter->param_pid), 
        &module_count, sizeof(module_count), &ret_bytes);

    if(ret_bytes) {
        // 分配缓冲区空间
        ModuleItem *module_items = new ModuleItem[module_count];
        memset(module_items, 0, sizeof(ModuleItem) * module_count);
        // 获取进程模块列表
        driver_manager->io_control(CODE_GET_PROCESS_MODULE, &my_hunter->param_pid, sizeof(my_hunter->param_pid),
            module_items, sizeof(ModuleItem) * module_count, &ret_bytes);
        if(ret_bytes) {
            for(DWORD i = 0; i < ret_bytes / sizeof(ModuleItem); i++) {
                labels.clear();
                auto &item = module_items[i];
                labels << TCHARToQString(item.path);    // 路径
                labels << "0x" + QString("%1").arg((uint)item.base, 8, 16, QLatin1Char('0'));   // 基址
                labels << "0x" + QString("%1").arg((uint)item.size, 8, 16, QLatin1Char('0'));   // 大小

                // 添加信息
                emit my_hunter->child_tableview_append(labels);
            }
        }
        // 释放缓冲区
        delete[] module_items;
    }

    // 让子窗口设置模型
    emit my_hunter->child_wnd_set_modle();

    return 0;
}

unsigned MyHunter::get_process_thread_list(void *arg)
{
    MyHunter *my_hunter = (MyHunter *)arg;
    DriverManager *driver_manager = my_hunter->driver_manager_; // 驱动管理器

    DWORD thread_count = 0, ret_bytes = 0;

    // 获取进程的线程个数
    driver_manager->io_control(CODE_GET_PROCESS_THREAD_COUNT, &my_hunter->param_pid, sizeof(my_hunter->param_pid),
        &thread_count, sizeof(thread_count), &ret_bytes);

    if(ret_bytes) {
        // 分配缓冲区空间
        ThreadItem *thread_items = new ThreadItem[thread_count];

        // 获取指定进程的线程
        driver_manager->io_control(CODE_GET_PROCESS_THREAD, &my_hunter->param_pid, sizeof(my_hunter->param_pid),
            thread_items, thread_count * sizeof(ThreadItem), &ret_bytes);

        if(ret_bytes) {
            for(DWORD i = 0; i < ret_bytes / sizeof(ThreadItem); i++) {
                QStringList labels;
                // 获取pid
                labels << QString::number(thread_items[i].pid);
                // 获取ETHREAD
                labels << "0x" + QString("%1").arg((uint)thread_items[i].ethread, 8, 16, QLatin1Char('0')).toUpper();
                // 获取teb
                labels << "0x" + QString("%1").arg((uint)thread_items[i].teb, 8, 16, QLatin1Char('0')).toUpper();

                // 添加信息
                emit my_hunter->child_tableview_append(labels);
            }
        }

        // 释放缓冲区
        delete[] thread_items;
    }

    // 让子窗口设置模型
    emit my_hunter->child_wnd_set_modle();

    return 0;
}

unsigned MyHunter::get_process_handle_list(void *arg)
{
    return 0;
}

DWORD MyHunter::get_segdes_base(SegmentDescriptor *segment_descriptor)
{
    // 拼接字段
    DWORD base_0_15 = segment_descriptor->SegDesLow.base_0_15;
    DWORD base_16_23 = segment_descriptor->SegDesHigh.base_16_23;
    DWORD base_24_31 = segment_descriptor->SegDesHigh.base_24_31;
    DWORD base = 0;
    base_24_31 <<= 24;
    base_16_23 <<= 16;
    base = base_0_15 | base_16_23 | base_24_31;
    return base;
}

DWORD MyHunter::get_segdes_limit(SegmentDescriptor *segment_descriptor, bool is_comput)
{
    // 拼接字段
    DWORD limit_0_15 = segment_descriptor->SegDesLow.limit_0_15;
    DWORD limit_16_19 = segment_descriptor->SegDesHigh.limit_16_19;
    limit_16_19 <<= 16;
    DWORD limit = 0;
    limit = limit_0_15 | limit_16_19;

    // 是否直接计算
    if(is_comput) {
        // 判断粒度单位
        if (segment_descriptor->SegDesHigh.g == 1) {
            // 以4k为单位
            limit = limit * 4096;
        }
        limit += 0xfff;
    }
    
    return limit;
}
