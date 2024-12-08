#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>

#define CMD_TIME_CLOSE _IO('A',1)
#define CMD_TIME_OPEN _IO('A',2)
#define CMD_TIME_SET _IOW('A',2,int)

int main(int argc, char *argv[])
{
    int fd;                          // 定义int类型文件描述符
    fd = open("/dev/test0", O_RDWR); // 打开/dev/test设备
    if (fd < 0){
        printf("file open error\n");
        return fd;
    }
    
    ioctl(fd,CMD_TIME_SET,1000);
    ioctl(fd,CMD_TIME_OPEN);
    sleep(1);
    ioctl(fd,CMD_TIME_SET,3000);
    sleep(7);
    ioctl(fd,CMD_TIME_CLOSE);
    
    close(fd);
    return 0;
}
