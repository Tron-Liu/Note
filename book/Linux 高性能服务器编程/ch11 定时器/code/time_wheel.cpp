#include "time_wheel.h"

time_wheel::time_wheel() : cur_slot(0) {
  for (int i = 0; i < N; i++) slots[i] = NULL;  // 初始化每个槽的头结点
}

time_wheel::~time_wheel() {
  // 遍历每个槽，并销毁其中的定时器
  for (int i = 0; i < N; i++) {
    TimeWheelTimer* tmp = slots[i];
    while (tmp) {
      slots[i] = tmp->next;
      delete tmp;
      tmp = slots[i];
    }
  }
}

TimeWheelTimer* time_wheel::add_timer(int timeout) {
  if (timeout < 0) return NULL;

  int ticks = 0;
  // 下面根据待插入定时器的超时值计算它将在时间轮转动多少个滴答后被触发，
  // 并将该滴答数存储与变量 ticks 中
  // 如果待插入定时器的超时值小于时间轮的槽间隔，则向上取整为 1
  // 否则就将 ticks 向下取整为 timeout / SI
  if (timeout < SI)
    ticks = 1;
  else
    ticks = timeout / SI;

  // 计算待插入的定时器在时间轮转动多少圈后被触发
  int rotation = ticks / N;
  // 计算待插入的定时器应该被插入哪个槽中
  int ts = (cur_slot + (ticks % N)) % N;
  // 创建新的定时器，它在时间轮转动 rotation 圈后被触发，且位于第 ts 个槽上
  TimeWheelTimer* timer = new TimeWheelTimer(rotation, ts);

  // 如果第 ts 个槽中尚无任何定时器，则把新建的定时器插入其中
  // 并将该定时器设置为该槽的头结点
  if (!slots[ts]) {
    printf("add timer, rotation is %d, ts is %d, cur_slot is %d.\n", rotation,
           ts, cur_slot);
    slots[ts] = timer;
  }
  // 否则，将定时器插入第 ts 个槽中
  else {
    timer->next = slots[ts];
    slots[ts]->prev = timer;
    slots[ts] = timer;
  }
  return timer;
}

void time_wheel::del_timer(TimeWheelTimer* timer) {
  if (!timer) return;

  int ts = timer->time_slot;
  // slots[ts] 是目标定时器所在槽的头结点。如果目标定时器就是该头结点
  // 则需要重置第 ts 个槽的头结点
  if (timer == slots[ts]) {
    slots[ts] = slots[ts]->next;
    if (slots[ts]) slots[ts]->prev = NULL;

    delete timer;
  } else {
    timer->prev->next = timer->next;
    if (timer->next) {
      timer->next->prev = timer->prev;
    }
    delete timer;
  }
}

void time_wheel::tick() {
  // 取得时间轮上当前槽的头结点
  TimeWheelTimer* tmp = slots[cur_slot];
  printf("current slot is %d.\n", cur_slot);

  while (tmp) {
    printf("tick the timer once.\n");
    // 如果定时器的 rotation 值大于 0，则它这一轮不起作用
    if (tmp->rotation > 0) {
      tmp->rotation--;
      tmp = tmp->next;
    }
    // 否则，说明定时器已经到期，执行定时任务，然后删除该定时器
    else {
      tmp->cb_func(tmp->user_data);
      if (tmp == slots[cur_slot]) {
        printf("delete header in cur_slot.\n");
        slots[cur_slot] = tmp->next;
        delete tmp;

        if (slots[cur_slot]) slots[cur_slot]->prev = NULL;
      } else {
        tmp->prev->next = tmp->next;
        if (tmp->next) tmp->next->prev = tmp->prev;

        TimeWheelTimer* tmp2 = tmp->next;
        delete tmp;
        tmp = tmp2;
      }
    }
  }

  // 更新时间轮的当前槽，以反映时间轮的转动
  cur_slot = ++cur_slot % N;
}
