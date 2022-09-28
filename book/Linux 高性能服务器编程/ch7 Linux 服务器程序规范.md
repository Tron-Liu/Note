# 第7章 Linux 服务器程序规范

1. 服务器程序规范
    * Linux 服务器程序一般以后台进程形式运行。后台进程又称守护进程（daemon）。它没有控制终端，因而也不会意外接收到用户输入。守护进程的父进程通常时 init 进程（ PID为1的进程 ）
    * Linux 服务器程序通常有一套日志系统，它至少能输出日志到文件，有的高级服务器还能输出日志到专门的 UDP 服务器。大部分后台进程都在 `/var/log` 目录下拥有自己的日志目录
    * Linux 服务器程序一般以某个专门的非 root 身份运行。比如 mysqld、httpd、syslogd 等后台进程，分别拥有自己的运行账户 mysql、apache 和 syslog
    * Linux 服务器程序通常是可配置的。服务器程序通常能处理很多命令行选项，如果一次运行的选项太多，则可以用配置文件来管理。绝大多数服务器程序都有配置文件，并存在 `/etc` 目录下
    * Linux 服务器程序通常会在启动的时候生成一个 PID 文件并存入 `/var/run` 目录中，以记录该后台进程的 PID
    * Linux 服务器程序通常需要考虑系统资源和限制，以预测自身能承受多大负荷，比如进程可用文件描述符总数和内存总量等

## 7.1 日志

### 7.1.1 Linux 系统日志

### 7.1.2 syslog 函数

1. 应用程序使用 `syslog` 函数与 `rsyslogd` 守护进程通信。
2. `syslog` 函数的定义如下：

    ```c
    #include <syslog.h>

    void syslog(int priority, const char* message, ...);
    ```

3. 日志级别

    ```c
    #include <syslog.h>

    #define LOG_EMERG     0  // 系统不可用
    #define LOG_ALERT     1  // 报警，需要立即采取动作
    #define LOG_CRIT      2  // 非常严重的情况
    #define LOG_ERR       3  // 错误
    #define LOG_WARNING   4  // 警告
    #define LOG_NOTICE    5  // 通知
    #define LOG_INFO      6  // 信息
    #define LOG_DEBUG     7  // 调试
    ```

4. 改变 syslog 的默认输出方式，进一步结构化日志内容

    ```c
    #include <syslog.h>

    void openlog(const char* ident, int logopt, int facility);

    facility: 可用来修改syslog函数中的默认设施值
    ```

## 9.2 poll 系统调用

## 9.3 epoll 系列系统调用

### 9.3.1 内核事件表

1. `epoll` 是 Linux 特有的 I/O 复用函数。它在实现和使用上与 `select`、`poll` 有很大差异
   * 首先，`epoll` 使用一组函数来完成任务，而不是单个函数
   * 其次，`epoll` 把用户关心的文件描述符上的事件放在内核里的一个事件表中，从而无需像 `select` 和 `poll` 那样每次调用都要重复传入文件描述符集或事件集
   * `epoll` 需要使用一个额外的文件描述符来唯一标识内核中的事件表，这个文件描述符由 `epoll_create` 创建
2. `epoll_create` 函数定义如下：

   ```c
   #include <sys/epoll.h>

   int epoll_create(int size)
   ```

3. `epoll_ctl` 操作 `epoll` 的内核事件表，函数原型如下：

   ```c
   #include <epoll.h>

   int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event)

   fd: 要操作的文件描述符
   op: 指定操作类型
   event: 指定事件

   成功返回 0，失败则返回 -1 并设置 errno
   ```

4. `op` 参数的操作类型

   ```c
   EPOLL_CTL_ADD: 往事件表中注册 fd 上的事件
   EPOLL_CTL_MOD: 修改 fd 上的注册事件
   EPOLL_CTL_DEL: 删除 fd 上的注册事件
   ```

5. `epoll_event` 定义如下：

   ```c
   struct epoll_event {
     __uint32_t events;    // epoll 事件
     epoll_data_t data;    // 用户数据
   }

   typedef union epoll_data {
     void* ptr;
     int fd;
     uint32_t u32;
     uint64_t u64;
   } epoll_data_t;

   fd: 指定事件所从属的目标文件描述符
   ptr: 指定与 fd 相关的用户数据
   ```

### 9.3.2 epoll_wait 函数

1. `epoll_wait` 函数在一段超时时间内等待一组文件描述符上的事件，原型如下：

   ```c
   #include <sys/epoll.h>

   int epoll_wait(int epfd, struct epoll_event* events, int maxevents, int timeout)

   epfd: 指定内核事件表
   maxevents: 最多监听多少个事件

   函数成功返回就绪的文件描述符的个数，失败时返回-1并设置errno
   ```

2. `epoll_wait` 函数如果检测到事件，就将所有就绪的事件从内核事件表（由 `epfd` 参数指定）中复制到它的第二个参数 `events` 指向的数组中。这个数组只用于输出 `epoll_wait` 检测到的就绪事件

### 9.3.3 LT 和 ET 模式

1. `epoll` 对文件描述符的操作有两种模式 `LT` 模式和 `ET` 模式
   * LT: Level Trigger，电平触发。该模式是默认的工作模式
