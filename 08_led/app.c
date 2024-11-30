#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
    char buf[32] = {0};
    int fd = open("/dev/test",O_RDWR);
    
    buf[0] = atoi(argv[1]);
    write(fd, buf,sizeof(buf));

    close(fd);
    return 0;
}