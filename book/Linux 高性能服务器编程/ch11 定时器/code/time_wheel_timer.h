#ifndef TIME_WHEEL_TIMER
#define TIME_WHEEL_TIMER

#include <netinet/in.h>
#include <stdio.h>
#include <time.h>

#define BUFFER_SIZE 64

class TimeWheelTimer;

// 绑定 socket 和定时器
struct client_data {
  sockaddr_in address;
  int sockfd;
  char buf[BUFFER_SIZE];
  TimeWheelTimer* timer;
};

// 定时器类
class TimeWheelTimer {
 public:
  TimeWheelTimer(int rot, int ts);

 public:
  int rotation;   // 记录定时器在时间轮转多少圈后生效
  int time_slot;  // 记录定时器属于时间轮上的哪个槽
  void (*cb_func)(client_data*);  // 定时器回调函数
  client_data* user_data;         // 客户数据
  TimeWheelTimer* next;           // 指向下一个定时器
  TimeWheelTimer* prev;           // 指向前一个定时器
};

#endif