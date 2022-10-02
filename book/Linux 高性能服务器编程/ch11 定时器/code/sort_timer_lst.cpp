#include "sort_timer_lst.h"

SortTimerLst::SortTimerLst() : head(NULL), tail(NULL) {}

SortTimerLst::~SortTimerLst() {
  UtilTimer* tmp = head;
  while (tmp) {
    head = tmp->next;
    delete tmp;
    tmp = head;
  }
}

void SortTimerLst::add_timer(UtilTimer* timer) {
  if (!timer) return;

  if (!head) {
    head = tail = timer;
    return;
  }

  if (timer->expire < head->expire) {
    timer->next = head;
    head->prev = timer;
    head = timer;
    return;
  }

  add_timer(timer, head);
}

void SortTimerLst::adjust_timer(UtilTimer* timer) {
  if (!timer) return;

  UtilTimer* tmp = timer->next;

  // 如果被调整的目标定时器处在链表尾部，或者该定时器新的
  // 超时值仍然小于其下一个定时器的超时值，则不用调整
  if (!tmp || (timer->expire < tmp->expire)) return;

  // 如果目标定时器是链表的头结点，则将定时器冲链表中取出并重新插入链表
  if (timer == head) {
    head = head->next;
    head->prev = NULL;
    timer->next = NULL;

    add_timer(timer, head);
  }
  // 如果目标定时器不是链表的头结点，则将该定时器从链表中取出
  // 然后插入其原来所在位置之后的部分链表中
  else {
    timer->prev->next = timer->next;
    timer->next->prev = timer->prev;

    add_timer(timer, timer->next);
  }
}

void SortTimerLst::del_timer(UtilTimer* timer) {
  if (!timer) return;

  // 如果链表中只有一个目标定时器
  if ((timer == head) && (timer == tail)) {
    delete timer;
    head = NULL;
    tail = NULL;
  }

  // 链表中至少有两个定时器，目标定时器是头节点
  if (timer == head) {
    head = head->next;
    head->prev = NULL;
    delete timer;

    return;
  }

  // 链表中至少有两个定时器，目标定时器是尾节点
  if (timer == tail) {
    tail = tail->prev;
    tail->next = NULL;
    delete timer;

    return;
  }

  // 如果目标定时器位于链表中间
  timer->prev->next = timer->next;
  timer->next->prev = timer->prev;
}

// SIGALRM 信号每次被触发就在其信号处理函数中执行一次 tick 函数
// 如果是使用统一时间源，则是主函数
void SortTimerLst::tick() {
  if (!head) return;

  printf("timer tick\n");
  time_t cur = time(NULL);  // 获得系统当前的时间
  UtilTimer* tmp = head;

  // 从头节点开始依次处理每个定时器，直到遇到一个尚未到期的定时器
  // 因为每个定时器都是用绝对时间作为超时值，所以我们可以把定时器的
  // 超时值和系统当前时间进行比较，以判断定时器是否到期
  while (tmp) {
    if (cur < tmp->expire) break;

    // 调用定时器的回调函数，以执行定时任务
    tmp->cb_func(tmp->user_data);
    // 执行完定时器中的定时任务之后，就将它从链表中删除，并重置链表头节点
    head = tmp->next;
    if (head) head->prev = NULL;

    delete tmp;
    tmp = head;
  }
}

// 将目标定时器 timer 添加到节点 lst_head 之后的部分链表中
void SortTimerLst::add_timer(UtilTimer* timer, UtilTimer* lst_head) {
  UtilTimer* prev = lst_head;
  UtilTimer* tmp = prev->next;

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
