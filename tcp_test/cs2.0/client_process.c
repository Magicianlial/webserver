#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main() {
    //create socket
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    //connect
    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    inet_pton(AF_INET, "172.27.130.132", &serveraddr.sin_addr.s_addr);
    serveraddr.sin_port = htons(8008);
    int ret = connect(fd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    if(ret == -1) {
        perror("connect");
        exit(-1);
    }

    //comunicate
    char recvBuf[1024] = {0};
    int i = 0;
    while(1) {
        //write
        sprintf(recvBuf, "data : %d\n", i++);
        write(fd, recvBuf, strlen(recvBuf) + 1);
        sleep(1);
        // read
        
        int len = read(fd, recvBuf, sizeof(recvBuf));
        if(len == -1) {
            perror("read");
            exit(-1);
        } else if(len > 0) {
            printf("recv server data : %s\n", recvBuf);
        } else if(len == 0) {
            printf("server closed...\n");
            break;
        }
    }

    //close
    close(fd);

    return 0;
}