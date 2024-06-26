/**
 * @file test4.cpp
 * @brief 使用共享内存的聊天室服务器程序
 * @version 0.1
 * @date 2024-03-10
 */

#include <arpa/inet.h>
#include <asm-generic/errno-base.h>
#include <assert.h>
#include <cerrno>
#include <csignal>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <libgen.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/epoll.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define USER_LIMIT 5
#define BUFFER_SIZE 1024
#define FD_LIMIT 65535
#define MAX_EVENT_NUMBER 1024
#define PROCESS_LIMIT 65535

struct client_data {
  sockaddr_in address; // 客户端的 socket 地址
  int connfd;          // socket 文件描述符
  pid_t pid;           // 处理这个连接的子进程的 PID
  int pipefd[2];       // 和父进程通信用的管道
};

static const char *shm_name = "/my_shm";
int shmfd;
char *share_mem = nullptr;

int epollfd;
int listenfd;
// 当前客户数量
int curUserCount = 0;
bool stopChild = false;
// 客户连接数组，进程用客户连接的编号来索引这个数组，即可取得相关的客户连接数据
client_data *users = nullptr;
// 子进程和客户连接的映射关系。用进程的 PID
// 来索引这个数组，即可取得该进程所处理的客户连接的编号
int *subProcess = nullptr;
int sig_pipefd[2];

int setnonblocking(int fd) {
  int old_option = fcntl(fd, F_GETFL);
  int new_option = old_option | O_NONBLOCK;
  fcntl(fd, F_SETFL, new_option);
  return old_option;
}

void addfd(int epollfd, int fd) {
  epoll_event event;
  event.data.fd = fd;
  event.events = EPOLLIN | EPOLLET;
  epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
  setnonblocking(fd);
}

void addsig(int sig, void (*handler)(int), bool restart = true) {
  struct sigaction sa;
  sa.sa_handler = handler;
  memset(&sa, '\0', sizeof(sa));
  if (restart) {
    sa.sa_flags |= SA_RESTART;
  }
  sigfillset(&sa.sa_mask);
  assert(sigaction(sig, &sa, NULL) != -1);
}

void sig_handler(int sig) {
  int save_errno = errno;
  int msg = sig;
  send(sig_pipefd[1], (char *)&msg, 1, 0);
  errno = save_errno;
}

void del_resource() {
  close(sig_pipefd[0]);
  close(sig_pipefd[1]);
  close(listenfd);
  close(epollfd);
  shm_unlink(shm_name);
  delete[] users;
  delete[] subProcess;
}

void child_term_handler(int sig) { stopChild = true; }

// 子进程运行的函数
// idx: 指出该子进程处理的客户连接的编号
// users: 保存所有客户连接数据的数组
// share_mem: 共享内存的起始地址
int run_child(int idx, client_data *users, char *share_mem) {
  epoll_event events[MAX_EVENT_NUMBER];
  // 子进程使用 I/O 复用技术来同时监听：
  // 客户连接socket、与父进程通信的管道文件描述符
  int child_epollfd = epoll_create(5);
  assert(child_epollfd != -1);

  int connfd = users[idx].connfd;
  addfd(child_epollfd, connfd);

  int pipefd = users[idx].pipefd[1];
  addfd(child_epollfd, pipefd);

  int ret;
  // 子进程需要设置自己的信号处理函数
  addsig(SIGTERM, child_term_handler, false);
  while (!stopChild) {
    int number = epoll_wait(child_epollfd, events, MAX_EVENT_NUMBER, -1);
    if (number < 0 && (errno != EINTR)) {
      printf("epoll failure\n");
      break;
    }

    for (int i = 0; i < number; ++i) {
      int sockfd = events[i].data.fd;
      // 子进程负责的客户连接有数据到达
      if ((sockfd == connfd) && (events[i]).events & EPOLLIN) {
        memset(share_mem + idx * BUFFER_SIZE, '\0', BUFFER_SIZE);
        // 将客户数据读取到对应的读缓存中。
        // 读缓存是共享内存的一段，它开始于 idx * BUFFER_SIZE 处，长度为
        // BUFFER_SIZE 因此，各个客户的读缓存是共享的
        ret = recv(connfd, share_mem + idx * BUFFER_SIZE, BUFFER_SIZE - 1, 0);

        if (ret < 0) {
          if (errno != EAGAIN) {
            stopChild = true;
          }
        } else if (ret == 0) {
          stopChild = true;
        } else {
          // 成功读取客户数据后就通过管道通知主进程来处理
          send(pipefd, (char *)&idx, sizeof(idx), 0);
        }
      } else if ((sockfd == pipefd) && (events[i].events & EPOLLIN)) {
        // 主进程通过管道通知本进程将第 client
        // 个客户的数据发送到本进程负责的客户端
        int client = 0;
        ret = recv(sockfd, (char *)&client, sizeof(client), 0);
        if (ret < 0) {
          if (errno != EAGAIN) {
            stopChild = true;
          }
        } else if (ret == 0) {
          stopChild = true;
        } else {
          send(connfd, share_mem + client * BUFFER_SIZE, BUFFER_SIZE, 0);
        }
      } else {
        continue;
      }
    }
  }

  close(connfd);
  close(pipefd);
  close(child_epollfd);
  return 0;
}

int main(int argc, char *argv[]) {
  if (argc <= 2) {
    printf("usage: ./%s ip_address port_number\n", basename(argv[0]));
    return 1;
  }

  const char *ip = argv[1];
  int port = atoi(argv[2]);

  int ret = 0;
  struct sockaddr_in address;
  address.sin_family = AF_INET;
  inet_pton(AF_INET, ip, &address.sin_addr);
  address.sin_port = htons(port);

  listenfd = socket(PF_INET, SOCK_STREAM, 0);
  assert(listenfd >= 0);

  ret = bind(listenfd, (struct sockaddr *)&address, sizeof(address));
  assert(ret != -1);

  ret = listen(listenfd, 5);
  assert(ret != -1);

  curUserCount = 0;
  users = new client_data[USER_LIMIT + 1];
  subProcess = new int[PROCESS_LIMIT];
  for (int i = 0; i < PROCESS_LIMIT; ++i) {
    subProcess[i] = -1;
  }

  epoll_event events[MAX_EVENT_NUMBER];
  epollfd = epoll_create(5);
  assert(epollfd != -1);
  addfd(epollfd, listenfd);

  ret = socketpair(PF_UNIX, SOCK_STREAM, 0, sig_pipefd);
  assert(ret != -1);
  setnonblocking(sig_pipefd[1]);
  addfd(epollfd, sig_pipefd[0]);

  addsig(SIGCHLD, sig_handler);
  addsig(SIGTERM, sig_handler);
  addsig(SIGINT, sig_handler);
  // SIG_IGN: 忽略该信号，不做任何处理
  addsig(SIGPIPE, SIG_IGN);

  bool stop_server = false;
  bool terminate = false;

  shmfd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
  // 改变 fd 指定文件的大小为 length
  ret = ftruncate(shmfd, USER_LIMIT * BUFFER_SIZE);
  assert(ret != -1);

  share_mem = (char *)mmap(NULL, USER_LIMIT * BUFFER_SIZE,
                           PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
  assert(share_mem != MAP_FAILED);
  close(shmfd);

  while (!stop_server) {
    int number = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, -1);
    if ((number < 0) && (errno != EINTR)) {
      printf("epoll failure\n");
      break;
    }

    for (int i = 0; i < number; i++) {
      int sockfd = events[i].data.fd;
      if (sockfd == listenfd) {
        struct sockaddr_in client_address;
        socklen_t client_addrlength = sizeof(client_address);

        int connfd = accept(listenfd, (struct sockaddr *)&client_address,
                            &client_addrlength);
        if (connfd < 0) {
          printf("errno is: %d\n", errno);
          continue;
        }

        if (curUserCount >= USER_LIMIT) {
          const char *info = "too many users\n";
          printf("%s", info);
          send(connfd, info, strlen(info), 0);
          close(connfd);
          continue;
        }

        users[curUserCount].address = client_address;
        users[curUserCount].connfd = connfd;

        ret = socketpair(PF_UNIX, SOCK_STREAM, 0, users[curUserCount].pipefd);
        assert(ret != -1);

        pid_t pid = fork();
        if (pid < 0) {
          close(connfd);
          continue;
        } else if (pid == 0) {
          close(epollfd);
          close(listenfd);
          close(users[curUserCount].pipefd[0]);
          close(sig_pipefd[0]);
          close(sig_pipefd[1]);
          run_child(curUserCount, users, share_mem);
          munmap((void *)share_mem, USER_LIMIT * BUFFER_SIZE);
          exit(0);
        } else {
          close(connfd);
          close(users[curUserCount].pipefd[1]);
          addfd(epollfd, users[curUserCount].pipefd[0]);
          users[curUserCount].pid = pid;
          subProcess[pid] = curUserCount;
          curUserCount++;
        }
      } else if ((sockfd == sig_pipefd[0]) && (events[i].events & EPOLLIN)) {
        int sig;
        char signals[1024];
        ret = recv(sig_pipefd[0], signals, sizeof(signals), 0);
        if (ret == -1) {
          continue;
        } else if (ret == 0) {
          continue;
        } else {
          for (int i = 0; i < ret; ++i) {
            switch (signals[i]) {
            // 子进程退出，表示有某个客户端关闭了连接
            case SIGCHLD: {
              pid_t pid;
              int stat;
              while ((pid == waitpid(-1, &stat, WNOHANG)) > 0) {
                int del_user = subProcess[pid];
                subProcess[pid] = -1;
                if ((del_user < 0) || (del_user > USER_LIMIT)) {
                  continue;
                }
                epoll_ctl(epollfd, EPOLL_CTL_DEL, users[del_user].pipefd[0], 0);
                close(users[del_user].pipefd[0]);
                users[del_user] = users[--curUserCount];
                subProcess[users[del_user].pid] = del_user;
              }

              if (terminate && curUserCount == 0) {
                stop_server = true;
              }

              break;
            }
            case SIGTERM:
            case SIGINT: {
              printf("kill all the child now\n");
              if (curUserCount == 0) {
                stop_server = true;
                break;
              }
              for (int i = 0; i < curUserCount; ++i) {
                int pid = users[i].pid;
                kill(pid, SIGTERM);
              }

              terminate = true;
              break;
            }
            default: {
              break;
            }
            }
          }
        }
      } else if (events[i].events & EPOLLIN) {
        int child = 0;
        // 读取管道数据，child变量记录了是哪个客户连接有数据到达
        ret = recv(sockfd, (char *)&child, sizeof(child), 0);
        printf("read data from child accross pipe\n");
        if (ret == -1) {
          continue;
        } else if (ret == 0) {
          continue;
        } else {
          // 向除负责处理第 child
          // 个客户连接的子进程之外的其他子进程发送消息，通知它们有客户数据要写
          for (int j = 0; curUserCount; ++j) {
            if (users[j].pipefd[0] != sockfd) {
              printf("send data to child across pipe\n");
              send(users[j].pipefd[0], (char *)&child, sizeof(child), 0);
            }
          }
        }
      }
    }
  }
  del_resource();
  return 0;
}