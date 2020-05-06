#pragma once

#include <ntifs.h>
#include <Ntddk.h>
#include "../../common/protocol.h"


// 入口函数
DRIVER_INITIALIZE DriverEntry;

// 卸载函数
DRIVER_UNLOAD DriverUnload;

// 创建
NTSTATUS
MyDispatchCreate(
    _In_ struct _DEVICE_OBJECT *DeviceObject,
    _Inout_ struct _IRP *Irp
);

// 控制
NTSTATUS
MyDispatchControl(
    _In_ struct _DEVICE_OBJECT *DeviceObject,
    _Inout_ struct _IRP *Irp
);

// 关闭
NTSTATUS
MyDispatchClose(
    _In_ struct _DEVICE_OBJECT *DeviceObject,
    _Inout_ struct _IRP *Irp
);


// 将函数放入分页内存中
#pragma alloc_text("INIT", DriverEntry)
#pragma alloc_text("PAGE", DriverUnload)
#pragma alloc_text("PAGE", MyDispatchCreate)
#pragma alloc_text("PAGE", MyDispatchControl)
#pragma alloc_text("PAGE", MyDispatchClose)


// 未公开函数指针声明
typedef NTSTATUS(__stdcall *pfnZwQueryInformationProcess)(
    _In_      HANDLE           ProcessHandle,
    _In_      PROCESSINFOCLASS ProcessInformationClass,
    _Out_     PVOID            ProcessInformation,
    _In_      ULONG            ProcessInformationLength,
    _Out_opt_ PULONG           ReturnLength
    );


// 功能函数

// 还原指定cpu核心运行（全部核心都可运行）
void restore_core_working();

// 获取_EPROCESS.ActiveProcessLinks的偏移
ULONG get_activeprocesslinks_offset();

// 通过pid查找进程
PEPROCESS find_process_by_id(ULONG pid);

// 获取指定进程的CR3
ULONG get_process_cr3(ULONG pid);

/*
 * 获取GDT缓冲区大小
 *      core_mark: 指定cpu核心关联掩码
 *      output: 输出缓冲区
 *  返回值: IoStatus的Information
 */
ULONG_PTR get_gdt_buf_size(DWORD32 core_mark, DWORD32 *output);

/*
 * 获取GDT数据
 *      core_mark: 指定cpu核心关联掩码
 *      output: 输出缓冲区
 *      output_size: 输出缓冲区大小
 *  返回值: IoStatus的Information
 */
ULONG_PTR get_gdt(DWORD32 core_mark, PVOID output, ULONG output_size);

/*
 * 获取IDT数据
 *      core_mark: 指定cpu核心关联掩码
 *      output: 输出缓冲区
 *      output_size: 输出缓冲区大小
 *  返回值: IoStatus的Information
 */
ULONG_PTR get_idt(DWORD32 core_mark, PVOID output, ULONG output_size);

/*
 * 获取指定进程的EPROCESS
 *      pid: 进程id
 *      output: 输出缓冲区
 *      output_size: 输出缓冲区大小
 *  返回值: IoStatus的Information
 */
ULONG_PTR get_eprocess(ULONG pid, PULONG output, ULONG output_size);

/*
 * 获取指定进程的映像路径
 *      pid: 进程id
 *      output: 输出缓冲区
 *      output_size: 输出缓冲区大小
 *  返回值: IoStatus的Information
 */
ULONG_PTR get_image_path(ULONG pid, PVOID output, ULONG output_size);

/*
 * 读取指定进程的内存
 *      pid: 进程id
 *      mem_addr: 内存地址
 *      read_buf: 缓冲区
 *      buf_size: 缓冲区大小
 *  返回值: IoStatus的Information
 */
ULONG_PTR read_process_memory(ULONG pid, PVOID mem_addr, PVOID read_buf, ULONG buf_size);

/*
 * 写入指定进程的内存
 *      pid: 进程id
 *      mem_addr: 内存地址
 *      write_buf: 缓冲区
 *      buf_size: 缓冲区大小
 *  返回值: IoStatus的Information
 */
ULONG_PTR write_process_memory(ULONG pid, PVOID mem_addr, PVOID write_buf, ULONG buf_size);

/*
 * 获取指定进程的模块个数
 *      pid: 进程id
 *      output: 输出缓冲区
 *      output_size: 输出缓冲区大小
 * 返回值: IoStatus的Information
 */
ULONG_PTR get_process_module_count(ULONG pid, PULONG output);

/*
 * 获取指定进程的模块
 *      pid: 进程id
 *      output: 输出缓冲区
 *      output_size: 输出缓冲区大小
 * 返回值: IoStatus的Information
 */
ULONG_PTR get_process_module(ULONG pid, PVOID output, ULONG output_size);

/*
 * 获取指定进程的线程个数
 *      pid: 进程id
 *      output: 输出缓冲区
 *      output_size: 输出缓冲区大小
 * 返回值: IoStatus的Information
 */
ULONG_PTR get_process_thread_count(ULONG pid, PULONG output);

/*
 * 获取指定进程的线程
 *      pid: 进程id
 *      output: 输出缓冲区
 *      output_size: 输出缓冲区大小
 * 返回值: IoStatus的Information
 */
ULONG_PTR get_process_thread(ULONG pid, PVOID output, ULONG output_size);

/*
 * 获取SSDT个数
 *      output: 输出缓冲区
 *      output_size: 输出缓冲区大小
 * 返回值: IoStatus的Information
 */
extern ULONG KeServiceDescriptorTable;
ULONG_PTR get_ssdt_count(PULONG output, ULONG output_size);

/*
 * 获取SSDT
 *      output: 输出缓冲区
 *      output_size: 输出缓冲区大小
 * 返回值: IoStatus的Information
 */
ULONG_PTR get_ssdt(PVOID output, ULONG output_size);

/*
 * 获取ShadowSSDT个数
 *      output: 输出缓冲区
 *      output_size: 输出缓冲区大小
 * 返回值: IoStatus的Information
 */
ULONG_PTR get_shadowssdt_count(PULONG output, ULONG output_size);

/*
 * 获取ShadowSSDT
 *      output: 输出缓冲区
 *      output_size: 输出缓冲区大小
 * 返回值: IoStatus的Information
 */
ULONG_PTR get_shadowssdt(PVOID output, ULONG output_size);

/*
 * 没导出的LDR_DATA_TABLE_ENTRY结构体声明
 */
typedef struct _LDR_DATA_TABLE_ENTRY {
    LIST_ENTRY InLoadOrderLinks;
    LIST_ENTRY InMemoryOrderLinks;
    LIST_ENTRY InInitializationOrderLinks;
    PVOID DllBase;
    PVOID EntryPoint;
    ULONG SizeOfImage;
    UNICODE_STRING FullDllName;
    UNICODE_STRING BaseDllName;
    ULONG Flags;
    USHORT LoadCount;
    USHORT TlsIndex;
    union {
        LIST_ENTRY HashLinks;
        struct {
            PVOID SectionPointer;
            ULONG CheckSum;
        };
    };
    union {
        struct {
            ULONG TimeDateStamp;
        };

        struct {
            PVOID LoadedImports;
        };
    };
} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;

/*
 * 获取驱动模块个数
 *      pDriverObject: 驱动对象
 *      output: 输出缓冲区
 * 返回值: IoStatus的Information
 */
ULONG_PTR get_driver_module_count(PDRIVER_OBJECT pDriverObject, PULONG output);

/*
 * 获取驱动模块个数
 *      pDriverObject: 驱动对象
 *      output: 输出缓冲区
 * 返回值: IoStatus的Information
 */
ULONG_PTR get_driver_module(PDRIVER_OBJECT pDriverObject, PVOID output, ULONG output_size);