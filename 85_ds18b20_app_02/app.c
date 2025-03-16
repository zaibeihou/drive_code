#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdlib.h>

#define SET_RESOLUTION _IOW('A', 1, int)  // 参数1：命令的类型，参数2：命令的序号，参数3：命令的参数类型
#define GET_RESOLUTION _IOR('A', 2, int)  // 建议使用不同的命令序号

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

int main(int argc, char *argv[])
{
    int fd;
    int temp;
    int resolution;

    if(argc != 2){
        printf("Usage: %s <resolution>\n", argv[0]);
        return -1;
    }
    
    resolution = atoi(argv[1]);
    
    if(resolution < 9 || resolution > 12){
        printf("resolution must be 9, 10, 11 or 12\n");
        return -1;
    }

    fd = open("/dev/ds18b20", O_RDWR);
    if(fd < 0){
        printf("open /dev/ds18b20 failed\n");
        return -1;
    }

    ioctl(fd, SET_RESOLUTION, resolution);

    ioctl(fd, GET_RESOLUTION, &resolution);
    printf("resolution = %d\n", resolution);

    while(1){
        read(fd, &temp, sizeof(temp));
        ds18b20_temp_convert(temp);
        usleep(500);
    }

    return 0;
}