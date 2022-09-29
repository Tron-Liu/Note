#ifndef LOCKER_H
#define LOCKER_H

#include <pthread.h>

#include <exception>

class Locker {
 public:
  // 创建并初始化互斥锁
  Locker() {
    if (pthread_mutex_init(&m_mutex, NULL) != 0) {
      throw std::exception();
    }
  }
  // 销毁互斥锁
  ~Locker() { pthread_mutex_destroy(&m_mutex); }
  // 获取互斥锁
  bool lock() { return pthread_mutex_lock(&m_mutex) == 0; }
  // 释放互斥锁
  bool unlock() { return pthread_mutex_unlock(&m_mutex) == 0; }

 private:
  pthread_mutex_t m_mutex;
};

#endif