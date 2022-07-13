#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>

int main() {
    // create socket
    int lfd = socket(PF_INET, SOCK_STREAM, 0);
    if(lfd == -1) {
        perror("socket");
        exit(-1);
    }
    struct sockaddr_in saddr;
    saddr.sin_port = htons(8008);
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;

    // bind
    int ret = bind(lfd, (struct sockaddr *)&saddr, sizeof(saddr));
    if(ret == -1) {
        perror("bind");
        exit(-1);
    }

    // listen
    ret = listen(lfd, 8);
    if(ret == -1) {
        perror("listen");
        exit(-1);
    }

    // epoll_create
    int epfd = epoll_create(100);

    // add listen_fd to epoll
    struct epoll_event epev;
    epev.events = EPOLLIN;
    epev.data.fd = lfd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &epev);

    // epoll_wait 
    struct epoll_event epevs[1024];
    while(1) {
        ret = epoll_wait(epfd, epevs, 1024, -1);
        if(ret == -1) {
            perror("epoll_wait");
            exit(-1);
        }

        printf("ret == %d\n", ret);

        for(int i = 0; i < ret; i++) {
            int curfd = epevs[i].data.fd;

            if(curfd == lfd) {
                // new client come
                struct sockaddr_in cliaddr;
                int len = sizeof(cliaddr);
                int cfd = accept(lfd, (struct sockaddr *)&cliaddr, &len);

                epev.events = EPOLLIN | EPOLLOUT;
                epev.data.fd = cfd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &epev);
            } else {
                if(epevs[i].events & EPOLLOUT) {
                    continue;
                }
                // recv data
                char buf[1024] = {0};
                int rlen = read(curfd, buf, sizeof(buf));
                if(rlen == -1) {
                    perror("read");
                    exit(-1);
                } else if(rlen == 0) {
                    printf("client closed ...\n");
                    epoll_ctl(epfd, EPOLL_CTL_DEL, curfd, NULL);
                    close(curfd);
                } else if(rlen > 0) {
                    printf("recv buf is : %s\n", buf);
                    write(curfd, buf, strlen(buf) + 1);
                }
            }
        }

    }

    close(lfd);
    close(epfd);

    return 0;
}