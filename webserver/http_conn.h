#ifndef HTTPCONNECTION_H
#define HTTPCONNECTION_H

#include <sys/epoll.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdarg.h>
#include "lock.h"
#include <sys/uio.h>

class http_conn {
public:

    static int m_epollfd; // 所有的socket上的事件都被注册到同一个EPOLL
    static int m_user_count; // 统计用户的数量
    static const int READ_BUFFER_SIZE = 2048; // 读缓冲区大小
    static const int WRITE_BUFFER_SIZE = 1024; // 写缓冲区大小

    http_conn() {}
    ~http_conn() {}

    void process(); // 处理客户端请求
    void init(int sockfd, const sockaddr_in &addr); // 初始化新接收的连接
    void close_conn(); // 关闭连接
    bool read(); // 非阻塞读
    bool write(); // 非阻塞写
private:

    int m_sockfd; // 该HTTP连接的socket；
    sockaddr_in m_address; // 通信的socket的地址
    char m_read_buf[READ_BUFFER_SIZE]; // 读缓冲区
    int m_read_idx; // 读缓冲区中以及读入客户端数据的最后一个字节的大小

};












#endif