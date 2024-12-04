#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(){
    char buf0 [32] = "hello test!";
    int fd0 = open("/dev/atomic_test0",O_RDWR);
    int fd1 = open("/dev/atomic_test1",O_RDWR);
    
    write(fd0, buf0,strlen(buf0));
    write(fd1, buf0,strlen(buf0));

    sleep(5);
    close(fd0);
    close(fd1);
    return 0;
}