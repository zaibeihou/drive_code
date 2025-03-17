#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>
#include <unistd.h>


int main(int argc, char *argv[])
{
    struct input_event event;
    
    int fd;
    fd = open("/dev/input/event4",O_RDWR);
    if(fd < 0){
        perror("open");
        exit(1);
    }
    while(1){
        read(fd,&event,sizeof(event));
        if(event.type == EV_ABS){
            if(event.code == ABS_X){
                printf("x = %d\n",event.value);
            }else if(event.code == ABS_Y){
                printf("y = %d\n",event.value);
            }
        }
    }


    return 0;
}