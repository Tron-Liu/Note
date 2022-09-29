/**
 * @file: test2.cpp
 * @author: Tron-Liu (Tron-Liu@foxmail.com)
 * @brief: 升序定时器链表
 * @date: 2022-09-21
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef LST_TIMER
#define LST_TIMER

#include <time.h>

#define BUFFER_SIZE 64

// 前向声明
class util_timer;

//
struct client_data {
  sockaddr_in address;    // 客户端 socket 地址
  int sockfd;             // socket 文件描述符
  char buf[BUFFER_SIZE];  // 读缓存
  util_timer* timer;      // 定时器
};

class util_timer {
 public:
  time_t expire;  // 任务超时时间，这里使用绝对时间
  void (*cb_func)(client_data*);  // 任务回调函数
  // 回调函数处理的客户数据，由定时器的执行者传递给回调函数
  client_data* user_data;
  util_timer* prev;
  util_timer* next;
};

class sort_timer_lst {
 public:
  sort_timer_lst() : head(NULL), tail(NULL) {}
  ~sort_timer_lst() {
    util_timer* tmp = head;

    while(tmp) {
      head = tmp->next;
      delete tmp;
      tmp = head;
    }
  }
  void add_timer(util_timer* timer) {
    if (!timer) return;
    if (!head) {
      head = tail = timer;
      return ;
    }
    if (timer->expire < head->expire) {
      timer->next = head;
      head->prev = timer;
      head = timer;
      return;
    }
    add_timer(timer, head);
  }

  void tick() {
    if (!head) return;
    printf("timer tick\n");
    time_t cur = time(NULL);
    util_timer* tmp = head;

 private:
  void add_timer(util_timer* timer, util_timer* lst_head) {
    util_timer* prev = lst_head;
    util_timer* tmp = prev->next;

    while (tmp) {
      if (timer->expire < tmp->expire) {
        prev->next = timer;
        timer->next = tmp;
        tmp->prev = timer;
        timer->prev = prev;
        break;
      }
      prev = tmp;
      tmp = tmp->next;
    }
    
    if (!tmp) {
      prev->next = timer;
      timer->prev = prev;
      timer->next = NULL;
      tail = timer;
    }
  }

 private:
   util_timer* head;
   util_timer* tail;
}
int main(int argc, char* argv[]) {
  return 0;
}

#endif
