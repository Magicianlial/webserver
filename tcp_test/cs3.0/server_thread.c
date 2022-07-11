#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

struct sockInfo {
    int fd; //通信文件描述符
    pthread_t tid; //线程号
    struct sockaddr_in addr;

};

struct sockInfo sockinfos[128];

void *working(void *arg) {
    // pthread comunicate with client cfd, clien, infor, threadId
    // get client infor
    struct sockInfo * pinfo = (struct sockInfo *)arg;
    char cliIp[16];
    inet_ntop(AF_INET, &pinfo->addr.sin_addr.s_addr, cliIp, sizeof(cliIp));
    unsigned short cliPort = ntohs(pinfo->addr.sin_port);
    printf("client ip is %s, port is %d\n", cliIp, cliPort);

    // accept client data
    char recvBuf[1024] = {0};
    while(1) {
        int len = read(pinfo->fd, &recvBuf, sizeof(recvBuf));
        if(len == -1) {
            perror("read");
            exit(-1);
        } else if(len > 0) {
            printf("recv client data : %s\n", recvBuf);
        } else if(len == 0){
            printf("client closed ...\n");
            break;
        }

        write(pinfo->fd, recvBuf, strlen(recvBuf) + 1);
    }
    close(pinfo->fd);

    return NULL;
}

int main() {

    // create socket
    int lfd = socket(PF_INET, SOCK_STREAM, 0);
    if(lfd == -1) {
        perror("socket");
        exit(-1);
    }

    // bind
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(8008);
    saddr.sin_addr.s_addr = INADDR_ANY;
    int ret = bind(lfd, (struct sockaddr *)&saddr, sizeof(saddr));
    if(ret == -1) {
        perror("bind");
        exit(-1);
    }
    
    // listen
    ret = listen(lfd, 128);
    if(ret == -1) {
        perror("listen");
        exit(-1);
    }

    // init sockinfos data
    int maxPthread = sizeof(sockinfos) / sizeof(sockinfos[0]);
    for(int i = 0; i < maxPthread; ++i) {
        bzero(&sockinfos[i], sizeof(sockinfos[i]));
        sockinfos[i].fd = -1;
        sockinfos[i].tid = -1;
    }

    // loop wait client connect, create a thread
    while(1) {
        // accept
        struct sockaddr cliaddr;
        int len = sizeof(cliaddr);
        int cfd = accept(lfd, (struct sockaddr*)&cliaddr, &len);
        if(cfd == -1) {
            perror("accept");
            exit(-1);
        }

        // create a thread
        struct sockInfo *pinfo;
        for(int i = 0; i < maxPthread; ++i) {
            // choose a valid sockInfo
            if(sockinfos[i].fd == -1) {
                pinfo = &sockinfos[i];
                break;
            }
            if(i == maxPthread - 1) {
                sleep(1);
                i = 0;
            }
        }
        pinfo->fd = cfd;
        memcpy(&pinfo->addr, &cliaddr, len);

        pthread_create(&pinfo->tid, NULL, working, pinfo);
        pthread_detach(pinfo->tid);

    }

    close(lfd);
    return 0;
}