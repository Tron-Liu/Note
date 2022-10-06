#ifndef TIME_WHEEL
#define TIME_WHEEL

#include <time.h>

#include "time_wheel_timer.h"

class time_wheel {
 public:
  time_wheel();
  ~time_wheel();
  // 根据定时值 timeout 创建一个定时器，并把它插入合适的槽中
  TimeWheelTimer* add_timer(int timeout);
  // 删除目标定时器 timer
  void del_timer(TimeWheelTimer* timer);
  // SI 时间到后，调用该函数，时间轮向前滚动一个槽的间隔
  void tick();

 private:
  static const int N = 60;  // 时间轮上的槽数目
  static const int SI = 1;  // 每 1s 时间转动轮一次，即槽间隔为 1s
  // 时间轮的槽，其中每一个元素指向一个定时器链表，链表无序
  TimeWheelTimer* slots[N];
  int cur_slot;  // 时间轮的当前槽
};

#endif