/**
 * @file test5.cpp
 * @brief 用一个线程处理所有信号
 * @date 2024-04-05
 */

#include <bits/types/sigset_t.h>
#include <cstddef>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define handle_error_en(en, msg)                                               \
  do {                                                                         \
    errno = en;                                                                \
    perror(msg);                                                               \
    exit(EXIT_FAILURE);                                                        \
  } while (0)

static void *sig_thread(void *arg) {
  sigset_t *set = (sigset_t *)arg;
  int s, sig;

  for (;;) {
    // 调用 sigwait 等待信号
    s = sigwait(set, &sig);
    if (s != 0)
      handle_error_en(s, "sigwait");
    printf("Signal handling thread got signal %d\n", sig);
  }
}

int main(int argc, char* arg[])
{
  pthread_t thread;
  sigset_t set;
  int s;

  // 主进程中设置信号掩码
  sigemptyset(&set);
  sigaddset(&set, SIGQUIT);
  sigaddset(&set, SIGUSR1);

  s = pthread_sigmask(SIG_BLOCK, &set, NULL);
  if (s != 0)
    handle_error_en(s, "pthread_sigmask");
  
  s = pthread_create(&thread, NULL, &sig_thread, (void*)&set);
  if (s != 0)
    handle_error_en(s, "pthread_create");

  pause();
}