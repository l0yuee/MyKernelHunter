#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__


// 驱动中使用
#define MY_DEVICE_NAME TEXT("\\Device\\MyDevice-dsh")           // 设备名
#define MY_SYMBOL_NAME TEXT("\\DosDevices\\MyDevice-dsh")       // 符号名


// 应用程序中使用
#define MY_DRIVER_FILE "MyHunterDriver.sys"         // 驱动文件名
#define MY_SYMBOL_FILE TEXT("\\\\.\\MyDevice-dsh")  // 符号连接名

//自定义操作码宏
#define MY_CODE(function) CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800 + (function), METHOD_BUFFERED, FILE_ANY_ACCESS)

#define CODE_GET_GDT_BUFSIZE MY_CODE(0)     // 获取GDT大小
#define CODE_GET_GDT MY_CODE(1)             // 获取GDT

#define CODE_GET_EPROCESS MY_CODE(2)        // 获取EPROCESS
#define CODE_GET_IMAGE_PATH MY_CODE(3)      // 获取映像路径

#define CODE_GET_IDT MY_CODE(4)             // 获取IDT

#define CODE_GET_PROCESS_MODULE_COUNT MY_CODE(5)    // 获取进程模块个数
#define CODE_GET_PROCESS_MODULE MY_CODE(6)          // 获取进程模块信息

#define CODE_GET_PROCESS_THREAD_COUNT MY_CODE(7)    // 获取进程模块个数
#define CODE_GET_PROCESS_THREAD MY_CODE(8)          // 获取进程模块信息

#define CODE_SSDT_COUNT MY_CODE(9)  // 获取进程模块个数
#define CODE_SSDT MY_CODE(10)       // 获取进程模块信息

#define CODE_SHADOWSSDT_COUNT MY_CODE(11)   // 获取进程模块个数
#define CODE_SHADOWSSDT MY_CODE(12)         // 获取进程模块信息

#define CODE_DRIVER_MODULE_COUNT MY_CODE(13)    // 获取驱动模块个数
#define CODE_DRIVER_MODULE MY_CODE(14)          // 获取驱动模块

// 相关数据结构
#pragma pack(push)
#pragma pack(1)

/*
 * 段选择子
 */
struct SegmentSelector {
    unsigned int rpl : 2;       // 特权级
    unsigned int ti : 1;        // 为0指向GDT，为1指向LDT
    unsigned int index : 13;    // 索引
};

/*
 * 段描述符，共8字节
 *      base = 段基址，32位
 *      limit = 段界限，20位
 *          当 G = 0时，limit * 0 + 0xfff
 *          当 G = 1时，limit * 4K + 0xfff
 */
struct SegmentDescriptor {
    union {
        unsigned int low32;                     // 低32位字段
        struct {
            unsigned int limit_0_15 : 16;       // 第0-15位的段界限
            unsigned int base_0_15 : 16;        // 第0-15位的段基址
        };
    } SegDesLow;                                // 段描述符低32位字段

    union {
        unsigned int high32;                    // 高32位字段
        struct {
            unsigned int base_16_23 : 8;        // 第16-23位的段基址
            unsigned int type : 4;              // 内存属性
            unsigned int s : 1;                 // 为0则是系统段，为1则是存储段
            unsigned int dpl : 2;               // 特权级（0 ~ 3）
            unsigned int p : 1;                 // 存在位，表示段描述符是否有效
            unsigned int limit_16_19 : 4;       // 第16-19位的段界限
            unsigned int avl : 1;               // 软件可利用位，由操作系统决定作用
            unsigned int l : 1;                 // 64位段标志
            unsigned int d_b : 1;               // 为0则是16位段，为1则是32位段
            unsigned int g : 1;                 // 粒度位，为1单位是4K，为0是单位是1字节
            unsigned int base_24_31 : 8;        // 第24-31位的段基址
        };
    } SegDesHigh;                               // 段描述符高32位字段
};


/*
 * GDTR全局描述符表寄存器
 */
struct GDTR {
    unsigned short limit;   // 全局描述符表的界限
    unsigned int address;   // 全局描述符表的地址
};


/*
 * 门描述符类型
 */

#define TASK_GATE 5         // 任务门
#define INTERRUPT_GATE 6    // 中断门
#define TRAP_GATE 7         // 陷阱门

/*
 * 门描述符
 *      s位必定为0
 */
struct GateDescriptor {
    union {
        unsigned int low32;                     // 低32位字段
        struct {
            unsigned int offset_0_15 : 16;      // 第0-15位的偏移
            unsigned int seg_sel : 16;          // 选择子
        };
    } GateDesLow;                               // 门描述符低32位字段

    union {
        unsigned int high32;                    // 高32位字段
        struct {
            unsigned int reserve_0_4 : 5;       // 保留
            unsigned int reserve_5_7 : 3;       // 保留或全0
            unsigned int type : 3;              // 类型 Task: 5, Interrupt: 6, Trap: 7
            unsigned int d : 1;                 // 门的大小，1 = 32位，0 = 16位
            unsigned int s : 1;                 // 0系统段
            unsigned int dpl : 2;               // 特权级
            unsigned int p : 1;                 // 存在位
            unsigned int offset_16_31 : 16;     // 第16-31位的偏移
        };
    } GateDesHigh;                              // 门描述符高32位字段
};


/*
 * IDTR中断描述符寄存器
 */
struct IDTR {
    unsigned short limit;   // 全局描述符表的界限
    unsigned int address;   // 全局描述符表的地址
};


/*
 * SSDT
 */
typedef struct SSDT {
    unsigned int func_arr_ptr;      // api数组的指针
    unsigned int reserve;           // 保留
    unsigned int count;             // 数组元素个数
    unsigned char param_arr_ptr;    // api参数数组指针
} SSDT, ShadowSSDT;

/*
 * 模块节点
 */
struct ModuleItem {
    wchar_t path[0x200];    // 路径
    unsigned int base;      // 基址
    unsigned int size;      // 大小
};

/*
 * 线程节点
 */
struct ThreadItem {
    unsigned int pid;
    unsigned int ethread;
    unsigned int teb;
};


/*
 * SSDT节点
 */
typedef struct SSDTItem {
    unsigned int num;   // 序号
    unsigned int addr;  // 地址
} SSDTItem, ShadowSSDTItem;


/*
 * 驱动模块节点
 */
struct DriverModule {
    wchar_t name[0x100];    // 驱动名
    unsigned int base;      // 基址
    unsigned int size;      // 大小
    wchar_t path[0x100];    // 驱动路径
};

#pragma pack(pop)
#endif
