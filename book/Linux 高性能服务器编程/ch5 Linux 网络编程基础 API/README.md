# 第5章 Linux 网络编程基础 API

## 5.1 socket 地址 API

### 5.1.1 主机字节序和网络字节序

1. 现代 PC 大多采用小端字节序，因此小端字节序又被称为主机字节序
2. 当格式化的数据在两台使用不同字节序的主机之间直接传递时，接收端必然进行错误地解释。解决办法是：发送端总是要把发送地数据转化成大端字节序数据后再发送，因此大端字节序也称为网络字节序。
3. 大端字节序是指一个整数的高位字节存放在内存的低地址处，低位字节存储在内存的高地址处。小端字节序则是指整数的高位字节序存储在内存的高地址处，而低位字节则存在内存的低地址处
4. Linux 提供了 4 个函数来完成主机字节序和网络字节序之间的转换

   ```c
   #include <netinet/in.h>
   // host to network long
   unsigned long int htonl(unsigned long int hostlong);
   unsigned short int htons(unsigned short int hostshort);
   unsigned long int ntohl(unsigned long int netlong);
   unsigned short int ntohs(unsigned short int netshort);
   // 在这4个函数中，长整型函数通常用来转换IP地址，短整型函数通常用来转换端口号
   ```

### 5.1.3 专用 socket 地址

1. UNIX 本地域协议族专用 socket 地址结构体

   ```c
   #include <sys/un.h>
   struct sockaddr_un
   {
     sa_family_t sin_family;    // 地址族：AF_UNIX
     char sun_path[108];        // 文件路径名
   };
   ```

2. IPv4 专用 socket 地址结构体

   ```c
   struct in_addr
   {
    sa_family_t sin_family;     // 地址族：AF_INET
    u_int6_t sin_port;          // 端口号，要用网络字节序表示
    struct in_addr sin_addr;    // IPv4 地址结构体
   };
   struct in_addr
   {
     u_int32_t s_addr;          // IPv4 地址，要用网络字节序表示
   };
   ```

3. IPv6 专用 socket 地址结构体

   ```c
   struct in6_addr
   {
     sa_family_t sin6_family;     // 地址族：AF_INET6
     u_int6_t sin6_port;          // 端口号，用网络字节序表示
     u_int32_t sin6_flowinfo;     // 流信息，应设置为 0
     struct in6_addr sin6_addr;   // IPv6 地址结构体
     u_int32_t sin6_scope_id;     // scope ID
   };
   struct in6_addr
   {
    unsigned char sa_addr[16];    // IPv6 地址，用网络字节序表示
   };
   ```

所有专用 socket 地址类型的变量在实际使用时都需要转化为通用 socket 地址类型 sockaddr （强制转换即可）

### 5.1.4 IP 地址转换函数

1. 下面这对函数同时适用于 IPv4 地址和 IPv6 地址

   ```c
   #include <arpa/inet.h>

   // 该函数将用字符串表示的 IP 地址 src 转换成用网络字节序整数表示的 IP 地址，
   // 并把转换结果存储于 dst 指向的内存中
   // af: 指定地址族
   int inet_pton(int af, const char* src, void* dst);
   // 该函数于上述相反
   // cnt: 指定目标存储单元的大小
   const char* inet_ntop(int af, const void* src, char* dst, socklen_t cnt);

   // 下面两个宏可指定 cnt 大小
   #include <netinet/in.h>
   #define INET_ADDRSTRLEN 16
   #define INET6_ADDRSTRLEN 46
   ```

## 5.2 创建 socket

```c
#include <sys/types.h>
#include <sys/socket.h>

int socket(int domain, int type, int protocol);
// domain: 告诉系统使用哪个底层协议族
//         PF_INET  : IPv4
//         PF_INET6 : IPv6
//         PF_UNIX  : UNIX 本地域协议族
//   type: 指定服务类型
//         SOCK_STREAM : TCP
//         SOCK_UGRAM  : UDP
// protocol: 通常情况下，默认 0
```

## 5.3 命名 socket

1. 创建 socket 时，虽然指定了地址族，但并未指定使用该地址族中的哪个具体 socket 地址。将一个 socket 与 socket 地址绑定称为命名 socket

   ```c
   #include <sys/types.h>
   #include <sys/socket.h>

   int bind(int sockfd, const struct sockaddr* my_addr, socklen_t addrlen);
   // bind 成功返回 0，失败返回 -1 并设置 errno，常见的分别是 EACCES、EADDRINUSE
   // EACCES      : 被绑定的地址是受保护的地址，仅超级用户能够访问
   // EADDRINUSE  : 被绑定的地址正在使用中
   ```

## 5.4 监听 socket

1. 使用 listen 创建一个监听队列以存放待处理的客户连接

   ```c
   #include <sys/socket.h>
   
   // sockfd: 指定被监听的 socket
   // backlog: 提示内核监听队列的最大长度，典型值为 5
   // 监听队列的长度如果超过 backlog，服务器将不受理新的客户连接，客户端也将收到 ECONNREFUSED 错误信息
   int listen(int sockfd, int backlog);
   ```

## 5.5 接受连接

1. `accept` 系统调用从 listen 监听队列中接受一个连接

   ```c
   #include <sys/types.h>
   #include <sys/socket.h>
   
   // sockfd: 执行过 listen 系统调用的监听 socket
   // addr: 获取被接受连接的远端 socket 地址
   // addrlen: 指定远端 socket 的地址长度
   int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
   ```

2. 把执行过 listen 调用，处于 LISTEN 状态的 socket 称为监听 socket，而所有处于 ESTABLISHED 状态的 socket 则称为连接 socket

## 5.6 发起连接

1. 客户端通过 `connect` 系统调用来主动与服务器建立连接

   ```c
   #include <sys/types.h>
   #include <sys/socket.h>

   // sockfd: 由 socket 系统调用返回一个 socket
   // serv_addr: 服务器监听的 socket 地址，addrlen 则指定这个地址的长度
   int connect(int sockfd, const struct sockaddr *serv_addr, socklent_t addrlen);

   // connect 成功时返回 0，sockfd 则唯一标识这个连接
   //         失败返回 -1，常用 errno 如下
   //                    ECONNREFUSED: 目标端口不存在
   //                    ETIMEDOUT: 连接超时
   ```

## 5.7 关闭连接

1. 关闭一个连接实际上就是关闭该连接对应的 socket

   ```c
   #include <unistd.h>

   // fd: 待关闭的 socket
   int close(int fd);
   // close 并非总是立即关闭一个连接，而是将 fd 的引用计数减 1。只有 fd 的引用计数为 0 时，才真正关闭连接
   //     多进程程序中，一次 fork 系统调用默认将使父进程中打开的 socket 的引用计数加 1，因此必须在父子进程中
   //     都对该 socket 执行 close 调用才能将连接关闭
   ```

2. `shutdown` 可立即终止连接

   ```c
   #include <sys/socket.h>

   int shutdown(int sockfd, int howto);
   // howto 可选值: 
   //      SHUT_RD 关闭 sockfd 上读的这一半。应用程序不能再针对 socket 文件描述符执行该操作
   //              并且该 socket 接收缓冲区中的数据都被丢弃
   //      SHUT_WR 关闭 sockfd 上写的这一半。socket 的发送缓冲区中的数据会在真正关闭连接之前全部发送出去
   //              应用程序不可再对该 socket 文件描述符执行写操作。这种情况下，连接处于半关闭状态
   //      SHUT_RDWR 同时关闭 sockfd 上的读和写
   ```

## 5.8 数据读写

### 5.8.1 TCP 数据读写

1. 用于 TCP 流数据读写的系统调用是：

   ```c
   #include <sys/types.h>
   #include <sys/socket.h>

   ssize_t recv(int sockfd, void *buf, size_t len, int flags);
   ssize_t send(int sockfd, const void *buf, size_t len, int flags);
   ```

### 5.8.2 UDP 数据读写

1. 用于 UDP 数据报读写的系统调用是：

   ```c
   #include <sys/types.h>
   #include <sys/socket.h>

   ssize_t recvfrom(int sockfd, void* buf, size_t len, int flags, 
                    struct sockaddr* src_addr, socklen_t* addrlen);
   ssize_t sendto(int sockfd, const void* buf, size_t len, int flags,
                  const struct sockaddr* dest_addr, socklen_t addrlen);
   ```

### 5.8.3 通用数据读写函数

1. 下述系统调用可同时用于 TCP 和 UDP 数据读写

   ```c
   #include <sys/socket.h>

   ssize_t recvmsg(int sockfd, struct msghdr* msg, int flags);
   ssize_t sendmsg(int sockfd, struct msghdr* msg, int flags);
   ```

## 5.11 socket 选项

### 5.11.2 SO_RCVBUF 和 SO_SNDBUF 选项

1. SO_RCVBUF 和 SO_SNDBUF 选项分别表示 TCP 接收缓冲区和发送缓冲区的大小。不过，当我们用 setsockopt 来设置 TCP 的接收缓冲区和发送缓冲区的大小时，系统都会将其值加倍

### 5.11.3 SO_RCVLOWAT 和 SO_SNDLOWAT 选项

1. `SO_RCVLOWAT` 和 `SO_SNDLOWAT` 选项分别表示 TCP 接收缓冲区和发送缓冲区的低水位标记。它们一般被 I/O 复用系统调用用来判断 socket 是否可读或可写
2. 当 TCP 接收缓冲区中可读数据的总数大于其低水位标记时，I/O 复用系统调用将通知应用程序可以从对应的 socket 上读取数据；当 TCP 发送缓冲区中的空闲空间（可以写入数据的空间）大于其低水位标记时，I/O 复用系统调用将通知应用程序可以往对应的 socket 上写入数据
3. 默认情况下，TCP 接收（发送）缓冲区的低水位标记均为 1 字节
