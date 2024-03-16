/**
 * @file test4.cpp
 * @author
 * @brief 使用 splice 函数实现的回射服务器
 * @version 0.1
 * @date 2024-03-16
 *
 */

#include <arpa/inet.h>
#include <cassert>
#include <cerrno>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <libgen.h>
#include <netinet/in.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc <= 2) {
    printf("usage: %s ip_address port_number\n", basename(argv[0]));
    return 1;
  }

  const char *ip = argv[1];
  int port = atoi(argv[2]);

  struct sockaddr_in address;
  bzero(&address, sizeof(address));
  address.sin_family = AF_INET;
  inet_pton(AF_INET, ip, &address.sin_addr);
  address.sin_port = htons(port);

  int sock = socket(PF_INET, SOCK_STREAM, 0);
  assert(sock >= 0);

  int ret = bind(sock, (struct sockaddr *)&address, sizeof(address));
  assert(ret != -1);

  ret = listen(sock, 5);
  assert(ret != -1);

  struct sockaddr_in client;
  socklen_t client_addrlength = sizeof(client);
  int connfd = accept(sock, (struct sockaddr *)&client, &client_addrlength);

  if (connfd < 0)
    printf("errno is: %d\n", errno);
  else {
    int pipefd[2];
    assert(ret != -1);
    ret = pipe(pipefd);
    // 将 connfd 上流入的客户数据定向到管道中
    ret = splice(connfd, NULL, pipefd[1], NULL, 32768,
                 SPLICE_F_MORE | SPLICE_F_MOVE);
    assert(ret != -1);
    // 将管道的输出定向到 connfd 客户连接文件描述符
    ret = splice(pipefd[0], NULL, connfd, NULL, 32768,
                 SPLICE_F_MORE | SPLICE_F_MOVE);
    close(connfd);
  }

  close(sock);
  return 0;
}