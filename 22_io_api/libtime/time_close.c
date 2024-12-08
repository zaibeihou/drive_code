#include "lib.h"
#include <stdio.h>

int time_close(int fd)
{                             
    int ret;
    ret = ioctl(fd,CMD_TIME_CLOSE);
    if(ret){
        perror("ioctl close error!");
        exit(-2);    
    }
    return 0;
}