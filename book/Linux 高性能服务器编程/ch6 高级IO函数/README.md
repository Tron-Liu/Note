# 第6章 高级 I/O 函数

## 6.1 pipe 函数

1. `pipe`函数可用于创建一个管道，以实现进程间通信

    ```cpp
    #include <unistd.h>
    int pipe(int fd[2]);
    ```

2. `pipe`函数的参数是一个包含两个 int 型整数的数组指针。该函数成功时返回 0，并将一对打开的文件描述符值填入其参数指向的数组。如果失败，则返回 -1 并设置 `errno`
3. 通过`pipe`函数创建的这两个文件描述符`fd[0]`和`fd[1]`分别构成管道的两端。`fd[0]`只能用于从管道读出数据，`fd[1]`只能用于往管道写入数据，而不能反过来使用
4. `socket`的基础 API 中有一个 `socketpair` 函数。它能够方便地创建双向管道，socketpair 创建的这对文件描述符都是既可读又可写的，定义如下：

    ```cpp
    #include <sys/type.h>
    #include <sys/socket.h>

    // domain: 只能使用 UNIX 本地域协议族 AF_UNIX
    int socketpair(int domain, int type, int protocol, int fd[2])

    // 成功返回0，失败返回-1并设置 errno
    ```

## 6.2 dup 函数和 dup2 函数

1. 有时我们希望把标准输入重定向到一个文件，或者把标准输出重定向到一个网络连接

    ```cpp
    #include <unistd.h>

    // dup 函数创建一个新的文件描述符，该新文件描述符和原有文件描述符 file_descriptor 指向相同的文件、管道或者网络连接
    //     dup 返回的文件描述符总是取系统当前可用的最小整数值
    int dup(int file_descriptor);
    // dup2 与 dup 类似，不过该函数返回第一个不小于 file_descriptor_two 的整数值
    int dup2(int file_descriptor_one, int file_descriptor_two);

    // duo 和 dup2 创建的文件描述符并不继承原文件描述符的属性
    ```

2. `dup` 函数创建一个新的文件描述符，该新文件描述符和原有文件描述符 `file_descriptor` 指向相同的文件、管道或网络连接

## 6.3 readv 函数和 writev 函数

1. readv 函数将数据从文件描述符读到分散的内存块中，即分散读
2. writev 函数将多块分散的内存数据一并写入文件描述符中

    ```cpp
    #include <sys/uio.h>

    // iovec: 结构体，描述一块内存区
    // count: vector 数组的长度
    ssize_t readv(int fd, const struct iovec* vector, int count);
    ssize_t writev(int fd, const struct iovec* vector, int count);

    // 函数成功返回读出（写入）fd的字节数，失败返回-1并设置errno
    ```

## 6.4 sendfile 函数

1. `sendfile` 函数在两个文件描述符之间直接传递数据（完全在内核中操作），从而避免了内核缓冲区和用户缓冲区之间的数据拷贝，效率高，被称为**零拷贝**
2. `sendfile` 函数的定义如下：

   ```cpp
   #include <sys/sendfile.h>

   // out_fd: 待写入内容的文件描述符
   //  in_fd: 待读出内容的文件描述符
   // offset: 指定从读入文件流的哪个位置开始读，如果为空，则使用读入文件流默认的起始位置
   //  count: 指定在文件描述符 in_fd 和 out_fd 之间传输的字节数
   ssize_t sendfile(int out_fd, int in_fd, off_t* offset, size_t count);

   // 成功时返回传输的字节数，失败返回 -1 并设置 errno
   ```

3. in_fd: 必须指向真实的文件，不能是 socket 和管道；out_fd: 必须是一个 socket。由此可见，`sendfile` 几乎是专门为在网络上传输文件而设计的

## 6.5 mmap 函数和 munmap 函数

1. `mmap` 函数用于申请一段内存空间。我们可以将这段内存作为进程间通信的共享内存，也可以将文件直接映射到其中。`munmap` 函数则释放由 `mmap` 创建的这段内存空间。
2. 函数定义如下：

    ```cpp
    #include <sys/mman.h>

    //  start: 允许用户使用某个特定的地址作为这段内存的起始地址。如果被设置成 NULL，则系统自动分配一个地址 
    // length: 指定内存段的长度
    //   prot: 设置内存段的访问权限，可取以下几个值的 按位或
    //         PROT_READ : 内存段可读
    //         PROT_WRITE: 内存段可写
    //         PROT_EXEC : 内存段可执行
    //         PROT_NONE : 内存段不能被访问
    //  flags: 空间内存段内容被修改后程序的行为
    //     fd: 被映射文件对应的文件描述符
    // offset: 设置从文件的何处开始映射
    void* mmap(void *start, size_t length, int prot, int flags, int fd, off_t offset);
    int munmap(void *start, size_t length);
  
    // mmap 函数成功返回指向目标内存区域的指针，失败则返回 MAP_FAILED（(void*)-1）并设置errno
    // munmap 函数成功返回 0，失败返回 -1 并设置 errno
    ```

## 6.6 splice 函数

1. `splice` 函数用于在两个文件描述符之间移动数据，**零拷贝操作**
2. 函数定义如下：

    ```cpp
    #include <fcntl.h>

    //  fd_in: 待输入数据的文件描述符。
    //         如果 fd_in 是一个管道文件描述符，那么参数 off_in 必须被设置为 NULL
    //         如果不是，那么 off_in 表示从输入数据流的何处开始读取数据
    // off_in: 当 fd_in 不是管道文件描述符时，
    //         off_in 被设置为 NULL 表示从输入数据流的当前偏移位置读入，
    //         off_in 不为 NULL，则它将指出具体的偏移位置
    // fd_out/off_out: 含义与 fd_in/off_in 相同，不过用于输出数据流
    //    len: 指定移动数据的长度
    //  flags: 控制数据如何移动，可以被设置为表6-2某些值的 按位或
    ssize_t splice(int fd_in, loff_t* off_in, int fd_out, loff_t* off_out, size_t len, unsigned int flags);

    // 使用 splice 函数时，fd_in 和 fd_out 必须至少有一个是管道文件描述符
    // 成功返回移动字节的数量，失败返回 -1 并设置 errno
    ```

3. 表6-2 splice 的 flags 参数的常用值及其含义

    |       常用值       |       含义      |
    | :----------------:|:-------------- |
    | SPLICE_F_MOVE     | 如果合适的话，按整页内存移动数据。这只是给内核的一个提示。不过，因为它的实现存在 BUG，所以自内核 2.6.21 后，它实际上没有任何效果 |
    | SPLICE_F_NONBLOCK | 非阻塞的 splice 操作，但实际效果还会受文件描述符本身的阻塞状态的影响 |
    | SPLICE_F_MORE     | 给内核一个提示：后续的 splice 调用将读取更多的数据 |
    | SPLICE_F_GIFT     | 对 splice 没有效果 |

## 6.7 tee 函数

1. `tee` 函数在两个管道文件描述符之间复制数据，**零拷贝操作**
2. 函数原型如下：

    ```cpp
    #include <fcntl.h>

    // fd_in/fd_out: 管道文件描述符
    ssize_t tee(int fd_in, int fd_out, size_t len, unsigned int flags);

    // 成功时返回在两个文件描述符之间复制的数据数量（字节数）
    // 失败时返回 -1 并设置 errno
    ```

## 6.8 fcntl 函数

1. 将信号和文件描述符关联的方法，就是使用 fcntl 函数为目标文件描述符制定宿主进程或进程组，那么被指定的宿主进程或进程组将捕获这两个信号。
2. 函数的定义：

    ```cpp
    #include <fcntl.h>

    //  fd: 被操作的文件描述符
    // cmd: 指定执行何种类型的操作
    // 根据操作类型的不同，该函数可能还需要第三个可选参数 arg
    int fcntl(int fd, int cmd, ...);
    ```

3. fcntl 支持的常用操作及其参数

    | 操作分类 | 操作 | 含义 | 第三个参数的类型 | 成功时的返回值 |
    |:------ |:---- |:----|:-------------:|:------------|
    | 复制文件描述符 | F_DUPFD | 创建一个新的文件描述符，其值大于或等于 arg | long | 新创建的文件描述符的值 |
    | 复制文件描述符 | F_DUPFD_CLOEXEC | 与 F_DUPFD 相似，不过在创建文件描述符的同时，设置其 close-on-exec 标志 | long | 新创建的文件描述符的值 |
    | 获取文件描述符的标志 | F_GETFD | 获取 fd 的标志，比如 close-on-exec 标志 | 无 | fd 的标志 |
    | 设置文件描述符的标志 | F_SETFD | 设置 fd 的标志 | long | 0 |
    | 获取文件描述符的状态标志 | F_GETFL | 获取 fd 的状态标志，这些标志包括可由 open 系统调用设置的标志和访问模式 | void | fd 的状态标志 |
    | 设置文件描述符的状态标志 | F_SETFL | 设置 fd 的状态标志，但部分标志是不能被修改的（比如访问模式标志）| long | 0 |
    | 管理信号 | F_GETOWN | 获得 SIGIO 和 SIGURG 信号的宿主进程的 PID 或进程组的组 ID | 无 | 信号的宿主进程的 PID 或进程组的组 ID |
    | 管理信号 | F_SETOWN | 设定 SIGIO 和 SIGURG 信号的宿主进程的 PID 或进程组的组 ID | long | 0 |
    | 管理信号 | F_GETSIG | 获取当应用程序被通知 fd 可读或可写时，是哪个信号通知该事件的 | 无 | 信号值，0 表示 SIGIO |
    | 管理信号 | F_SETSIG | 设置当 fd 可读或可写时，系统应该触发哪个信号来通知应用程序 | long | 0 |
    | 操作管道容量 | F_SETPIPE_SZ | 设置由 fd 指定的管道的容量 | long | 0 |
    | 操作管道容量 | F_GETPIPE_SZ | 获取由 fd 指定的管道的容量 | 无 | 管道容量 |

在网络编程中，fcntl 函数通常用来将一个文件描述符设置为非阻塞
