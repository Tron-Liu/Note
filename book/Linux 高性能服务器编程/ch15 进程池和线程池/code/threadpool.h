#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <pthread.h>

#include <cstdio>
#include <list>

#include "locker.h"
#include "sem.h"

template <typename T> class ThreadPool {
public:
  ThreadPool(int thread_number = 8, int max_requests = 10000);
  ~ThreadPool();
  // 往请求队列中添加任务
  bool append(T *request);

private:
  // 工作线程运行的函数，它不断从工作队列中取出任务并执行
  static void *worker(void *arg);
  void run();

private:
  int m_thread_number; // 线程池中的线程数
  int m_max_requests;  // 请求队列中允许的最大请求数
  pthread_t *m_threads; // 描述线程池的数组，其大小为 m_thread_number
  std::list<T *> m_workqueue; // 请求队列
  Locker m_queuelocker;       // 保护请求队列的互斥锁
  Sem m_queuestat;            // 是否有任务需要处理
  bool m_stop;                // 是否结束线程
};

#endif