#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

int main() {

    // create socket
    int fd = socket(PF_INET, SOCK_DGRAM, 0);
    if(fd == -1) {
        perror("socket");
        exit(-1);
    }

    // set broadcast
    int op = 1;
    setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &op, sizeof(op));

    // create broadcast addr
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8008);
    inet_pton(AF_INET, "172.27.143.255", &addr.sin_addr.s_addr);



    // communicate
    int sendNum = 0;
    while(1) {

        char sendBuf[1024] = {0};

        sprintf(sendBuf, "hello client ...%d\n", sendNum++);

        //send data
        sendto(fd, sendBuf, strlen(sendBuf), 0, (struct sockaddr *)&addr, sizeof(addr));
        printf("广播的数据: %s\n", sendBuf);
        sleep(1);
    }

    close(fd);
    return 0;
}