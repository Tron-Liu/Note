/**
 * @file: test1.c
 * @brief: CGI 服务器原理
 * @date: 2022-09-17
 **/

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <libgen.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
  if (argc <= 2) {
    printf("usage: %s ip_address port_number\n", basename(argv[0]));
    return 1;
  }

  const char* ip = argv[1];
  int port = atoi(argv[2]);

  struct sockaddr_in address;
  bzero(&address, sizeof(address));
  address.sin_family = AF_INET;
  inet_pton(AF_INET, ip, &address.sin_addr);
  address.sin_port = htons(port);

  int sock = socket(PF_INET, SOCK_STREAM, 0);
  assert(sock >= 0);

  int ret = bind(sock, (struct sockaddr*)&address, sizeof(address));
  assert(ret != -1);

  ret = listen(sock, 5);
  assert(ret != -1);

  struct sockaddr_in client;
  socklen_t len = sizeof(client);

  int connfd = accept(sock, (struct sockaddr*)&client, &len);
  if (connfd < 0)
    printf("errno is: %d.\n", errno);
  else {
    //关闭标准输出文件描述符 STDOUT_FILENO (值为 1)
    close(STDOUT_FILENO);
    /*
     * dup 总是返回系统中最小的可用文件描述符，所以此处返回 1
     * 这样一来，服务器输出到标准输出的内容就会直接发送到
     * 与客户连接对应的 socket
     */
    dup(connfd);
    // printf 的输出内容将被客户端获得，而不是显示在服务器程序的终端
    printf("abcd\n");
    close(connfd);
  }

  close(sock);
  return 0;
}
