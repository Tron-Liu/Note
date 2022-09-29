/**
 * @file: test1.cpp
 * @author: Tron-Liu (Tron-Liu@foxmail.com)
 * @brief: 同时接收普通数据和带外数据
 * @date: 2022-09-09
 *
 * @copyright Copyright (c) 2022
 *
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
  if (argc >= 2) {
    printf("usage: %s ip_address port_number\n", basename(argv[0]));
    return 1;
  }
  const char* ip = argv[1];
  int port = atoi(argv[2]);

  // sockaddr_in: IPv4 专用 socket 地址结构体
  // sockaddr_in6: IPv6 专用 socket 地址结构体
  // sockaddr_un: UNIX 本地域协议族专用 socket 地址结构体
  struct sockaddr_in address;
  bzero(&address, sizeof(address));
  // sin_family: 地址族, IPv4使用 AF_INET
  address.sin_family = AF_INET;

  // inet_pton: 将用字符串表示的 IP 地址 (点分十进制字符串)
  // 转换成网络字节序整数表示的 IP 地址
  inet_pton(AF_INET, ip, &address.sin_addr);

  /**
   * htons: host to network short long, 实现主机字节序转换为网络字节序
   * htonl: host to network long
   * nstoh: network short long to host
   * nltoh: network long to host
   **/

  // sin_port: 端口号，要用网络字节序表示
  address.sin_port = htons(port);

  /**
   * int socket(int domain, int type, int protocol);
   *
   * domain: 告诉系统使用哪个底层协议族，PF_INET (IPv4)、PF_INET (IPv6)
   * type: 指定服务类型。SOCK_STREAM (流服务，使用该值表示传输层使用 TCP 协议)
   *                  SOCK_DGRAM (数据报服务，使用该值表示传输层使用 UDP 协议)
   *
   * 调用成功返回一个 socket 文件描述符
   */

  // 创建 socket
  int listenfd = socket(PF_INET, SOCK_STREAM, 0);
  assert(listenfd >= 0);

  /**
   * int bind(int sockfd, const struct sockaddr *my_addr, socklen_t addrlen)
   * 将 my_addr 所指的 socket 地址分配给未命名的 sockfd 文件描述符
   */
  // socket命名：将一个 socket 与 socket 地址绑定
  int ret = bind(listenfd, (struct sockaddr*)&address, sizeof(address));
  assert(ret != -1);

  /**
   * int listen(int sockfd, int backlog)
   * sockfd: 指定被监听的 socket
   * backlog: 提示内核监听队列的最大长度
   */

  // 监听 socket
  ret = listen(listenfd, 5);
  assert(ret != -1);

  struct sockaddr_in client_address;
  socklen_t client_addrlength = sizeof(client_address);

  /**
   * int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
   * sockfd: 执行过 listen 系统调用的监听 socket
   * addr: 用来获取被接受连接的远端 socket 地址
   * addrlen: 指定 socket 地址的长度
   */

  // 接受连接
  int connfd =
      accept(listenfd, (struct sockaddr*)&client_address, &client_addrlength);

  if (connfd < 0) {
    printf("errno is %d\n", errno);
    close(listenfd);
  }

  char buf[1024];
  fd_set read_fds;
  fd_set exception_fds;

  // FD_ZERO(fd_set *fdset); 清除 fdset 的所有位
  FD_ZERO(&read_fds);
  FD_ZERO(&exception_fds);

  while (1) {
    memset(buf, '\0', sizeof(buf));
    /* 每次调用 select 前都要重新在 read_fds 和 exception_fds 中
     * 设置文件描述符 connfd，因为事件发生之后，文件描述符集合将被内核修改
     */

    // FD_SET(int fd, fd_set *fdset): 设置 fdset 的位 fd
    FD_SET(connfd, &read_fds);
    FD_SET(connfd, &exception_fds);

    /**
     * int select(int nfds, fd_set* readfds, fd_set* writefds, fd_set*
     * exceptfds, struct timeval* timeout)
     * nfds: 指定被监听的文件描述符的总数
     * readfds、writefds 和 exceptfds
     * 参数分别指向可读、可写和异常等事件对应的文件描述符集合
     *
     * select 成功时返回就绪 (可读、可写、异常) 文件描述符的总数
     */
    ret = select(connfd + 1, &read_fds, NULL, &exception_fds, NULL);
    if (ret < 0) {
      printf("selection failure\n");
      break;
    }

    /* 对于可读事件，采用普通的 recv 函数读取数据 */
    if (FD_ISSET(connfd, &read_fds)) {
      ret = recv(connfd, buf, sizeof(buf) - 1, 0);
      if (ret <= 0) {
        break;
      }
      printf("get %d bytes of normal data %s\n", ret, buf);
    }
    /*
     * 对于异常事件，采用带 MSG_OOB 标志的 recv 函数读取外带数据
     */

    // int FD_ISSET(int fd, fd_set *fdset) : 测试 fdset 的位 fd 是否被设置
    else if (FD_ISSET(connfd, &exception_fds)) {
      ret = recv(connfd, buf, sizeof(buf) - 1, MSG_OOB);
      if (ret <= 0) {
        break;
      }
      printf("get %d bytes of normal data %s\n", ret, buf);
    }
  }

  close(connfd);
  close(listenfd);

  return 0;
}
