#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <iostream>
#include "DeviceListener.hpp"
#include "CommunicDevice.hpp"

std::thread *CCommunicDevice::pProducer = NULL;   //生产者
std::thread *CCommunicDevice::pConsumer = NULL;   //消费者
CTemplateCache<std::function<void(void)>> CCommunicDevice::taskQueue;

typedef std::pair<int, CCommunicDevice *> HandleType;

CCommunicDevice::CCommunicDevice(unsigned size) : mRemainderFrame("")
{
    mSize = size;
    pReadCache = new (std::nothrow) unsigned char[mSize]; // 分配一块连续的内存，分配失败返回nullptr
    debugId = 0;
    std::lock_guard<std::mutex> guard(mLock);
    if (NULL == pProducer)
    {
        auto listenRead = []() -> void
        {
            while (true)
            {
                auto table = CSelectListener::getInstance()();
                for (auto it : table)
                {
                    if (NULL != it.second)
                        it.second->handleEvent(it.first);
                    else
                        std::cout << "file handle corresponds to an empty object." << std::endl;
                }
                sched_yield();
            }
        };
        pProducer = new std::thread(listenRead);
        if (NULL != pProducer)
            pProducer->joinable() ? pProducer->detach() : void(0);
        else
            std::cout << "producer thread creation failed." << std::endl;
    }

    if (NULL == pConsumer)
    {
        auto actuator = [this]() -> void
        {
            std::function<void(void)> task;
            while (true)
            {
                taskQueue.wait();
                while (taskQueue.pull(task))
                    task();
                sched_yield();
            }
            std::cout << "" << std::endl;
        };
        pConsumer = new std::thread(actuator);
        if (NULL != pConsumer)
            pConsumer->joinable() ? pConsumer->detach() : void(0);
        else
            std::cout << "consumer thread creation failed." << std::endl;
    }
}

CCommunicDevice::~CCommunicDevice()
{
    if (pReadCache)
        delete[] pReadCache;
    pReadCache = NULL;
}

uint32_t CCommunicDevice::getMsgCount()
{
	return taskQueue.getSize();
}
void CCommunicDevice::handleEvent(int fd)
{
    std::cout << "CCommunicDevice::handleEvent. " << " fd: " << fd << std::endl;
    if (!pReadCache)
    {
        std::cout << "read cache is nullptr." << std::endl;
        return;
    }
    memset(pReadCache, 0, mSize);
    int n = read(fd, pReadCache, mSize);
    std::cout << "n: " << n << " pReadCache: " << pReadCache << std::endl;
    if (n <= 0)
    {
        std::cout << "device data read error "<< n << " " << strerror(errno) << std::endl;
        return;
    }
    if(debugId == 1)
    {
        //LOGINF("###..n = %d, taskQueue.size = %u", n, taskQueue.getSize());
    }
    // LOGHEX(pReadCache, n, "read-frame");
    auto task = std::bind(&CCommunicDevice::loopCb, this, std::string((char *)pReadCache, n));
    taskQueue.push(task);
}

void CCommunicDevice::send(std::string str)
{
    int iDevFd = CSelectListener::getInstance()(this);
    int n = write(iDevFd, str.data(), str.length());
    if (n <= 0)
    {
        std::cout << "[iDevFd: "<< iDevFd <<" ] Data transmission failed."<< strerror(errno) << std::endl;
        std::cout << str.data() << " " << str.length() << "SEND FAILED" << std::endl;
    }
}

void CCommunicDevice::send(uint8_t *buf, uint32_t length)
{
    int iDevFd = CSelectListener::getInstance()(this);
    int n = write(iDevFd, buf, length);
    if (n <= 0)
    {
        std::cout << "[iDevFd: "<< iDevFd <<" ] Data transmission failed."<< strerror(errno) << std::endl;
        std::cout << buf << " " << length << "SEND FAILED" << std::endl;
    }
}

void CCommunicDevice::send(int iDevFd, std::string str)
{
    int n = write(iDevFd, str.data(), str.length());
    if (n <= 0)
    {
        std::cout << "[iDevFd: "<< iDevFd <<" ] Data transmission failed."<< strerror(errno) << std::endl;
        std::cout << str.data() << " " << str.length() << "SEND FAILED" << std::endl;

    }
}

void CCommunicDevice::addFD(int iDevFD)
{
    CSelectListener::getInstance() << CSelectListener::HandleType(iDevFD, this);
    CSelectListener::getInstance().reset();
}

void CCommunicDevice::delFD(int iDevFD)
{
    CSelectListener::getInstance() >> CSelectListener::HandleType(iDevFD, this);
}

void CCommunicDevice::swapCore(CCommunicDevice *ptrCore)
{
    int fd = CSelectListener::getInstance()[this];
    if (fd > 0)
    {
        CSelectListener::getInstance() >> CSelectListener::HandleType(fd, this);
        CSelectListener::getInstance() << CSelectListener::HandleType(fd, ptrCore);
        CSelectListener::getInstance().reset();
    }
    else
        std::cout << "swap core is error." << std::endl;
}

void CCommunicDevice::registerCB(std::function<void(std::string)> cb)
{
    taskQueue.lock();
    mCB = cb;
    taskQueue.unlock();
}

bool CCommunicDevice::getFrame(std::string &cacheFrame, std::string &frame)
{
    bool ret = false;
    std::size_t Offset = 0;
    unsigned frameLength = 0;
    if(0 == cacheFrame.length())
    {
        return ret;
    }
    
    uint8_t FrameHeader[3] = {0xFF, 0xAA, 0x00};
    if (std::string::npos != (Offset = cacheFrame.find((char*)FrameHeader)))
    {
        if(cacheFrame.length() > (Offset + 7))
        {
            frameLength = (cacheFrame[Offset + 6] & 0xFF);
            //帧长度最小为9
            if(9 > frameLength)
            {//此帧数据格式有问题，舍弃帧头
                std::cout << "recv mcu frameLength = 0, cacheFrame.length() "<< cacheFrame.length() << std::endl;
                cacheFrame.assign(cacheFrame, Offset + 2, cacheFrame.length());
                frame = std::string("");
                return ret;
            }
        }
        else
        {
            frame = std::string("");
            return ret;
        }
        cacheFrame.length() >= (Offset + frameLength)
            ? (frame.assign(cacheFrame, Offset, frameLength),
            cacheFrame.assign(cacheFrame, Offset + frameLength, cacheFrame.length()))
            : (frame = std::string(""));
        frame.length() > 0 ? (ret = true) : (ret = false);
        #if 1
        if(ret)
        {
            ret = checkSum(frame.data(), frameLength);
            if(ret == false)
            {
                std::cout << frame.data() << frame.length() << " checkSumErr" << std::endl;
            }
        }
        #endif
    }
    return ret;
}

void CCommunicDevice::loopCb(std::string cacheFrame)
{
    bool ret = false;
    taskQueue.lock();
    auto cb = mCB;
    taskQueue.unlock();

    mRemainderFrame += cacheFrame;
    while (true)
    {
        std::string frame;
        ret = this->getFrame(mRemainderFrame, frame);
        if (ret && cb)
            cb(frame);
        else
        {
            // RemainderFrame = cacheFrame;
            break;
        }
    }
}

void CCommunicDevice::debug(void)
{
#if 0
    char buf1[] = {
        0xFF,
        0xAA,
        0xFF,
        0xAA,
        0x50,
        0x00,
        0x01,
        0x00,

    };
    char buf2[] = {0x0F,
                   0x06,
                   0x06,
                   0x12,
                   0x14,
                   0x05,
                   0x06,
                   0x07,
                   0x0A,
                   0xFF,
                   0xAA,
                   0x50,
                   0xFF,
                   0xFF, 0x00, 0x0F, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x0A};

    taskQueue.push(std::string(buf1, sizeof(buf1)));
    taskQueue.push(std::string(buf2, sizeof(buf2)));
    taskQueue.push(std::string(buf1, sizeof(buf1)));
    taskQueue.push(std::string(buf2, sizeof(buf2)));
#endif
}
bool CCommunicDevice::checkSum(const char* pdata, uint32_t len)
{
    uint8_t u8CheckSum = uint8_t(pdata[len - 2]);
    uint8_t calcCheckSum = 0;
    for(int i = 2; i < len - 2; i++)
    {
        calcCheckSum += pdata[i];
    }
    return calcCheckSum == u8CheckSum;
}
