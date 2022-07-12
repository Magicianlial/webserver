#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <wait.h>
#include <errno.h>

void recleChild(int arg) {
    while(1) {
        int ret = waitpid(-1, NULL, WNOHANG);
        if(ret == -1) {
            break; //done
        } else if(ret == 0) {
            break; //still have
        } else {
            printf("子进程pid: %d 回收了\n", ret);
        }
    }
}

int main() {

    // crate signal(waitpid)
    struct sigaction act;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    act.sa_handler = recleChild;
    sigaction(SIGCHLD, &act, NULL);


    // create socket
    int lfd = socket(PF_INET, SOCK_STREAM, 0);
    if(lfd == -1) {
        perror("socket");
        exit(-1);
    }

    // port multiplexing
    int optval = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));

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

    // continue accept
    while(1) {
        // accept a client
        struct sockaddr_in cliaddr;
        int len = sizeof(cliaddr);
        int cfd = accept(lfd, (struct sockaddr *)&cliaddr, &len);
        if(cfd == -1) {
            if(errno == EINTR) continue;
            perror("accept");
            exit(-1);
        }

        // create process to communicate 
        pid_t pid = fork();
        if(pid == 0) {
            // subprocess
            // get client infor
            char cliIp[16];
            inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, cliIp, sizeof(cliIp));
            unsigned short cliPort = ntohs(cliaddr.sin_port);
            printf("client ip is %s, port is %d\n", cliIp, cliPort);

            // get client data
            char recvBuf[1024] = {0};
            while(1) {
                int len = read(cfd, &recvBuf, sizeof(recvBuf));
                if(len == -1) {
                    perror("read");
                    exit(-1);
                } else if(len > 0) {
                    printf("recv client data : %s\n", recvBuf);
                } else if(len == 0){
                    printf("client closed ...\n");
                    break;
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