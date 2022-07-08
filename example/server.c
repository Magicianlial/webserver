#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

int main() {
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if(lfd == -1) {
        perror("socket");
        exit(-1);
    }

    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    //inet_pton(AF_INET, "192.168.1.1", saddr.sin_addr.s_addr);
    saddr.sin_addr.s_addr = INADDR_ANY;
    saddr.sin_port = htons(8008);
    int ret = bind(lfd, (struct sockeaddr *)&saddr, sizeof(saddr));
    if(ret == -1) {
        perror("bind");
        exit(-1);
    }

    ret = listen(lfd, 8);
    if(ret == -1) {
        perror("listen");
        exit(-1);
    }

    struct sockaddr_in clientaddr;
    socklen_t len = sizeof(clientaddr);
    int cfd = accept(lfd, (struct sockeaddr *) &clientaddr, &len);
    if(cfd == -1) {
        perror("accept");
        exit(-1);
    }

    char clientIP[16];
    inet_ntop(AF_INET, &clientaddr.sin_addr.s_addr, clientIP, sizeof(clientIP));
    unsigned short clientPort = ntohs(clientaddr.sin_port);

    printf("client ip is %s, port is %d", clientIP, clientPort);

    char recvBuf[1024] = {0};
    int len = read(cfd, recvBuf, sizeof(recvBuf));
    if(len == -1) {
        perror("read");
        exit(-1);
    } else if(len > 0) {
        printf("recv client data : %d\n", recvBuf);
    } else if(len == 0) {
        printf("client closed...\n");
    }

    char *data = "hello, i am server";
    write(cfd, "hello, i am server", strlen(data));

    close(cfd);
    close(lfd);

    return 0;
}