#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <wait.h>
#include <errno.h>
#include <poll.h>

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

    // init fd_sets
    struct pollfd fds[1024];
    for(int i = 0; i < 1024; i++) {
        fds[i].fd = -1;
        fds[i].events = POLLIN;
    }
    fds[0].fd = lfd;
    int nfds = 0;

    while(1) {
        //poll
        ret = poll(fds, nfds + 1, -1);

        if(ret == -1) {
            perror("poll");
            exit(-1);
        } else if(ret == 0) {
            continue;
        } else if(ret > 0) {
            // some fds change
            if(fds[0].revents & POLLIN) {
                // new client come
                struct sockaddr_in cliaddr;
                int len = sizeof(cliaddr);
                int cfd = accept(lfd, (struct sockaddr *)&cliaddr, &len);

                // add cfd to fd_set
                for(int i = 1; i < 1024; i++) {
                    if(fds[i].fd == -1) {
                        fds[i].fd = cfd;
                        fds[i].events = POLLIN;
                        break;
                    }
                }
                nfds = nfds > cfd ? nfds : cfd;
            }

            for(int i = 1; i <= nfds; i++) {
                if(fds[i].revents & POLLIN) {
                    // fd:i hava data
                    char buf[1024] = {0};
                    int rlen = read(fds[i].fd, buf, sizeof(buf));
                    if(rlen == -1) {
                        perror("read");
                        exit(-1);
                    } else if(rlen == 0) {
                        printf("client closed ...\n");
                        close(fds[i].fd);
                        fds[i].fd = -1;
                    } else if(rlen > 0) {
                        printf("recv buf is : %s\n", buf);
                        write(fds[i].fd, buf, strlen(buf) + 1);
                    }
                }
            }
        }
    }

    close(lfd);

    return 0;
}