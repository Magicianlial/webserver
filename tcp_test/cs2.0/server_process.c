#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main() {

    //create socket
    int lfd = socket(PF_INET, SOCK_STREAM, 0);
    if(lfd == -1) {
        perror("socket");
        exit(-1);
    }

    //bind
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(8008);
    saddr.sin_addr.s_addr = INADDR_ANY;
    int ret = bind(lfd, (struct sockaddr *)&saddr, sizeof(saddr));
    if(ret == -1) {
        perror("bind");
        exit(-1);
    }

    //listen
    ret = listen(lfd, 128);
    if(ret == -1) {
        perror("listen");
        exit(-1);
    }

    //continue accept
    while(1) {
        //accept a clinet
        struct sockaddr_in cliaddr;
        int len = sizeof(cliaddr);
        int cfd = accept(lfd, (struct sockaddr *)&cliaddr, &len);
        if(cfd == -1) {
            perror("accept");
            exit(-1);
        }

        //create process to communicate 
        pid_t pid = fork();
        if(pid == 0) {
            //subprocess
            //get client infor
            char cliIp[16];
            inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, cliIp, sizeof(cliIp));
            unsigned short cliPort = ntohs(cliaddr.sin_port);
            printf("client ip is %s, port is %d\n", cliIp, cliPort);

            //accept clinet data
            char recvBuf[1024] = {0};
            while(1) {
                int len = read(cfd, &recvBuf, sizeof(recvBuf));
                if(len == -1) {
                    perror("read");
                    exit(-1);
                } else if(len > 0) {
                    printf("recv client data : %s\n", recvBuf);
                } else {
                    printf("client closed ...");
                }

                write(cfd, recvBuf, strlen(recvBuf) + 1);
            }
            close(cfd);
            exit(0);
        }
    }

    close(lfd);
    return 0;
}