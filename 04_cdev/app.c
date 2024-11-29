#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(){
    int fd = open("/dev/test",O_RDWR);
    if(fd < 0){
        perror("open file error");
        return -1;
    }
    close(fd);
    return 0;
}