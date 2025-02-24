#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

#define GPIO_BASE 0xFDD60000
#define GPIO_SWPORTA_DDR_L_OFFSET 0x0008 //方向寄存器
#define GPIO_SWPORTA_DR_L_OFFSET 0x0000 //数据寄存器

void led_on(unsigned char *mmap_base)
{
    *(volatile unsigned int *)(mmap_base + GPIO_SWPORTA_DDR_L_OFFSET) = 0x80008044;
    *(volatile unsigned int *)(mmap_base + GPIO_SWPORTA_DR_L_OFFSET) = 0x80008040;
}

void led_off(unsigned char *mmap_base)
{
    *(volatile unsigned int *)(mmap_base + GPIO_SWPORTA_DDR_L_OFFSET) = 0x80008044;
    *(volatile unsigned int *)(mmap_base + GPIO_SWPORTA_DR_L_OFFSET) = 0x80000040;
}
int main(int argc, char *argv[])
{
    int fd;
    unsigned char *mmap_base;

    fd = open("/dev/mem", O_RDWR);
    if (fd < 0)
    {
        perror("open");
        return -1;
    }

    mmap_base = (unsigned char *)mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIO_BASE);
    if (mmap_base == MAP_FAILED)
    {
        perror("mmap");
        return -1;
    }
    while(1){
        led_on(mmap_base);
        sleep(1);
        led_off(mmap_base);
        sleep(1);
    }

    return 0; 
}