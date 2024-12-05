#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(){
    char buf [32] = "hello read!";
    int fd = open("/dev/test0",O_RDWR);
    
    write(fd,buf,strlen(buf));
    printf("write : %s\n",buf);
    
    close(fd);
    return 0;
}