#include "TimeHeap.h"

TimeHeap::TimeHeap(int cap) throw(std::exception) : capacity(cap), cur_size(0) {
  // 创建堆数组
  array = new HeapTimer*[capacity];
  if (!array) throw std::exception();

  for (int i = 0; i < capacity; i++) array[i] = NULL;
}

TimeHeap::TimeHeap(HeapTimer** init_array, int size,
                   int capacity) throw(std::exception)
    : cur_size(size), capacity(capacity) {
  if (capacity < size) throw std::exception();

  // 创建堆数组
  array = new HeapTimer*[capacity];
  if (!array) throw std::exception();

  for (int i = 0; i < capacity; i++) array[i] = NULL;

  if (size != 0) {
    // 初始化数组
    for (int i = 0; i < size; i++) {
      array[i] = init_array[i];
    }
    // 对数组中的第[(cur_size-1)/2] ~ 0 个元素执行下虑操作
    for (int i = (cur_size - 1) / 2; i >= 0; i--) {
      percolate_down(i);
    }
  }
}

TimeHeap::~TimeHeap() {
  for (int i = 0; i < cur_size; i++) delete array[i];
  delete[] array;
}

void TimeHeap::add_timer(HeapTimer* timer) throw(std::exception) {
  if (!timer) return;

  // 堆数组容量不够，扩大1倍
  if (cur_size >= capacity) resize();

  int hole = cur_size++;
  int parent = 0;
  // 对空穴到根结点的路径上的所有结点执行上虑操作
  for (; hole > 0; hole = parent) {
    parent = (hole - 1) / 2;
    if (array[parent]->expire <= timer->expire) break;
    array[hole] = array[parent];
  }
  array[hole] = timer;
}

void TimeHeap::del_timer(HeapTimer* timer) {
  if (!timer) return;

  // 仅仅将目标定时器的回调函数设置为空，即所谓的延迟销毁
  // 这将节省真正删除该定时器造成的开销，但也容易使堆数组膨胀
  timer->cb_func = NULL;
}

HeapTimer* TimeHeap::top() const {
  if (empty()) return NULL;

  return array[0];
}

void TimeHeap::pop_timer() {
  if (empty()) return;

  if (array[0]) {
    delete array[0];
    // 将原来的栈顶元素替换为堆数组中最后一个元素
    array[0] = array[--cur_size];
    // 重新调整时间堆
    percolate_down(0);
  }
}

void TimeHeap::tick() {
  HeapTimer* tmp = array[0];
  time_t cur = time(NULL);

  while (!empty()) {
    if (!tmp) break;
    // 堆顶定时器没到期，则退出循环
    if (tmp->expire > cur) break;
    // 否则就执行堆顶定时器中的任务
    if (array[0]->cb_func) {
      array[0]->cb_func(array[0]->user_data);
    }
    pop_timer();
    tmp = array[0];
  }
}

bool TimeHeap::empty() const { return cur_size == 0; }

void TimeHeap::percolate_down(int hole) {
  int parent = hole;
  int child = parent * 2 + 1;
  HeapTimer* temp = array[parent];

  while (child < cur_size) {
    if ((child < (cur_size - 1)) &&
        (array[child + 1]->expire < array[child]->expire)) {
      child++;
    }

    if (array[child]->expire > temp->expire) break;

    array[parent] = array[child];

    parent = child;
    child = parent * 2 + 1;
  }
  array[parent] = temp;

  //   for (; ((hole * 2 + 1) <= (cur_size - 1)); hole = child) {
  //     child = hole * 2 + 1;
  //     if ((child < (cur_size - 1)) &&
  //         (array[child + 1]->expire < array[child]->expire)) {
  //       child++;
  //     }

  //     if (array[child]->expire < temp->expire) {
  //       array[hole] = array[child];
  //     } else
  //       break;
  //   }
  //   array[hole] = temp;
}

void TimeHeap::resize() throw(std::exception) {
  HeapTimer** temp = new HeapTimer*[2 * capacity];

  for (int i = 0; i < 2 * capacity; i++) temp[i] = NULL;
  
  if (!temp) throw std::exception();

  capacity = 2 * capacity;

  for (int i = 0; i < cur_size; i++) temp[i] = array[i];
  
  delete[] array;
  array = temp;
}
