#ifndef TIME_HEAP
#define TIME_HEAP

#include "HeapTimer.h"
#include <algorithm>

class TimeHeap {
 public:
  // 构造函数之一，初始化一个大小为 cap 的空堆
  TimeHeap(int cap);
  // 构造函数之二，用已有数组来初始化堆
  TimeHeap(HeapTimer** init_array, int size, int capacity);
  // 销毁时间堆
  ~TimeHeap();

 public:
  // 添加目标定时器
  void add_timer(HeapTimer* timer);
  void del_timer(HeapTimer* timer);
  // 获得堆顶部的定时器
  HeapTimer* top() const;
  // 删除堆顶部的定时器
  void pop_timer();
  // 心搏函数
  void tick();
  bool empty() const;
 
 private:
  // 最小堆的下虑操作，它确保堆数组中以第 hole 个结点作为根的子树
  // 拥有最小堆性质
  void percolate_down(int hole);
  // 将堆数组容量扩大 1 倍
  void resize();

 private:
  // 堆数组
  HeapTimer** array;
  // 堆数组的容量
  int capacity;
  // 堆数组当前包含元素的个数
  int cur_size;
};

#endif