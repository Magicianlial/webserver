#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <pthread.h>

// 线程池类，定义成模板类（代码复用）
template<typename T>
class threadpool {
public:

private:
    // 线程数量
    int m_thread_number;

    // 线程池数组
    pthread_t *m_threads;

    // 请求队列中最大允许等待处理请求数量
    int m_max_requests;

    //请求队列
}













#endif