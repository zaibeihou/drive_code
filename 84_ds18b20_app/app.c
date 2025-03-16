#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

void ds18b20_temp_convert(int temp)
{
    if(temp >> 11 & 0x01){
        temp = ~temp + 1;
        printf("temp = -%d.%d\n", temp >> 4, (temp & 0x0f) * 625);
    }
    else{
        printf("temp = %d.%d\n", temp >> 4, (temp & 0x0f) * 625);
    }
}

int main()
{
    int fd;
    int temp;

    fd = open("/dev/ds18b20", O_RDWR);
    if(fd < 0){
        printf("open /dev/ds18b20 failed\n");
        return -1;
    }

    while(1){
        read(fd, &temp, sizeof(temp));
        ds18b20_temp_convert(temp);
        usleep(500);
    }

    return 0;
}