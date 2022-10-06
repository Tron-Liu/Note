#ifndef SORT_TIMER_LST
#define SORT_TIMER_LST

#include <stdio.h>

#include "util_timer.h"

// 定时器链表，一个升序、双向链表，且带有头结点和尾结点
class SortTimerLst {
 public:
  SortTimerLst();
  ~SortTimerLst();

  void add_timer(UtilTimer* timer);
  void adjust_timer(UtilTimer* timer);
  void del_timer(UtilTimer* timer);
  void tick();

 private:
  void add_timer(UtilTimer* timer, UtilTimer* lst_head);

 private:
  UtilTimer* head;
  UtilTimer* tail;
};

#endif