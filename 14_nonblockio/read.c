#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(){
    char buf [32] = {0};
    int fd = open("/dev/test0",O_RDWR|O_NONBLOCK);
    int n;
    while( (n =read(fd,buf,sizeof(buf))) < 0 ){
        printf("read : \n");
        sleep(1);
    }
    
    printf("read : %s\n",buf);
    
    close(fd);
    return 0;
}