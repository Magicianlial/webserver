#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main() {
    // create socket
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    // connect
    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    inet_pton(AF_INET, "172.27.130.132", &serveraddr.sin_addr.s_addr);
    serveraddr.sin_port = htons(8008);
    int ret = connect(fd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    if(ret == -1) {
        perror("connect");
        exit(-1);
    }

    // comunicate
    int num = 0;
    while(1) {
        // write
        char sendBuf[1024] = {0};
        sprintf(sendBuf, "send data %d", num++);
        write(fd, sendBuf, strlen(sendBuf) + 1);
        
        // read
        
        int len = read(fd, sendBuf, sizeof(sendBuf));
        if(len == -1) {
            perror("read");
            exit(-1);
        } else if(len > 0) {
            printf("recv server data : %s\n", sendBuf);
        } else if(len == 0) {
            printf("server closed...\n");
            break;
        }

        usleep(1000);
    }

    // close
    close(fd);

    return 0;
}