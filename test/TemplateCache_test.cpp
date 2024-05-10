#include "../src/TemplateCache.hpp"
#include <iostream>
#include <thread>

CTemplateCache<int> cache;

void producer(){
    int data = 10;
    cache.push(data);
}

void consumer(){
    cache.wait();
    int data;
    cache.pull(data);
    std::cout << data << std::endl;
}

int main(){
    
    std::thread t1(producer);
    std::thread t2(consumer);
    t1.join();
    t2.join();
    return 0;
}