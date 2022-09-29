#include "cond.h"

Cond::Cond() {
  if (pthread_mutex_init(&m_mutex, NULL) != 0) {
    throw std::exception();
  }
  if (pthread_cond_init(&m_cond, NULL) != 0) {
    // 构造函数中一旦出现问题，就应该立即释放已经成功分配了的资源
    pthread_mutex_destroy(&m_mutex);
    throw std::exception();
  }
}

Cond::~Cond() {
  pthread_mutex_destroy(&m_mutex);
  pthread_cond_destroy(&m_cond);
}

bool Cond::wait() {
  int ret = 0;
  pthread_mutex_lock(&m_mutex);
  ret = pthread_cond_wait(&m_cond, &m_mutex);
  pthread_mutex_unlock(&m_mutex);
  return ret == 0;
}

bool Cond::signal() { return pthread_cond_signal(&m_cond) == 0; }
