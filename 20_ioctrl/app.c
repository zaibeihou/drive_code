#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>

#define CMD_TEST _IOW('A',1,long)
#define CMD_TEST1 _IOR('A',2,int)

struct app
{
    int a;
    int b;
    int c;
};


int main(int argc, char *argv[])
{
    int fd; 
    int val = 0;                             // 定义int类型文件描述符
    struct app app = {
        .a = 1,
        .b = 0,
        .c = 1,
    };

    fd = open("/dev/test0", O_RDWR); // 打开/dev/test设备
    if (fd < 0){
        printf("file open error\n");
        return fd;
    }
    ioctl(fd,CMD_TEST1,&val);
    printf("val = %d\n",val);
    
    ioctl(fd, CMD_TEST, &app);
    
    close(fd);
    return 0;
}
