#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

int main() {
    //create socket
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    //connect
    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    inet_pton(AF_INET, "192.158.193.128", &serveraddr.sin_addr.s_addr);
    serveraddr.sin_port = htons(8008);
    int ret = connect(fd, (struct sockeaddr *)&serveraddr, sizeof(serveraddr));
    if(ret == -1) {
        perror("connect");
        exit(-1);
    }

    //write
    char *data = "hello, i am client";
    write(fd, data, strlen(data));

    //read
    read()

    return 0;
}