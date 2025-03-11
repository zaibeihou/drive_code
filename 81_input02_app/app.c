#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>
#include <unistd.h>

int main(){
    int fd;
    int ret;
    struct input_event event;

    fd = open("/dev/input/event4",O_RDWR);
    if(fd < 0){
        printf("open failed\n");
        return -1;
    }

    while(1){
        ret = read(fd,&event,sizeof(event));
        if(ret == sizeof(event)){
            printf("type:%d,code:%d,value:%d\n",event.type,event.code,event.value);
        }

        if(event.type == EV_KEY){
            if(event.code == KEY_1){
                if(event.value == 0){
                    printf("key 1 up\n");
                }else if(event.value == 1){
                    printf("key 1 down\n");
                }
            }
        }
    }
    close(fd);

}
