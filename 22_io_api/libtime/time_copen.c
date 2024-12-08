#include "lib.h"
#include <stdio.h>

int time_open(int fd)
{                             
    int ret;
    ret = ioctl(fd,CMD_TIME_OPEN);
    if(ret){
        perror("ioctl open error!");
        exit(-1);    
    }
    return 0;
}