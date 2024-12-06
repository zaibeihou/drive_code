#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
    long counter;
    int fd = open("/dev/test",O_RDWR);
    int fd0 = open("/dev/test0",O_RDWR);
    while(1){
        read(fd0,&counter,sizeof(counter));//使用 read 函数读取内核传递来的秒数
        char b = (char)counter;
        sleep(1);
        printf("num is %d\n",counter);
        write(fd, &b,sizeof(b));
    }
    

    close(fd);
    return 0;
}