#ifndef COND_H_
#define COND_H_

#include <pthread.h>

#include <exception>

// 封装条件变量的类
class Cond {
 public:
  // 创建并初始化条件变量
  Cond();
  // 销毁条件变量
  ~Cond();
  // 等待条件变量
  bool wait();
  // 唤醒等待条件变量的线程
  bool signal();

 private:
  pthread_mutex_t m_mutex;
  pthread_cond_t m_cond;
};

#endif