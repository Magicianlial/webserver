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

    // server addr
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(8008);
    inet_pton(AF_INET, "127.0.0.1", &saddr.sin_addr.s_addr);


    int num = 0;
    // comunicate
    while(1) {

        char sendBuf[128];
        sprintf(sendBuf, "hellp, i am client %d\n", num++);

        // send data
        sendto(fd, sendBuf, strlen(sendBuf) + 1, 0, (struct sockaddr *)&saddr, sizeof(saddr));

        //recv data
        int num = recvfrom(fd, sendBuf, sizeof(sendBuf), 0, NULL, NULL);
        if(num == -1) {
            perror("recvfrom");
            exit(-1);
        }
        printf("server say : %s\n", sendBuf);

        sleep(1);
    }

    close(fd);
    return 0;
}