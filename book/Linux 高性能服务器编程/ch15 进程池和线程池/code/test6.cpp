#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cassert>

#include "http_conn.h"
#include "locker.h"
#include "threadpool.h"

#define MAX_FD 65536
#define MAX_EVENT_NUMBER 10000

extern int addfd(int epollfd, int fd, bool one_shot);
extern int removefd(int epollfd, int fd);

void addsig(int sig, void(handler)(int), bool restart = true) {
  struct sigaction sa;
  memset(&sa, '\0', sizeof(sa));
  sa.sa_handler = handler;

  if (restart) sa.sa_flags |= SA_RESTART;

  sigfillset(&sa.sa_mask);
  assert(sigaction(sig, &sa, NULL) != -1);
}

void show_error(int connfd, const char *info) {
  printf("%s", info);
  send(connfd, info, strlen(info), 0);
  close(connfd);
}

int main(int argc, char *argv[]) {
  if (argc <= 2) {
    printf("usage: %s ip_address port_number.\n", basename(argv[0]));
    return 1;
  }

  const char *ip = argv[1];
  int port = atoi(argv[2]);

  // 忽略 SIGPIPE 信号
  addsig(SIGPIPE, SIG_IGN);

  // 创建线程池
  ThreadPool<HttpConn> *pool = NULL;
  try {
    pool = new ThreadPool<HttpConn>;
  } catch (...) {
    return 1;
  }

  // 预先为每个可能的客户连接分配一个 HttpConn 请求
  

  return 0;
}