#include "drivermanager.h"
#include <tchar.h>

DriverManager::DriverManager()
{

}

DriverManager::~DriverManager()
{
    // 清理资源
    if (driver_path)
        delete[] driver_path;
    if (service_)
        ::CloseServiceHandle(service_);
    if (scm_manager_)
        ::CloseServiceHandle(scm_manager_);
}

bool DriverManager::install_driver()
{
    // 建立与指定计算机上的服务控制管理器的连接，并打开指定的服务控制管理器数据库
    scm_manager_ = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if(!scm_manager_) {
        // 获取错误码
        last_error_ = ::GetLastError();
        return false;
    }

    // 创建服务
    service_ = ::CreateService(scm_manager_, SERVICE_NAME, SERVICE_SHOW, SERVICE_ALL_ACCESS,
        SERVICE_KERNEL_DRIVER, SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL, driver_path,
        nullptr, nullptr, nullptr, nullptr, nullptr);
    if(!service_) {
        // 获取错误码并关闭服务管理器句柄
        last_error_ = ::GetLastError();
        ::CloseServiceHandle(scm_manager_);
        scm_manager_ = nullptr;
        return false;
    }

    // 关闭句柄
    ::CloseServiceHandle(service_);
    ::CloseServiceHandle(scm_manager_);
    service_ = nullptr;
    scm_manager_ = nullptr;

    return true;
}

bool DriverManager::start_driver()
{
    // 建立与指定计算机上的服务控制管理器的连接，并打开指定的服务控制管理器数据库
    scm_manager_ = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (!scm_manager_) {
        // 获取错误码
        last_error_ = ::GetLastError();
        return false;
    }

    // 打开现有服务
    service_ = ::OpenService(scm_manager_, SERVICE_NAME, SERVICE_ALL_ACCESS);
    if (!service_) {
        // 获取错误码并关闭服务管理器句柄
        last_error_ = ::GetLastError();
        ::CloseServiceHandle(scm_manager_);
        scm_manager_ = nullptr;
        return false;
    }

    // 启动服务
    bool ret = ::StartService(service_, 0, nullptr);
    if(!ret) {
        // 启动失败，获取错误码
        last_error_ = ::GetLastError();
    }

    // 关闭句柄
    ::CloseServiceHandle(service_);
    ::CloseServiceHandle(scm_manager_);
    service_ = nullptr;
    scm_manager_ = nullptr;

    return ret;
}

bool DriverManager::stop_driver()
{
    // 建立与指定计算机上的服务控制管理器的连接，并打开指定的服务控制管理器数据库
    scm_manager_ = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (!scm_manager_) {
        // 获取错误码
        last_error_ = ::GetLastError();
        return false;
    }

    // 打开现有服务
    service_ = ::OpenService(scm_manager_, SERVICE_NAME, SERVICE_ALL_ACCESS);
    if (!service_) {
        // 获取错误码并关闭服务管理器句柄
        last_error_ = ::GetLastError();
        ::CloseServiceHandle(scm_manager_);
        scm_manager_ = nullptr;
        return false;
    }

    // 停止服务
    SERVICE_STATUS status = { 0 };
    bool ret = ::ControlService(service_, SERVICE_CONTROL_STOP, &status);
    if (!ret) {
        // 启动失败，获取错误码
        last_error_ = ::GetLastError();
    }

    // 关闭句柄
    ::CloseServiceHandle(service_);
    ::CloseServiceHandle(scm_manager_);
    service_ = nullptr;
    scm_manager_ = nullptr;

    return ret;
}

bool DriverManager::uninstall_driver()
{
    // 建立与指定计算机上的服务控制管理器的连接，并打开指定的服务控制管理器数据库
    scm_manager_ = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (!scm_manager_) {
        // 获取错误码
        last_error_ = ::GetLastError();
        return false;
    }

    // 打开现有服务
    service_ = ::OpenService(scm_manager_, SERVICE_NAME, SERVICE_ALL_ACCESS);
    if (!service_) {
        // 获取错误码并关闭服务管理器句柄
        last_error_ = ::GetLastError();
        ::CloseServiceHandle(scm_manager_);
        scm_manager_ = nullptr;
        return false;
    }

    bool ret = ::DeleteService(service_);
    if (!ret) {
        // 启动失败，获取错误码
        last_error_ = ::GetLastError();
    }

    // 关闭句柄
    ::CloseServiceHandle(service_);
    ::CloseServiceHandle(scm_manager_);
    service_ = nullptr;
    scm_manager_ = nullptr;

    return ret;
}

bool DriverManager::connect_driver(const TCHAR *symbol_name)
{
    // 连接驱动
    driver_ = ::CreateFile(symbol_name, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if(driver_ == INVALID_HANDLE_VALUE) {
        // 连接失败，获取返回值
        last_error_ = ::GetLastError();
        return false;
    }
    return true;
}

void DriverManager::disconnect_driver()
{
    if(driver_ != INVALID_HANDLE_VALUE)
        ::CloseHandle(driver_);
}

bool DriverManager::io_control(DWORD control_code, LPVOID in_buf, DWORD in_buf_size, LPVOID out_buf, DWORD out_buf_size,
    LPDWORD ret_bytes)
{
    if(!::DeviceIoControl(driver_, control_code, in_buf, in_buf_size, out_buf, out_buf_size, ret_bytes, NULL)) {
        last_error_ = ::GetLastError();
        return false;
    }
    return true;
}

void DriverManager::set_driver(const TCHAR *path)
{
    // 释放空间
    if (driver_path)
        delete[] driver_path;

    // 申请空间
    DWORD length = _tcslen(path);
    DWORD bytes = (length + 1) * sizeof(TCHAR);
    driver_path = new TCHAR[bytes];
    // 拷贝字符串
    _tcsncpy(driver_path, path, length + 1);
}
