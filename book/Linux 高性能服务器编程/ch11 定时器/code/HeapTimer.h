#ifndef MIN_HEAP
#define MIN_HEAP

#include <netinet/in.h>
#include <time.h>

#include <iostream>

using std::exception;

#define BUFFER_SZIE 64

class HeapTimer;

struct client_data {
  sockaddr_in address;
  int sockfd;
  char buf[BUFFER_SZIE];
  HeapTimer* timer;
};


class HeapTimer {
 public:
  HeapTimer(int delay);

 public:
  // 定时器生效的绝对时间
  time_t expire;
  // 定时器的回调函数
  void (*cb_func)(client_data*);
  // 用户数据
  client_data* user_data;
};

#endif