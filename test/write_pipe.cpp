#include <iostream>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>

int main() {
    // 打开管道
    int fd = open("/home/yang/dev/mypipe", O_WRONLY);
    if (fd == -1) {
        std::cerr << "Failed to open pipe for writing." << std::endl;
        return 1;
    }

    // 向管道写入数据
    const char *message = "Hello, pipe!";
    ssize_t bytes_written = write(fd, message, strlen(message));
    if (bytes_written == -1) {
        std::cerr << "Failed to write to pipe." << std::endl;
        close(fd);
        return 1;
    }

    // 关闭管道
    close(fd);

    std::cout << "Data written to pipe successfully." << std::endl;

    return 0;
}
