/**
 * @file 15-1.h
 * @author Tron-Liu (Tron-Liu@foxmail.com)
 * @brief 半同步/半异步进程池
 * @version 0.1
 * @date 2022-09-14
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef PROCESSPOOL_H
#define PROCESSPOOL_H

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// 描述一个子进程的类，m_pid 是目标子进程的 PID，m_pipefd
// 是父进程和子进程通信用的管道
class process {
 public:
  process() : m_pid(-1) {}

 public:
  pid_t m_pid;
  int m_pipefd[2];
};

// 进程池类, 将它定义为模板类是为了代码复用。其模板参数是处理逻辑任务的类
template <typename T>
class processpool {
 private:
  // 将构造函数定义为私有的，因此我们只能通过后面的 create 静态函数来创建
  // processpool 实例
  processpool(int listenfd, int process_number = 8);

 public:
  // 单体模式，以保证程序最多创建一个 processpool 实例，
  // 这是程序正确处理信号的必要条件
  static processpool<T>* create(int listenfd, int process_number = 8) {
    if (!m_instance) {
      m_instance = new processpool<T>(listenfd, process_number);
    }
    return m_instance;
  }
  ~processpool() { delete[] m_sub_process; }

  // 启动进程池
  void run();

 private:
  void setup_sig_pipe();
  void run_parent();
  void run_child();

 private:
  // 进程池允许的最大子进程数量
  static const int MAX_PROCESS_NUMBER = 16;
  // 每个子进程最多能处理的客户数量
  static const int USER_PER_PROCESS = 65536;
  // epoll 最多能处理的事件数
  static const int MAX_EVENT_NUMBER = 10000;
  // 进程池中的进程总数
  int m_process_number;
  // 子进程在池中的序号，从 0 开始
  int m_idx;
  // 每个进程都有一个 epoll 内核事件表，用 m_epoll 标识
  int m_epollfd;
  // 监听 socket
  int m_listenfd;
  // 子进程通过 m_stop 来决定是否停止运行
  int m_stop;
  // 保存所有子进程的描述信息
  process* m_sub_process;
  // 进程池静态实例
  static processpool<T>* m_instance;
};

template <typename T>
processpool<T>* processpool<T>::m_instance = NULL;

// 用于处理信号的管道，以实现统一事件源。后面称之为信号管道
static int sig_pipefd[2];

static int setnonblocking(int fd) {
  int old_option = fcntl(fd, F_GETFL);
  // 设置非阻塞标志
  int new_option = old_option | O_NONBLOCK;
  fcntl(fd, F_SETFL, new_option);

  return old_option;
}

static void addfd(int epollfd, int fd) {
  epoll_event event;
  event.data.fd = fd;
  /**
   * epoll 支持的事件类型和 poll 基本相同
   * 表示 epoll 事件类型的宏是在 poll 对应的宏前加上 "E"
   *
   * EPOLLIN: 数据（包括普通数据和优先数据）可读
   * EPOLLOUT: 数据（包括普通数据和优先数据）可写
   * EPOLLPRI: 高优先级数据可读，比如 TCP 带外数据
   * EPOLLERR: 错误
   * EPOLLHUP: TCP 连接被对方关闭，或者对方关闭了写操作
   * EPOLLET: 设置 epoll 为 ET 模式。
   * ET(Edge Trigger, 边沿触发)模式是 epoll 的高效工作模式
   */
  event.events = EPOLLIN | EPOLLET;
  epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
  setnonblocking(fd);
}

// 从 epollfd 标识的 epoll 内核事件表中删除 fd 上的所有注册事件
static void removefd(int epollfd, int fd) {
  /**
   * epoll_ctl(int epfd, int op, int fd, struct epoll_event *event)
   * 操作 epoll 的内核事件表
   * fd: 要操作的文件描述符
   * op: 指定的操作类型
   *     EPOLL_CTL_ADD: 往事件表中注册 fd 上的事件
   *     EPOLL_CTL_MOD: 修改 fd 上的注册事件
   *     EPOLL_CTL_DEL: 删除 fd 上的注册事件
   * event: 指定事件
   *
   * 成功返回 0，失败返回 -1 并设置 errno
   */

  /**
   * struct epoll_event {
   *  __uint_t events;    // epoll 事件
   *  epoll_data_t data;  // 存储用户数据
   * }
   */

  /**
   * typedef union epoll_data {
   *  void* ptr;      // 可用来指定与 fd 相关的用户数据
   *  int fd;         // 指定事件所从属的目标文件描述符
   *  uint32_t u32;
   *  uint64_t u64;
   * } epoll_data_t;
   */
  epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, 0);
  close(fd);
}

// 信号处理函数
static void sig_handler(int sig) {
  int save_errno = errno;
  int msg = sig;
  send(sig_pipefd[1], (char*)&msg, 1, 0);
  errno = save_errno;
}

int main(int argc, char* argv[]) {
  return 0;
}

#endif