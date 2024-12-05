#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(){
    char buf [32] = {0};
    int fd = open("/dev/test0",O_RDWR);
    
    read(fd,buf,sizeof(buf));
    printf("read : %s\n",buf);
    
    close(fd);
    return 0;
}