#ifndef IPCINTERFACECORE_LINUX__H__
#define IPCINTERFACECORE_LINUX__H__

#include <atomic>
#include "ArgumentsTableLinux.hpp"
#include "IPCCallbackLinux.hpp"

// 宏定义 USER_DEFINED，用于定义用户自定义的IPC接口
#define USER_DEFINED(returnType, funcName, ...)                                                                     \
    class Ipc##funcName                                                                                             \
    {                                                                                                               \
    public:                                                                                                         \
        using Indices = argvs::ArgIndexTuple<std::tuple_size<std::tuple<__VA_ARGS__>>::value>::__type;              \
                                                                                                                    \
        /*函数调用运算符重载 operator()()，用于调用特定接口*/                                                          \
        /*当调用这个函数时，会首先检查参数是否符合要求，然后根据是否有发布者来确定是发布接口还是调用回调函数。*/             \
        template <typename... T>                                                                                    \
        void operator()(T... args)                                                                                  \
        {                                                                                                           \
            if (mArguments(std::forward<T>(args)...))                                                               \
            {                                                                                                       \
                if (m_ptrPublisher.load())                                                                          \
                    m_ptrPublisher.load()->publish(std::string(#funcName), mArguments.data());                      \
                else                                                                                                \
                    mIPCcb();                                                                                       \
            }                                                                                                       \
        }                                                                                                           \
                                                                                                                    \
        /*获取特定接口的单例对象。它确保每个接口只有一个实例*/                                                           \
        static Ipc##funcName &GetObj()                                                                              \
        {                                                                                                           \
            static Ipc##funcName it;                                                                                \
            return it;                                                                                              \
        }                                                                                                           \
                                                                                                                    \
    private:                                                                                                        \
        Ipc##funcName() : m_ptrPublisher(nullptr), mArguments(#funcName), mIPCcb(mArguments) {}                    \
        ~Ipc##funcName() {}                                                                                         \
                                                                                                                    \
    public:                                                                                                         \
        std::atomic<CPublisher *> m_ptrPublisher;     /*发布者指针*/                                                              \
        CArgumentsTable<__VA_ARGS__> mArguments;      /*参数表格*/                                                              \
        CIPCCallback<__VA_ARGS__> mIPCcb;             /*IPC回调对象*/                                                              \
    };                                                                                                              \
                                                                                                                    \
    /*用于调用特定接口,直接通过单例对象调用接口的函数调用运算符重载*/                                                     \
    template <typename... T>                                                                                        \
    returnType funcName(T... args)                                                                                  \
    {                                                                                                               \
        Ipc##funcName::GetObj()(args...);                                                                           \
    }                                                                                                               \
                                                                                                                    \
    /*命名空间 subscriber 内的函数模板，用于注册回调函数到 IPC 回调对象中。*/ \
    /*使用 std::function 将回调函数注册到 IPC 回调对象中。*/ \
    namespace subscriber                                                                                            \
    {                                                                                                               \
        template <typename ICallback>                                                                               \
        void funcName(ICallback &cb)                                                                                \
        {                                                                                                           \
            std::function<std::function<void(void)>(void)> mReg = [&cb]() -> std::function<void(void)> {            \
                return Ipc##funcName::GetObj().mIPCcb.Register(&ICallback::funcName, cb, Ipc##funcName::Indices()); \
            };                                                                                                      \
            Ipc##funcName::GetObj().mIPCcb << mReg;                                                                 \
        }                                                                                                           \
                                                                                                                    \
        template <typename subscrib, typename ICallback>                                                            \
        void funcName(subscrib &Sub, ICallback &cb)                                                                 \
        {                                                                                                           \
            subscriber::funcName(cb);                                                                               \
            Sub.regSubscriber(#funcName, &Ipc##funcName::GetObj().mIPCcb);                                          \
        }                                                                                                           \
    }                                                                                                               \
                                                                                                                    \
    /*命名空间 publisher 内的函数模板，用于设置发布者的指针。*/ \
    /*将发布者的指针赋值给 IPC 接口的 m_ptrPublisher 成员变量*/ \
    namespace publisher                                                                                             \
    {                                                                                                               \
        template <typename publish>                                                                                 \
        void funcName(publish &args)                                                                                \
        {                                                                                                           \
            Ipc##funcName::GetObj().m_ptrPublisher = &args;                                                         \
        }                                                                                                           \
    }

#endif /*IPCINTERFACECORE_LINUX__H__*/
