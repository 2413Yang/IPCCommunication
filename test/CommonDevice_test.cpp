#include "../src/CommunicDevice.hpp"
#include <iostream>
#include <fcntl.h>
#include <unistd.h>

class A : public CCommunicDevice{
public:
    A(unsigned size) : CCommunicDevice(size){}
    virtual void start() override{}
    virtual void stop() override {}
    void addFD(int fd){
        CCommunicDevice::addFD(fd);
    }
    void delFD(int fd){
        CCommunicDevice::delFD(fd);
    }
    void handleEvent(int fd){
        CCommunicDevice::handleEvent(fd);
    }
};

void onDataReceived(const std::string &data){
    std::cout <<"onDataReceived: "<< data << std::endl;
}

int main(){
    // 创建通信设备对象
    A device(1024);
    // 注册回调函数
    device.registerCB(onDataReceived);
    
    // 打开串口设备文件
    int fd = open("/home/yang/dev/mypipe", O_RDWR);
    if (fd == -1){
        std::cout << "Failed to open serial port." << std::endl;
        return 1;
    }
    //添加文件描述符到设备文件种
    device.addFD(fd);
    // 事件循环
    while (true)
    {
        // 处理设备的事件
        // device.handleEvent(fd);

        // // 发送数据到串口
        // std::string message = "Hello, Serial Port!";
        // device.send(fd, message);

        // 延时一段时间
        usleep(1000000); // 1秒
    }

    // 删除文件描述符
    device.delFD(fd);

    // 关闭串口设备文件
    close(fd);
    
    return 0;
}