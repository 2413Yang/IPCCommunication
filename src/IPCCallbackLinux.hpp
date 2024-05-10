#ifndef IPCCALLBACK_LINUX_H
#define IPCCALLBACK_LINUX_H

#include <functional>
#include <vector>
#include <mutex>
#include "ArgumentsTableLinux.hpp"
#include <json/json.h> // Linux 平台使用的 jsoncpp 头文件

// 无参无返回值的函数类型别名 CBVoid
using CBVoid = std::function<void(void)>;

// IPC回调类模板，用于管理回调函数和处理接收到的参数
template <typename... T>
class CIPCCallback
{
public:
    // IPC回调类的构造函数，用于初始化 IPC 回调类，并传入参数表格的引用。
    CIPCCallback(CArgumentsTable<T...> &ArgTable) : mArgsLocal(&ArgTable) {}
    ~CIPCCallback() {}

    // 注册回调函数。接受一个函数指针作为参数，返回一个绑定了参数表格的回调函数。
    // 使用 std::bind 将回调函数和参数表格绑定在一起，以便在接收到参数时调用
    CBVoid Register(void (*cb)(T...))
    {
        return std::bind(cb, std::get<T>(std::move((*mArgsLocal)()))...);
    }

    // 重载 << 用于添加回调函数到回调函数列表中
    void operator<<(CBVoid &func)
    {
        mCbTable.push_back(func);
    }

    // 处理接收到的参数。
    // 它首先调用参数表格的 separate 函数，判断参数是否分离成功，如果分离成功则依次调用回调函数列表中的每个回调函数
    void SubInterface(Json::Value &arg)
    {
        if (mArgsLocal->separate(arg))
        {
            std::lock_guard<std::mutex> lock(mMutex);
            for (auto &it : mCbTable)
                it()();
        }
    }

    // 函数调用运算符重载，用于执行所有注册的回调函数。它依次调用回调函数列表中的每个回调函数
    void operator()()
    {
        std::lock_guard<std::mutex> lock(mMutex);
        for (auto &it : mCbTable)
            it()();
    }

private:
    std::mutex mMutex;
    CArgumentsTable<T...> *mArgsLocal;  //参数表格指针
    std::vector<CBVoid> mCbTable;       //回调函数的列表
};

#endif // IPCCALLBACK_LINUX_H
