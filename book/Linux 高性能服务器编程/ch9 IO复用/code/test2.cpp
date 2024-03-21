/**
 * @file test2.cpp
 * @brief poll 和 epoll 在使用上的差别
 * @version 0.1
 * @date 2024-03-18
 */

#include <poll.h>
#include <sys/poll.h>

#define MAX_EVENT_NUMBER 1024

// 索引 poll 返回的就绪文件描述符
void test_poll() {
  struct pollfd fds[MAX_EVENT_NUMBER];
  int ret = poll(fds, MAX_EVENT_NUMBER, -1);
  // 必须遍历所有已注册文件描述符并找到其中的就绪者
  for (int i = 0; i < MAX_EVENT_NUMBER; ++i) {
    // 判断第 i 个文件描述符是否就绪
    if (fds[i].events & POLLIN) {
      int sockfd = fds[i].fd;
      // 处理 sockfd
    }
  }
}

// 索引 epoll 返回的就绪文件描述符
void test_epoll() {
  int ret = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, -1);
  // 仅遍历就绪的 ret 个文件描述符
  for (int i = 0; i < ret; ++i) {
    int sockfd = events[i].data.fd;
    // sockfd 肯定就绪，直接处理
  }
}
