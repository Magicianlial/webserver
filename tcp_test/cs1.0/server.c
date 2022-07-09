#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main() {
    //create socket
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if(lfd == -1) {
        perror("socket");
        exit(-1);
    }

    //bind
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    //inet_pton(AF_INET, "172.27.130.132", saddr.sin_addr.s_addr);
    saddr.sin_addr.s_addr = INADDR_ANY;
    saddr.sin_port = htons(8008);
    int ret = bind(lfd, (struct sockaddr *)&saddr, sizeof(saddr));
    if(ret == -1) {
        perror("bind");
        exit(-1);
    }

    //listen
    ret = listen(lfd, 8);
    if(ret == -1) {
        perror("listen");
        exit(-1);
    }

    //accept
    struct sockaddr_in clientaddr;
    int len = sizeof(clientaddr);
    int cfd = accept(lfd, (struct sockaddr *) &clientaddr, &len);
    if(cfd == -1) {
        perror("accept");
        exit(-1);
    }

    //client ip port setting
    char clientIP[16];
    inet_ntop(AF_INET, &clientaddr.sin_addr.s_addr, clientIP, sizeof(clientIP));
    unsigned short clientPort = ntohs(clientaddr.sin_port);

    printf("client ip is %s, port is %d", clientIP, clientPort);

    //communicate
    char recvBuf[1024] = {0};
    while(1) {
        // read
        
        int rlen = read(cfd, recvBuf, sizeof(recvBuf));
        if(rlen == -1) {
            perror("read");
            exit(-1);
        } else if(rlen > 0) {
            printf("recv client data : %s\n", recvBuf);
        } else if(rlen == 0) {
            printf("client closed...\n");
            break;
        }

        //write
        char *data = "hello, i am server";
        write(cfd, data, strlen(data));
    }

    //close
    close(cfd);
    close(lfd);

    return 0;
}