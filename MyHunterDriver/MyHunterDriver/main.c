#include "MyDriver.h"


// 入口函数
_Use_decl_annotations_
NTSTATUS
DriverEntry(struct _DRIVER_OBJECT* DriverObject, PUNICODE_STRING  RegistryPath)
{
    KdBreakPoint();
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Hello Kernel! - Install\n");

    // 注册卸载函数
    DriverObject->DriverUnload = DriverUnload;

    // 注册派遣函数
    DriverObject->MajorFunction[IRP_MJ_CREATE] = MyDispatchCreate;              // 创建
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = MyDispatchControl;     // 控制
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = MyDispatchClose;                // 关闭

    // 设置IO模式
    DriverObject->Flags |= DO_BUFFERED_IO;   // 复制缓冲区，在R0中开辟空间，复制缓冲区内容到此处


    NTSTATUS status;    // 返回状态

    // 创建设备对象，一般为IO管理器管理，内核设备FILE_DEVICE_UNKNOWN
    UNICODE_STRING driver_name;
    RtlInitUnicodeString(&driver_name, MY_DEVICE_NAME);
    PDEVICE_OBJECT device_object_ptr = NULL;
    status = IoCreateDevice(DriverObject, 0, &driver_name, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &device_object_ptr);

    if (!NT_SUCCESS(status)) {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "IoCreateDevice Error: %p\n", status);
        return status;
    }

    // 创建符号连接
    UNICODE_STRING symbo_name;
    RtlInitUnicodeString(&symbo_name, MY_SYMBOL_NAME);
    status = IoCreateSymbolicLink(&symbo_name, &driver_name);

    if (!NT_SUCCESS(status)) {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "IoCreateSymbolicLink Error: %p\n", status);
        return status;
    }

    return STATUS_SUCCESS;
}


// 卸载驱动
void DriverUnload(struct _DRIVER_OBJECT* DriverObject)
{
    // 删除设备
    IoDeleteDevice(DriverObject->DeviceObject);     // 这是个链表，多个设备对象时就需要遍历了

    // 删除符号链接，如果有的话
    UNICODE_STRING symbo_name;
    RtlInitUnicodeString(&symbo_name, MY_SYMBOL_NAME);
    IoDeleteSymbolicLink(&symbo_name);

    KdPrint(("Hello Kernel! - UnInstall\n"));
}


// 创建
NTSTATUS
MyDispatchCreate(_In_ struct _DEVICE_OBJECT *DeviceObject, _Inout_ struct _IRP *Irp)
{
    // 完成请求
    Irp->IoStatus.Information = 0;
    Irp->IoStatus.Status = STATUS_SUCCESS;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return STATUS_SUCCESS;
}

// 控制
NTSTATUS
MyDispatchControl(_In_ struct _DEVICE_OBJECT *DeviceObject, _Inout_ struct _IRP *Irp)
{
    KdPrint(("Dispatch: [MyDispatchControl]\n"));

    NTSTATUS Status = STATUS_SUCCESS;
    ULONG_PTR Information = 0;

    PIO_STACK_LOCATION stack_location = IoGetCurrentIrpStackLocation(Irp);                  // 获取Irp堆栈
    ULONG input_length = stack_location->Parameters.DeviceIoControl.InputBufferLength;      // 获取输入缓冲区长度
    ULONG output_length = stack_location->Parameters.DeviceIoControl.OutputBufferLength;    // 获取输出缓冲区长度
    ULONG control_code = stack_location->Parameters.DeviceIoControl.IoControlCode;          // 控制码
    PVOID systembuff = Irp->AssociatedIrp.SystemBuffer;                                     // 内核缓冲区


    // 具体工作任务
    switch (control_code) {
    case CODE_GET_GDT_BUFSIZE:                  // 获取GDT缓冲区大小
        Information = get_gdt_buf_size(*(DWORD32 *)systembuff, systembuff);
        break;
    case CODE_GET_GDT:                          // 获取GDT
        Information = get_gdt(*(DWORD32 *)systembuff, systembuff, output_length);
        break;
    case CODE_GET_EPROCESS:                     // 获取指定进程的EPROCESS
        Information = get_eprocess(*(ULONG *)systembuff, systembuff, output_length);
        break;
    case CODE_GET_IMAGE_PATH:                   // 获取指定进程的映像路径
        Information = get_image_path(*(ULONG *)systembuff, systembuff, output_length);
        break;
    case CODE_GET_IDT:                          // 获取IDT
        Information = get_idt(*(DWORD32 *)systembuff, systembuff, output_length);
        break;
    case CODE_GET_PROCESS_MODULE_COUNT:         // 获取指定进程模块个数
        Information = get_process_module_count(*(ULONG *)systembuff, systembuff);
        break;
    case CODE_GET_PROCESS_MODULE:               // 获取指定进程模块
        Information = get_process_module(*(ULONG *)systembuff, systembuff, output_length);
        break;
    case CODE_GET_PROCESS_THREAD_COUNT:         // 获取指定进程线程个数
        Information = get_process_thread_count(*(ULONG *)systembuff, systembuff);
        break;
    case CODE_GET_PROCESS_THREAD:               // 获取指定进程线程
        Information = get_process_thread(*(ULONG *)systembuff, systembuff, output_length);
        break;
    case CODE_SSDT_COUNT:                       // 获取SSDT个数
        Information = get_ssdt_count((PULONG)systembuff, output_length);
        break;
    case CODE_SSDT:                             // 获取SSDT
        Information = get_ssdt((PULONG)systembuff, output_length);
        break;
    case CODE_SHADOWSSDT_COUNT:                 // 获取SSDT个数
        Information = get_shadowssdt_count((PULONG)systembuff, output_length);
        break;
    case CODE_SHADOWSSDT:                       // 获取SSDT
        Information = get_shadowssdt((PULONG)systembuff, output_length);
        break;
    case CODE_DRIVER_MODULE_COUNT:
        Information = get_driver_module_count(DeviceObject->DriverObject, (PULONG)systembuff);
        break;
    case CODE_DRIVER_MODULE:
        Information = get_driver_module(DeviceObject->DriverObject, systembuff, output_length);
        break;
    default:
        break;
    }

    // 完成请求
    Irp->IoStatus.Information = Information;
    Irp->IoStatus.Status = Status;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return STATUS_SUCCESS;
}

// 关闭
NTSTATUS
MyDispatchClose(_In_ struct _DEVICE_OBJECT *DeviceObject, _Inout_ struct _IRP *Irp)
{
    // 完成请求
    Irp->IoStatus.Information = 0;
    Irp->IoStatus.Status = STATUS_SUCCESS;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return STATUS_SUCCESS;
}

void restore_core_working()
{
    // 获取CPU核心数量
    ULONG core_count = KeQueryActiveProcessorCount(NULL);

    // 生成关联掩码
    KAFFINITY mark = 0;
    for (ULONG i = 0; i < core_count; i++) {
        mark |= (1 << i);
    }

    // 设置关联掩码指定的cpu核心运行
    KeSetSystemAffinityThread(mark);
}

ULONG get_activeprocesslinks_offset()
{
    NTSTATUS status = STATUS_SUCCESS;

    // 获取系统版本
    ULONG offset = 0;
    RTL_OSVERSIONINFOW os_info = { 0 };
    status = RtlGetVersion(&os_info);
    if (!NT_SUCCESS(status)) {
        return offset;
    }

    // 判断系统版本
    switch (os_info.dwMajorVersion) {
    case 6:
        switch (os_info.dwMinorVersion) {
        case 1:             // win7
#ifdef _WIN64
            offset = 0x188;
#else
            offset = 0xb8;
#endif
            break;
        }
        break;
    case 10:                // win10
#ifdef _WIN64
        offset = 0x2f0;
#else
        offset = 0xb8;
#endif
        break;
    }
    return offset;
}

PEPROCESS find_process_by_id(ULONG pid)
{
    // 获取ActiveProcessLinks字段的偏移
    ULONG offset = get_activeprocesslinks_offset();
    if (offset == 0) {
        return NULL;
    }

    // 遍历进程
    PEPROCESS first_eprocess = NULL, traverse_eprocess = NULL;
    first_eprocess = PsGetCurrentProcess();
    traverse_eprocess = first_eprocess;
    UINT8 found = FALSE;

    do {
        // 从EPROCESS中获取PID
        ULONG process_id = (ULONG)PsGetProcessId(traverse_eprocess);
        if (process_id == pid) {
            // 找到
            found = TRUE;
            break;
        }

        // 根据偏移计算下一个EPROCESS
        traverse_eprocess = (PEPROCESS)((PUCHAR)(((PLIST_ENTRY)((PUCHAR)traverse_eprocess + offset))->Flink) - offset);
    } while (traverse_eprocess != first_eprocess);

    if (!found) {
        return NULL;
    }

    return traverse_eprocess;
}

ULONG get_process_cr3(ULONG pid)
{
    ULONG reg_cr3 = 0;
    PEPROCESS eprocess = find_process_by_id(pid);
    if (eprocess == NULL)
        return 0;

    KAPC_STATE kapc_state;
    // 附加到进程
    KeStackAttachProcess(eprocess, &kapc_state);

    // 获取cr3
    __asm {
        cli
        mov eax, cr3;
        mov reg_cr3, eax
            sti
    }

    // 分离进程
    KeUnstackDetachProcess(&kapc_state);
    return reg_cr3;
}

ULONG_PTR get_gdt_buf_size(DWORD32 core_mark, DWORD32 *output)
{
    // 设置关联掩码指定的cpu核心运行
    KeSetSystemAffinityThread(core_mark);

    // 获取GDTR
    struct GDTR gdtr;
    RtlSecureZeroMemory(&gdtr, sizeof(struct GDTR));
    __asm {
        sgdt gdtr;
    }

    // 获取gdt的大小
    *output = gdtr.limit + 1;

    // 还原运行核心
    restore_core_working();

    return sizeof(DWORD32);
}

ULONG_PTR get_gdt(DWORD32 core_mark, PVOID output, ULONG output_size)
{
    // 设置关联掩码指定的cpu核心运行
    KeSetSystemAffinityThread(core_mark);

    // 进入临界区
    KeEnterCriticalRegion();

    // 获取GDTR
    struct GDTR gdtr;
    RtlSecureZeroMemory(&gdtr, sizeof(struct GDTR));
    __asm {
        sgdt gdtr;
    }

    // 判断拷贝数据的大小
    ULONG gdt_limit = gdtr.limit + 1;
    ULONG copy_size = output_size > gdt_limit ? gdt_limit : output_size;

    // 判断内存地址是否有效
    if (!MmIsAddressValid((PVOID)gdtr.address)) {
        // 出临界区
        KeLeaveCriticalRegion();
        // 还原运行核心
        restore_core_working();
        return 0;
    }

    // 拷贝内存数据
    RtlCopyMemory(output, (PVOID)gdtr.address, copy_size);

    // 出临界区
    KeLeaveCriticalRegion();

    // 还原运行核心
    restore_core_working();

    return copy_size;
}

ULONG_PTR get_idt(DWORD32 core_mark, PVOID output, ULONG output_size)
{
    //KdBreakPoint();
    // 设置关联掩码指定的cpu核心运行
    KeSetSystemAffinityThread(core_mark);

    // 进入临界区
    KeEnterCriticalRegion();

    // 获取IDTR
    struct IDTR idtr;
    __asm {
        sidt idtr
    }

    // 判断拷贝数据的大小
    ULONG idt_limit = idtr.limit + 1;
    ULONG copy_size = output_size > idt_limit ? idt_limit : output_size;

    // 判断内存地址是否有效
    if (!MmIsAddressValid((PVOID)idtr.address)) {
        // 出临界区
        KeLeaveCriticalRegion();
        // 还原运行核心
        restore_core_working();
        return 0;

    }

    // 拷贝内存数据
    RtlCopyMemory(output, (PVOID)idtr.address, copy_size);

    // 出临界区
    KeLeaveCriticalRegion();

    // 还原运行核心
    restore_core_working();
    return copy_size;
}

ULONG_PTR get_eprocess(ULONG pid, PULONG output, ULONG output_size)
{
    // 遍历查找EPROCESS
    PEPROCESS eprocess = find_process_by_id(pid);
    if (eprocess == NULL)
        return 0;
    *output = (ULONG)eprocess;
    return output_size;
}

ULONG_PTR get_image_path(ULONG pid, PVOID output, ULONG output_size)
{
    // 遍历查找EPROCESS
    PEPROCESS eprocess = find_process_by_id(pid);

    KAPC_STATE kapc_state;
    // 附加到进程
    KeStackAttachProcess(eprocess, &kapc_state);

    // 获取PEB
    PPEB peb_ptr = (PPEB)(*(PUINT_PTR)((PUCHAR)eprocess + 0x1a8));
    if (peb_ptr == NULL) {
        // 分离进程
        KeUnstackDetachProcess(&kapc_state);
        return 0;
    }

    // 获取PROCESS_PARAM
    PUINT_PTR user_proc_param = (PUINT_PTR)(*(PUINT_PTR)((PCHAR)peb_ptr + 0x010));
    if (user_proc_param == NULL) {
        // 分离进程
        KeUnstackDetachProcess(&kapc_state);
        return 0;
    }

    // 获取映像路径
    PUNICODE_STRING image_path = (PUNICODE_STRING)((PUCHAR)user_proc_param + 0x038);
    if (!MmIsAddressValid(image_path) || !MmIsAddressValid(image_path->Buffer)) {
        // 分离进程
        KeUnstackDetachProcess(&kapc_state);
        return 0;
    }

    // 计算缓冲区大小
    output_size = output_size > image_path->Length ? image_path->Length : output_size;

    // 进入临界区
    KeEnterCriticalRegion();

    RtlCopyMemory(output, image_path->Buffer, output_size);

    // 出临界区
    KeLeaveCriticalRegion();

    // 分离进程
    KeUnstackDetachProcess(&kapc_state);

    return output_size;
}

ULONG_PTR read_process_memory(ULONG pid, PVOID mem_addr, PVOID read_buf, ULONG buf_size)
{
    // 获取指定进程的EPROCESS
    PEPROCESS eprocess = find_process_by_id(pid);
    // 找到目录表基址
    PVOID old_cr3 = 0;
    PVOID dir_base = (PVOID)(*(PUINT_PTR)((PUCHAR)eprocess + 0x18));

    __asm {
        // 屏蔽中断，禁止线程切换
        cli

        // 保存原来的cr3
        mov eax, cr3
        mov old_cr3, eax

        // 修改cr3
        mov eax, dir_base
        mov cr3, eax
    }

    // 检查内存是否有效
    if (MmIsAddressValid(mem_addr)) {
        // 计算大小，不允许跨分页
        ULONG size = ((ULONG)mem_addr / PAGE_SIZE + 1) * PAGE_SIZE - (ULONG)mem_addr;
        buf_size = buf_size > size ? size : buf_size;
        RtlCopyMemory(read_buf, mem_addr, buf_size);
    }

    __asm {
        // 还原cr3
        mov eax, old_cr3
        mov cr3, eax

        // 恢复中断
        sti
    }

    return buf_size;
}

ULONG_PTR write_process_memory(ULONG pid, PVOID mem_addr, PVOID write_buf, ULONG buf_size)
{
    // 获取指定进程的EPROCESS
    PEPROCESS eprocess = find_process_by_id(pid);
    // 找到目录表基址
    PVOID old_cr3 = 0;
    PVOID dir_base = (PVOID)(*(PUINT_PTR)((PUCHAR)eprocess + 0x18));

    __asm {
        // 屏蔽中断，禁止线程切换
        cli

        // 关闭内存写保护
        mov eax, cr0
        and eax, not 10000h
        mov cr0, eax

        // 保存原来的cr3
        mov eax, cr3
        mov old_cr3, eax

        // 修改cr3
        mov eax, dir_base
        mov cr3, eax
    }

    // 检查内存是否有效
    if (MmIsAddressValid(mem_addr)) {
        // 计算大小，不允许跨分页
        ULONG size = ((ULONG)mem_addr / PAGE_SIZE + 1) * PAGE_SIZE - (ULONG)mem_addr;
        buf_size = buf_size > size ? size : buf_size;
        RtlCopyMemory(mem_addr, write_buf, buf_size);
    }

    __asm {
        // 还原cr3
        mov eax, old_cr3
        mov cr3, eax

        // 还原内存写保护
        mov eax, cr0
        or eax, 10000h
        mov cr0, eax

        // 恢复中断
        sti
    }

    return buf_size;
}

ULONG_PTR get_process_module_count(ULONG pid, PULONG output)
{
    NTSTATUS status;
    // 获取为公开的API
    pfnZwQueryInformationProcess ZwQueryInformationProcess;
    UNICODE_STRING UtrZwQueryInformationProcessName = RTL_CONSTANT_STRING(L"ZwQueryInformationProcess");
    ZwQueryInformationProcess = (pfnZwQueryInformationProcess)MmGetSystemRoutineAddress(&UtrZwQueryInformationProcessName);

    HANDLE p_handle = 0;
    CLIENT_ID cid = { 0 };
    cid.UniqueProcess = (HANDLE)pid;
    OBJECT_ATTRIBUTES obj;
    InitializeObjectAttributes(&obj, 0, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, 0, 0);
    status = ZwOpenProcess(&p_handle, PROCESS_ALL_ACCESS, &obj, &cid);
    if (!NT_SUCCESS(status)) {
        return 0;
    }

    // 获取base_info信息
    PROCESS_BASIC_INFORMATION base_info;
    ULONG ret_bytes = 0;
    status = ZwQueryInformationProcess(p_handle, ProcessBasicInformation, &base_info, sizeof(base_info), &ret_bytes);
    if (!NT_SUCCESS(status)) {
        return 0;
    }

    KAPC_STATE kapc_state;
    PEPROCESS eprocess = find_process_by_id(pid);
    // 附加到进程
    KeStackAttachProcess(eprocess, &kapc_state);

    // 获取PEB
    PPEB peb_ptr = base_info.PebBaseAddress;
    if (!MmIsAddressValid(peb_ptr)) {
        // 分离进程
        KeUnstackDetachProcess(&kapc_state);
        return 0;
    }



    // 获取_PEB_LDR_DATA
    PULONG_PTR peb_ldr_data_ptr = *(PULONG_PTR *)((PUCHAR)peb_ptr + 0x00c);
    if (!MmIsAddressValid(peb_ldr_data_ptr)) {
        // 分离进程
        KeUnstackDetachProcess(&kapc_state);
        return 0;
    }

    // 获取加载模块链表
    PLIST_ENTRY list_entry_ptr = *(PLIST_ENTRY *)((PUCHAR)peb_ldr_data_ptr + 0x00c);
    if (!MmIsAddressValid(list_entry_ptr)) {
        // 分离进程
        KeUnstackDetachProcess(&kapc_state);
        return 0;
    }

    // 进入临界区
    KeEnterCriticalRegion();

    // 遍历链表
    PLIST_ENTRY ptr = list_entry_ptr;
    ULONG count = 0;

    do {
        // 判断是否是尾节点
        ULONG dll_base = *(PULONG_PTR)((PUCHAR)ptr + 0x018);
        if (dll_base == 0) {
            ptr = ptr->Flink;
            continue;
        }

        count++;
        ptr = ptr->Flink;
    } while (ptr != list_entry_ptr);

    *output = count;

    // 出临界区
    KeLeaveCriticalRegion();

    // 分离进程
    KeUnstackDetachProcess(&kapc_state);

    return sizeof(ULONG);
}

ULONG_PTR get_process_module(ULONG pid, PVOID output, ULONG output_size)
{
    
    NTSTATUS status;
    // 获取为公开的API
    pfnZwQueryInformationProcess ZwQueryInformationProcess;
    UNICODE_STRING UtrZwQueryInformationProcessName = RTL_CONSTANT_STRING(L"ZwQueryInformationProcess");
    ZwQueryInformationProcess = (pfnZwQueryInformationProcess)MmGetSystemRoutineAddress(&UtrZwQueryInformationProcessName);

    HANDLE p_handle = 0;
    CLIENT_ID cid = { 0 };
    cid.UniqueProcess = (HANDLE)pid;
    OBJECT_ATTRIBUTES obj;
    InitializeObjectAttributes(&obj, 0, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, 0, 0);
    status = ZwOpenProcess(&p_handle, PROCESS_ALL_ACCESS, &obj, &cid);
    if (!NT_SUCCESS(status)) {
        return 0;
    }

    // 获取base_info信息
    PROCESS_BASIC_INFORMATION base_info;
    ULONG ret_bytes = 0;
    status = ZwQueryInformationProcess(p_handle, ProcessBasicInformation, &base_info, sizeof(base_info), &ret_bytes);
    if (!NT_SUCCESS(status)) {
        return 0;
    }

    KAPC_STATE kapc_state;
    PEPROCESS eprocess = find_process_by_id(pid);
    // 附加到进程
    KeStackAttachProcess(eprocess, &kapc_state);

    // 获取PEB
    PPEB peb_ptr = base_info.PebBaseAddress;
    if (!MmIsAddressValid(peb_ptr)) {
        // 分离进程
        KeUnstackDetachProcess(&kapc_state);
        return 0;
    }


    // 获取_PEB_LDR_DATA
    PULONG_PTR peb_ldr_data_ptr = *(PULONG_PTR *)((PUCHAR)peb_ptr + 0x00c);
    if (!MmIsAddressValid(peb_ldr_data_ptr)) {
        // 分离进程
        KeUnstackDetachProcess(&kapc_state);
        return 0;
    }

    // 获取加载模块链表
    PLIST_ENTRY list_entry_ptr = *(PLIST_ENTRY *)((PUCHAR)peb_ldr_data_ptr + 0x00c);
    if (!MmIsAddressValid(list_entry_ptr)) {
        // 分离进程
        KeUnstackDetachProcess(&kapc_state);
        return 0;
    }

    // 进入临界区
    KeEnterCriticalRegion();

    // 遍历链表
    PLIST_ENTRY ptr = list_entry_ptr;
    ULONG copy_size = 0;

    struct ModuleItem *module_ptr = (struct ModuleItem *)output;
    do {
        // 判断是否是尾节点
        ULONG dll_base = *(PULONG_PTR)((PUCHAR)ptr + 0x018);
        if (dll_base == 0) {
            ptr = ptr->Flink;
            continue;
        }

        // 判断是否读满整个缓冲区
        if (copy_size >= output_size)
            break;

        // 判断内存是否有效
        if (!MmIsAddressValid((PVOID)module_ptr))
            break;

        // 获取模块路径
        PUNICODE_STRING full_name = (PUNICODE_STRING)((PUCHAR)ptr + 0x024);
        if(MmIsAddressValid(full_name->Buffer)) {
            RtlCopyMemory(module_ptr->path, full_name->Buffer, full_name->MaximumLength);      // 拷贝路径
            module_ptr->base = *(PULONG)((PUCHAR)ptr + 0x018);      // 获取模块基址
            module_ptr->size = *(PULONG)((PUCHAR)ptr + 0x020);      // 获取模块大小
            module_ptr++;
            copy_size += sizeof(struct ModuleItem);
        }

        ptr = ptr->Flink;
    } while (ptr != list_entry_ptr);


    // 出临界区
    KeLeaveCriticalRegion();

    // 分离进程
    KeUnstackDetachProcess(&kapc_state);

    return copy_size;
}

ULONG_PTR get_process_thread_count(ULONG pid, PULONG output)
{
    KAPC_STATE kapc_state;

    // 找到eprocess
    PEPROCESS eprocess = find_process_by_id(pid);

    // 获取PEB
    PPEB peb_ptr = (PPEB)(*(PUINT_PTR)((PUCHAR)eprocess + 0x1a8));
    if (peb_ptr == NULL) {
        return 0;
    }

    // 附加到进程
    KeStackAttachProcess(eprocess, &kapc_state);

    ULONG count = 0;

    // 获取线程的ThreadListHead
    PLIST_ENTRY list_entry = (PLIST_ENTRY)((PUCHAR)eprocess + 0x02c);
    if (!MmIsAddressValid((PVOID)list_entry)) {
        // 分离进程
        KeUnstackDetachProcess(&kapc_state);
        return 0;
    }

    PLIST_ENTRY traversal = list_entry;
    // 遍历
    do {
        // 获取ethread
        PETHREAD ethread = (PETHREAD)((PUCHAR)traversal - 0x1e0);
        // 获取teb
        PVOID teb = *(PVOID *)((PUCHAR)list_entry - 0x158);
        // 判断是否有效
        if (MmIsAddressValid((PVOID)ethread)) {
                count++;
        }
        // 指向下一个节点
        traversal = traversal->Flink;
    } while (traversal != list_entry);

    // 分离进程
    KeUnstackDetachProcess(&kapc_state);

    *output = count;

    return sizeof(count);
}

ULONG_PTR get_process_thread(ULONG pid, PVOID output, ULONG output_size)
{
    KAPC_STATE kapc_state;

    // 找到eprocess
    PEPROCESS eprocess = find_process_by_id(pid);

    // 获取当前tid
    ULONG tid = (ULONG)PsGetCurrentThreadId();

    // 获取PEB
    PPEB peb_ptr = (PPEB)(*(PUINT_PTR)((PUCHAR)eprocess + 0x1a8));
    if (peb_ptr == NULL) {
        return 0;
    }

    // 附加到进程
    KeStackAttachProcess(eprocess, &kapc_state);

    ULONG count = 0;

    // 获取线程的ThreadListHead
    PLIST_ENTRY list_entry = (PLIST_ENTRY)((PUCHAR)eprocess + 0x02c);
    if (!MmIsAddressValid((PVOID)list_entry)) {
        // 分离进程
        KeUnstackDetachProcess(&kapc_state);
        return 0;
    }

    PLIST_ENTRY traversal = list_entry;
    struct ThreadItem *ptr = (struct ThreadItem *)output;
    ULONG copy_size = 0;

    // 遍历
    do {
        if (copy_size >= output_size)
            break;
        // 获取ethread
        PETHREAD ethread = (PETHREAD)((PUCHAR)traversal - 0x1e0);
        // 获取tab
        PVOID teb = *(PVOID *)((PUCHAR)list_entry - 0x158);
        // 获取pid
        CLIENT_ID cid = *(PCLIENT_ID)((PUCHAR)ethread + 0x22c);
        // 判断是否有效
        if (MmIsAddressValid((PVOID)ethread)) {
            ptr->ethread = (unsigned int)ethread;
            ptr->teb = (unsigned int)teb;
            ptr->pid = (unsigned int)cid.UniqueThread;
            copy_size += sizeof(struct ThreadItem);
            ptr++;
        }
        // 指向下一个节点
        traversal = traversal->Flink;
    } while (traversal != list_entry);

    // 分离进程
    KeUnstackDetachProcess(&kapc_state);

    return copy_size;
}

ULONG_PTR get_ssdt_count(PULONG output, ULONG output_size)
{
    // 获取SSDT的位置
    SSDT *ssdt = (SSDT *)KeServiceDescriptorTable;

    if (!MmIsAddressValid((PVOID)ssdt))
        return 0;
    *output = ssdt->count;
    return output_size;
}

ULONG_PTR get_ssdt(PVOID output, ULONG output_size)
{
    // 获取SSDT
    SSDT *ssdt = (SSDT *)KeServiceDescriptorTable;

    if (!MmIsAddressValid((PVOID)ssdt) && !MmIsAddressValid((PVOID)(ssdt->func_arr_ptr)))
        return 0;

    // 遍历
    SSDTItem *ptr = (SSDTItem *)output;
    for(ULONG i = 0; i < output_size / sizeof(SSDTItem); i++, ptr++) {
        ptr->num = i;
        ptr->addr = ((PULONG)(ssdt->func_arr_ptr))[i];
    }

    return output_size;
}

ULONG_PTR get_shadowssdt_count(PULONG output, ULONG output_size)
{
    // 获取ShadowSSDT位置
    ShadowSSDT *shadow_ssdt = (ShadowSSDT *)(KeServiceDescriptorTable + 0x50);

    if (!MmIsAddressValid((PVOID)shadow_ssdt))
        return 0;
    *output = shadow_ssdt->count;
    return output_size;
}

ULONG_PTR get_shadowssdt(PVOID output, ULONG output_size)
{
    // 获取ShadowSSDT
    ShadowSSDT *shadow_ssdt = (ShadowSSDT *)(KeServiceDescriptorTable + 0x50);

    if (!MmIsAddressValid((PVOID)shadow_ssdt) && !MmIsAddressValid((PVOID)(shadow_ssdt->func_arr_ptr)))
        return 0;

    // 遍历
    ShadowSSDTItem *ptr = (ShadowSSDTItem *)output;
    for (ULONG i = 0; i < output_size / sizeof(ShadowSSDTItem); i++, ptr++) {
        ptr->num = i;
        ptr->addr = ((PULONG)(shadow_ssdt->func_arr_ptr))[i];
    }

    return output_size;
}

ULONG_PTR get_driver_module_count(PDRIVER_OBJECT pDriverObject, PULONG output)
{
    LDR_DATA_TABLE_ENTRY *pDataTableEntry, *pTempDataTableEntry;

    // 双向循环链表
    PLIST_ENTRY pList;

    // 指向驱动对象的DriverSection
    pDataTableEntry = (LDR_DATA_TABLE_ENTRY*)pDriverObject->DriverSection;

    // 判断是否为空
    if (!pDataTableEntry)
        return 0;

    ULONG count = 0;

    // 开始遍历驱动对象链表
    pList = pDataTableEntry->InLoadOrderLinks.Flink;
    while (pList != &pDataTableEntry->InLoadOrderLinks) {
        pTempDataTableEntry = (LDR_DATA_TABLE_ENTRY *)pList;
        count++;
        pList = pList->Flink;
    }

    *output = count;

    return sizeof(count);
}

ULONG_PTR get_driver_module(PDRIVER_OBJECT pDriverObject, PVOID output, ULONG output_size)
{
    LDR_DATA_TABLE_ENTRY *pDataTableEntry, *pTempDataTableEntry;

    // 双向循环链表
    PLIST_ENTRY pList;

    // 指向驱动对象的DriverSection
    pDataTableEntry = (LDR_DATA_TABLE_ENTRY*)pDriverObject->DriverSection;

    // 判断是否为空
    if (!pDataTableEntry)
        return 0;

    ULONG count = 0;
    ULONG copy_bytes = 0;
    struct DriverModule *ptr = (struct DriverModule *)output;

    // 开始遍历驱动对象链表
    pList = pDataTableEntry->InLoadOrderLinks.Flink;
    while (pList != &pDataTableEntry->InLoadOrderLinks) {
        pTempDataTableEntry = (LDR_DATA_TABLE_ENTRY *)pList;

        if(MmIsAddressValid(pTempDataTableEntry->BaseDllName.Buffer) && pTempDataTableEntry->BaseDllName.Buffer) {
            // 驱动名
            RtlCopyMemory(ptr->name, pTempDataTableEntry->BaseDllName.Buffer, pTempDataTableEntry->BaseDllName.MaximumLength);
            // 基址
            ptr->base = (unsigned int)pTempDataTableEntry->DllBase;
            // 大小
            ptr->size = pTempDataTableEntry->SizeOfImage;
            // 驱动路径
            RtlCopyMemory(ptr->path, pTempDataTableEntry->FullDllName.Buffer, pTempDataTableEntry->FullDllName.MaximumLength);
            ptr++;
            copy_bytes += sizeof(struct DriverModule);
        }

        if (copy_bytes >= output_size)
            break;

        pList = pList->Flink;
    }

    return copy_bytes;
}
