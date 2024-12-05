#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include<fcntl.h>
#include<signal.h>

char buf [32] = {0};
int fd;

void func(){
    read(fd,buf,sizeof(buf));
    printf("read : %s\n",buf);
}

int main(){
    int flags;
    fd= open("/dev/test0",O_RDWR);
    
    signal(SIGIO,func);
    fcntl(fd,F_SETOWN,getpid());
    
    flags=fcntl(fd,F_GETFD);
    flags = flags | FASYNC;
    fcntl(fd, F_SETFL, flags);
    while (1){
        sleep(1);
    }

    
    close(fd);
    return 0;
}