#ifndef _TIMELIB_H_
#define _TIMELIB_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>

#define CMD_TIME_CLOSE _IO('A',1)
#define CMD_TIME_OPEN _IO('A',2)
#define CMD_TIME_SET _IOW('A',2,int)

int time_close(int fd);
int time_set(int fd,int time);
int time_open(int fd);

#endif