#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <unistd.h>


int main(int argc, char *argv[])
{
    int fd;
    struct sockaddr_nl sa;
    char buffer[4096];
    int len = 0;
    int i;

    fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_KOBJECT_UEVENT);
    if (fd == -1) {
        perror("socket");
        return 1;
    }

    memset(&sa, 0, sizeof(sa));
    sa.nl_family = AF_NETLINK;
    sa.nl_groups = 1;
    sa.nl_pid = 0;

    if (bind(fd, (struct sockaddr *)&sa, sizeof(sa)) == -1) {
        perror("bind");
        return 1;
    }
    while(1){
        memset(buffer, 0, sizeof(buffer));
        len = recv(fd, buffer, sizeof(buffer), 0);
        for(i = 0; i < len; i++){
            if(buffer[i] == '\0'){
                buffer[i] = '\n';
            }
        }
        printf("%s\n", buffer);
    }
    close(fd);
    return 0;
}

    
