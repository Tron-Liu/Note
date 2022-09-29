#ifndef SEM_H
#define SEM_H

#include <semaphore.h>

#include <exception>

// 封装信号量的类
class Sem {
 public:
  // 创建并初始化信号量
  Sem();
  // 销毁信号量
  ~Sem();
  // 等待信号量
  bool wait();
  // 增加信号量
  bool post();

 private:
  sem_t m_sem;
};

#endif