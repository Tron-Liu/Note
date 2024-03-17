# 第9章 I/O复用

1. I/O 复用使得程序能同时监听多个文件描述符，这对提高程序的性能至关重要。通常，网络程序在下列情况下需要使用 I/O 复用技术：
    * 客户端程序要同时处理多个 socket
    * 客户端程序要同时处理用户输入和网络连接
    * TCP 服务器要同时处理监听 socket 和连接 socket
    * 服务器要同时处理 TCP 请求和 UDP 请求
    * 服务器要同时监听多个端口，或者处理多种服务
2. 需要注意的是, I/O 复用虽然能同时监听多个文件描述符，但它本身是阻塞的。并且当多个文件描述符同时就绪时，如果不采取额外的措施，程序就只能按顺序依次处理其中的每一个文件描述符，这使得服务器程序看起来像是串行工作的。

## 9.1 select 系统调用

1. `select` 系统调用的用途是：在一段指定时间内，监听用户感兴趣的文件描述符上的可读、可写和异常等事件

### 9.1.1 select API

1. select 系统调用的原型如下：

    ```c
    #include <sys/select.h>

    int select(int nfds, fd_set* readfds, fd_set* writefds, fd_set* exceptfds, struct timeval* timeout);

    nfds: 指定被监听的文件描述符的总数。通常被设置为select监听的所有文件描述符中的最大值加1
    readfds: 指向可读事件对应的文件描述符
    writefds: 指向可写事件对应的文件描述符
    exceptfds: 指向异常事件对应的文件描述符
    timeout: 设置select函数的超时时间

    成功返回就绪（可读、可写和异常）文件描述符的总数
    失败返回-1并设置errno
    如果在超时时间内没有任何文件描述符就绪，返回0
    ```

2. 访问结构体 fd_set 的宏

    ```c
    #include <sys/select.h>

    FD_ZERO(fd_set *fdset);               // 清除 fdset 的所有位
    FD_SET(int fd, fd_set *fdset);        // 设置 fdset 的位 fd
    FD_CLR(int fd, fd_set *fdset);        // 清除 fdset 的位 fd
    int FD_ISSET(int fd, fd_set *fdset);  // 测试 fdset 的位 fd 是否被设置
    ```

### 9.1.2 文件描述符就绪条件

1. 哪些情况下文件描述符可以被认为是可读、可写或者出现异常，对于 select 的使用非常关键。
2. 在网络编程中，下列情况下 socket 可读：
    * socket 内核接收缓存区中的字节数大于或等于其低水位标记 `SO_RCVLOWAT`。此时我们可以无阻塞地读该 socket，并且读操作返回的字节数大于 0
    * socket 通信的对方关闭连接。此时对该 socket 的读操作将返回 0
    * 监听 socket 上有新的连接请求
    * socket 上有未处理的错误。此时我们可以使用 `getsockopt` 来读取和清除该错误
3. 下列情况下 socket 可写：
    * socket 内核发送缓存区中的可用字节数大于或等于其低水位标记 `SO_SNDLOWAT`。此时我们可以无阻塞地写该 socket，并且写操作返回的字节数大于 0
    * socket 的写操作被关闭。对写操作被关闭的 socket 执行写操作将触发一个 `SIGPIPE` 信号
    * socket 使用非阻塞 connect 连接成功或者失败（超时）之后
    * socket 上有未处理的错误。此时我们可以使用 `getsockopt` 来读取和清除该错误
4. 网络程序中，select 能处理的异常情况只有一种：socket 上接收带外数据

## 9.2 poll 系统调用

1. `poll` 系统调用在指定时间内轮询一定数量的文件描述符，以测试其中是否有就绪者
2. `poll` 的原型如下：

    ```c
    #inlucde <poll.h>

    int poll(struct pollfd* fds, )
    ```

3. `pollfds` 结构体的定义如下

    ```c
    struct pollfd {
        int fd;         // 文件描述符
        short events;   // 注册的事件
        short revents;  // 实际发生的事件，由内核填充
    }
    ```

4. `poll` 支持的事件类型如下表

    | 事件 | 描述 | 是否可作为输入 | 是否可作为输出 |
    |:--- |:--- |:------------ |:----------- |
    | POLLIN | 数据（包括普通数据和优先数据）| 是 | 是 |
  
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
