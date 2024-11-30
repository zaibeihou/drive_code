#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(){
    char buf [32] = {0};
    int fd = open("/dev/test",O_RDWR);
    if(fd < 0){
        perror("open file error");
        return -1;
    }

    read(fd, buf,sizeof(buf));
    printf("%s\n", buf);

    write(fd, buf, strlen(buf));

    close(fd);
    return 0;
}