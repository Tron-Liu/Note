/**
 * @file: test2.cpp
 * @brief: 升序定时器链表
 * @date: 2022-09-21
 */

#ifndef LST_TIMER
#define LST_TIMER

#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>

#define BUFFER_SIZE 64

// 前向声明
class UtilTimer;

// 用户数据结构
struct client_data {
  sockaddr_in address;    // 客户端 socket 地址
  int sockfd;             // socket 文件描述符
  char buf[BUFFER_SIZE];  // 读缓存
  UtilTimer* timer;       // 定时器
};

// 定时器类
class UtilTimer {
 public:
  UtilTimer() : prev(NULL), next(NULL) {}

 public:
  time_t expire;  // 任务超时时间，这里使用绝对时间
  void (*cb_func)(client_data*);  // 任务回调函数

  // 回调函数处理的客户数据，由定时器的执行者传递给回调函数
  client_data* user_data;
  UtilTimer* prev;
  UtilTimer* next;
};

#endif
