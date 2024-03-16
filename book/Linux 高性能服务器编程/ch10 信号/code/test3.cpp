/**
 * @file test3.cpp
 * @brief 用 SIGURG 检测带外数据是否达到
 * @version 0.1
 * @date 2022-10-02
 */

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUF_SIZE 1024

static int connfd;

// SIGURG 信号的处理函数
void sig_urg(int sig) {
  int save_errno = errno;

  char buffer[BUF_SIZE];
  memset(buffer, '\0', BUF_SIZE);

  // 接收带外数据
  int ret = recv(connfd, buffer, BUF_SIZE - 1, MSG_OOB);
  printf("got %d bytes of oob data '%s'. \n", ret, buffer);

  errno = save_errno;
}

void addsig(int sig, void (*sig_handler)(int)) {
  struct sigaction sa;
  memset(&sa, '\0', sizeof(sa));

  sa.sa_handler = sig_handler;
  sa.sa_flags |= SA_RESTART;
  sigfillset(&sa.sa_mask);

  assert(sigaction(sig, &sa, NULL) != -1);
}

int main(int argc, char* argv[]) {
  if (argc <= 2) {
    printf("usage: %s ip_address port_number\n", basename(argv[0]));
    return 1;
  }

  const char* ip = argv[1];
  int port = atoi(argv[2]);

  int ret = 0;
  struct sockaddr_in address;
  bzero(&address, sizeof(address));
  address.sin_family = AF_INET;

  inet_pton(AF_INET, ip, &address.sin_addr);
  address.sin_port = htons(port);

  int listenfd = socket(PF_INET, SOCK_STREAM, 0);
  assert(listenfd >= 0);

  ret = bind(listenfd, (struct sockaddr*)&address, sizeof(address));
  if (ret == -1) {
    printf("errno is %d\n", errno);
    return 1;
  }
  ret = listen(listenfd, 5);
  assert(ret != -1);

  struct sockaddr_in client;
  socklen_t client_addrlength = sizeof(client);

  connfd = accept(listenfd, (struct sockaddr*)&client, &client_addrlength);
  if (connfd < 0)
    printf("errno is %d\n", errno);
  else {
    addsig(SIGURG, sig_urg);
    // 使用 SIGURG 信号之前，我们必须设置 socket 的宿主进程或进程组
    fcntl(connfd, F_SETOWN, getpid());

    char buffer[BUF_SIZE];
    while (1) {
      memset(buffer, '\0', BUF_SIZE);
      ret = recv(connfd, buffer, BUF_SIZE - 1, 0);
      if (ret <= 0) break;
      printf("got %d bytes of normal data '%s'. \n", ret, buffer);
    }
  }
  return 0;
}