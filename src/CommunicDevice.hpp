
#ifndef MSGDEVICE__H__
#define MSGDEVICE__H__
#include <functional>
#include <thread>
#include <mutex>
#include <vector>
#include <atomic>
#include <string>
#include <condition_variable>
#include "TemplateCache.hpp"

class CCommunicDevice // 通信设备接口类
{
public:
    CCommunicDevice(unsigned size = 255); // 通信缓存的大小默认为255B
    ~CCommunicDevice();
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void registerCB(std::function<void(std::string)>);
    virtual void send(std::string);
    virtual void send(uint8_t *, uint32_t);
    virtual void send(int, std::string);
    void swapCore(CCommunicDevice *);
	uint32_t getMsgCount(); // 获取任务数量

protected:
    virtual void handleEvent(int fd);
    virtual bool getFrame(std::string &cache, std::string &frame);
    void loopCb(std::string);
    void addFD(int);
    void delFD(int);
    void debug(void);
    bool checkSum(const char* pdata, uint32_t len);

private:
    std::mutex mLock;
    std::function<void(std::string)> mCB;

protected:
    unsigned int mSize;
    unsigned char *pReadCache;
    std::string mRemainderFrame;

    static std::thread *pProducer, *pConsumer;
    static CTemplateCache<std::function<void(void)>> taskQueue;
    int     debugId;
};

#endif /*CMSGDEVICE__H__*/