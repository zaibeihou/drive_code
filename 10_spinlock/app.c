#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(){
    char buf0 [32] = "hello test!";
    int fd0 = open("/dev/test0",O_RDWR);

    sleep(5);
    close(fd0);
    return 0;
}