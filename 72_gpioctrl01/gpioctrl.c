#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

char gpio_path[50];

int gpio_export(char *argv)
{
    int fd;
    int len;
    int ret;
    fd = open("/sys/class/gpio/export", O_WRONLY);
    if (fd < 0)
    {
        printf("Error opening file\n");
        return -1;
    }
    len = strlen(argv);
    ret = write(fd, argv, len);
    if (ret < 0)
    {
        printf("Error writing to file\n");
        return -1;
    }
    close(fd);
    return 0;
}

int gpio_unexport(char *argv)
{
    int fd;
    int len;
    int ret;
    fd = open("/sys/class/gpio/unexport", O_WRONLY);
    if (fd < 0)
    {
        printf("Error opening file\n");
        return -1;
    }
    len = strlen(argv);
    ret = write(fd, argv, len);
    if (ret < 0)
    {
        printf("Error writing to file\n");
        return -1;
    }
    close(fd);
    return 0;
}

int gpio_ctrl(char *argv, char *value)
{
    int fd;
    int len;
    int ret;
    char file_path[50];
    sprintf(file_path, "%s/%s",gpio_path,argv);
    fd = open(file_path, O_WRONLY);
    if (fd < 0)
    {
        printf("Error opening file\n");
        return -1;
    }
    len = strlen(value);
    ret = write(fd, value, len);
    if (ret < 0)
    {
        printf("Error writing to file\n");
        return -1;
    }
    close(fd);
    return 0;
}

int main(int argc, char *argv[])
{
    sprintf(gpio_path, "/sys/class/gpio/gpio%s", argv[1]);
    if (access(gpio_path, F_OK)){
        gpio_export(argv[1]);
    }
    gpio_ctrl("direction", "out");
    gpio_ctrl("value", argv[2]);   
    gpio_unexport(argv[1]);
    return 0;
}