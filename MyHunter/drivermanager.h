#ifndef DRIVERMANAGER_H
#define DRIVERMANAGER_H

#include <Windows.h>

#define SERVICE_NAME TEXT("MyHunter")
#define SERVICE_SHOW SERVICE_NAME

class DriverManager
{
public:
    DriverManager();
    virtual ~DriverManager();

    bool install_driver();      // 安装驱动
    bool start_driver();        // 启动驱动
    bool stop_driver();         // 停止驱动
    bool uninstall_driver();    // 卸载驱动

    bool connect_driver(const TCHAR *symbol_name);      // 链接驱动
    void disconnect_driver();

    /*
     * 驱动控制
     *      control_code: 控制码
     *      in_buf：输入缓冲区
     *      in_buf_size：输入缓冲区大小
     *      out_buf：输出缓冲区
     *      out_buf_size：输出缓冲区大小
     *      ret_bytes：返回字节数
     */
    bool io_control(DWORD control_code, LPVOID in_buf, DWORD in_buf_size, LPVOID out_buf, DWORD out_buf_size, LPDWORD ret_bytes);

    void set_driver(const TCHAR *path);   // 设置驱动路径

    inline DWORD get_last_error() const { return last_error_; } // 获取错误码

private:
    TCHAR *driver_path = nullptr;       // 驱动路径
    SC_HANDLE scm_manager_ = nullptr;   // 服务管理器句柄
    SC_HANDLE service_ = nullptr;       // 服务句柄
    HANDLE driver_ = INVALID_HANDLE_VALUE;  // 驱动句柄

    DWORD last_error_ = 0;  // 错误码
};

#endif // DRIVERMANAGER_H
