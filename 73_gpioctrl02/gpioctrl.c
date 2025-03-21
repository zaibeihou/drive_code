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

int gpio_read(char *argv)
{
    int fd;
    int ret;
    char value[2];
    char file_path[50];
    sprintf(file_path, "%s/value",gpio_path);
    fd = open(file_path, O_RDONLY);
    if (fd < 0)
    {
        printf("Error opening file\n");
        return -1;
    }
    ret = read(fd, value, 1);
    if (ret < 0)
    {
        printf("Error reading from file\n");
        return -1;
    }
    close(fd);
    return atoi(value);
}

int main(int argc, char *argv[])
{
    sprintf(gpio_path, "/sys/class/gpio/gpio%s", argv[1]);
    if (access(gpio_path, F_OK)){
        gpio_export(argv[1]);
    }
    gpio_ctrl("direction", "in");
    printf("Value: %d\n", gpio_read(argv[1]));
    gpio_unexport(argv[1]);
    return 0;
}