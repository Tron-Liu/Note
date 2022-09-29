# 第14章 多线程编程

## 14.1 Linux 线程概述

## 14.2 创建线程和结束线程

1. `pthread_create` 创建一个线程，定义如下：

   ```c
   #include <pthread.h>

   int pthread_create(pthread_t* thread, const pthread_attr_t* attr,
                      void* (*start_routine)(void*), void* arg);
   thread: 新线程的标识符，后续 pthread* 函数通过它来引用新进程
   arr: 用于设置新线程的属性，给它传递 NULL 表示使用默认线程属性
   start_routine: 指定新线程将运行的函数
   arg: 指定新线程将运行的参数

   成功返回 0，失败返回错误码
   ```

   - `pthread_t` 的定义如下：

      ```c
      #include <bits/pthreadtypes.h>

      typedef unsigned long int pthread_t;
      ```

2. 一个用户可以打开的线程数量不能超过 `RLIMIT_NPROC` 软资源限制。系统上所有用户能创建的线程总数也不能超过 `/proc/sys/kernel/threads-max` 内核参数所定义的值

3. `pthread_exit` 可确保线程函数安全、干净地退出。该函数通过 `retval` 参数向线程的回收者传递其退出信息，它执行完之后不会返回到调用者，而且永远不会失败。定义如下：

   ```c
   #include <pthread.h>

   void pthread_exit(void* retval);
   ```

4. 一个进程中的所有线程都可以调用 `pthread_join` 函数来回收其他线程（前提是目标线程是可回收的），即等待其他线程结束。定义如下：

   ```c
   #include <pthread.h>

   int pthread_join(pthread_t thread, void** retval);

   thread: 目标线程的标识符
   retval: 目标线程返回的退出信息
   该函数会一直阻塞，直到被回收的线程结束为止

   成功返回 0，失败返回错误码
   ```

   `pthread_join` 可能引发的错误码
      | 错误码 | 描述 |
      |:----- |:---- |
      | EDEADLK | 可能引起死锁，比如两个线程互相针对对方调用 pthread_join，或者线程对自身调用 pthread_join |
      | EINVAL | 目标线程是不可回收的，或者已经有其他线程在回收该目标线程 |
      | ESRCH | 目标线程不存在 |

5. `pthread_cancel` 函数可异常终止一个线程，即取消线程。定义如下：

   ```c
   #include <pthread.h>

   int pthread_cancel(pthread_t thread);
   ```

## 14.4 POSIX 信号量

1. 3 种专门用于线程同步的机制：POSIX 信号量、互斥量和条件变量
2. POSIX 信号量函数的名字都以 `sem_` 开头，常用的 POSIX 信号量函数是下面 5 个：

   ```c
   #include <semaphore.h>

   int sem_init(sem_t* sem, int pshared, unsigned int value);
   int sem_destroy(sem_t* sem);
   int sem_wait(sem_t* sem);
   int sem_trywait(sem_t* sem);
   int sem_post(sem_t* sem);

   这些函数的第一个参数 sem 指向被操作的信号量
   ```

## 14.6 条件变量

1. 如果说互斥锁是用于同步线程对共享数据的访问的话，那么条件变量则是用于在线程之间同步共享数据的值
2. 条件变量提供了一种线程间的通知机制：当某个共享数据达到某个值的时候，唤醒等待这个共享数据的线程
