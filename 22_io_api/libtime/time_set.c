#include "lib.h"
#include <stdio.h>

int time_set(int fd,int time)
{                             
    int ret;
    ret = ioctl(fd,CMD_TIME_SET,time);
    if(ret){
        perror("ioctl set error!");
        exit(-3);    
    }
    return 0;
}