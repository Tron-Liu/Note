#include "sem.h"

Sem::Sem() {
  if (sem_init(&m_sem, 0, 0) != 0) {
    // 构造函数没有返回值，可以通过抛出异常来报告错误
    throw std::exception();
  }
}

Sem::~Sem() { sem_destroy(&m_sem); }

bool Sem::wait() { return sem_wait(&m_sem) == 0; }

bool Sem::post() { return sem_post(&m_sem) == 0; }
