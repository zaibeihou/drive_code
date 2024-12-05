#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <poll.h>

int main(){
    char buf [32] = {0};
    int fd = open("/dev/test0",O_RDWR);
    
    struct pollfd fds[1];
    
    fds[0].fd = fd;
    fds[0].events = POLLIN;

    while(1){
        poll(fds,1,-1);
        if(fds[0].revents == POLLIN){
            read(fd,buf,sizeof(buf));
            printf("read : %s\n",buf);
        }
    }

    close(fd);
    return 0;
}