#include "libtime/lib.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
    int fd;                          // 定义int类型文件描述符
    fd = open("/dev/test0", O_RDWR); // 打开/dev/test设备
    if (fd < 0){
        printf("file open error\n");
        return fd;
    }
    
    time_set(fd,1000);
    time_open(fd);
    sleep(1);
    time_set(fd,3000);
    sleep(7);
    time_close(fd);
    
    close(fd);
    return 0;
}
