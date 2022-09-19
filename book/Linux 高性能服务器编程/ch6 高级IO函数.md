# 第6章 高级 I/O 函数

## 6.1 pipe 函数

1. `pipe`函数可用于创建一个管道，以实现进程间通信

    ```cpp
    #include <unistd.h>
    int pipe(int fd[2]);
    ```

2. `pipe`函数的参数是一个包含两个 int 型整数的数组指针。该函数成功时返回 0，并将一对打开的文件描述符值填入其参数指向的数组。如果失败，则返回 -1 并设置 `errno`
3. 通过`pipe`函数创建的这两个文件描述符`fd[0]`和`fd[1]`分别构成管道的两端。`fd[0]`只能用于从管道读出数据，`fd[1]`只能用于往管道写入数据，而不能反过来使用 `socket`的基础 API 中有一个 `socketpair` 函数。它能够方便地创建双向管道。定义如下：

    ```cpp
    #include <sys/type.h>
    #include <sys/socket.h>
    int socketpair(int domain, int type, int protocol, int fd[2])

    domain: 只能使用 UNIX 本地域协议族 AF_UNIX
    成功返回0，失败返回-1并设置 errno
    ```

## 6.3 readv 函数和 writev 函数

1. readv 函数将数据从文件描述符读到分散的内存块中，即分散读
2. writev 函数将多块分散的内存数据一并写入文件描述符中

    ```cpp
    #include <sys/uio.h>
    ssize_t readv(int fd, const struct iovec* vector, int count);
    ssize_t writev(int fd, const struct iovec* vector, int count);

    iovec: 结构体，描述一块内存区
    count: vector 数组的长度

    函数成功返回读出（写入）fd 的字节数，失败返回-1并设置 errno
    ```

## 6.4 sendfile 函数

1. `sendfile` 函数在两个文件描述符之间直接传递数据（完全在内核中操作），从而避免了内核缓冲区和用户缓冲区之间的数据拷贝，效率高，被称为**零拷贝**
2. `sendfile` 函数的定义如下：

    ```cpp
    #include <sys/sendfile.h>
    ssize_t sendfile(int out_fd, int in_fd, off_t* offset, size_t count);

    in_fd: 待读出内容的文件描述符
    out_fd: 待写入内容的文件描述符
    offset: 指定从读入文件流的哪个位置开始读，如果为空，则使用读入文件流默认的起始位置
    count: 指定在文件描述符 in_fd 和 out_fd 之间传输的字节数

    成功时返回传输的字节数，失败返回-1并设置errno
    ```

3. in_fd: 必须指向真实的文件，不能是 socket 和管道；out_fd: 必须是一个 socket。由此可见，`sendfile` 几乎是专门为在网络上传输文件而设计的

## 6.5 mmap 函数和 munmap 函数

1. `mmap` 函数用于申请一段内存空间。我们可以将这段内存作为进程间通信的共享内存，也可以将文件直接映射到其中。`munmap` 函数则释放由 `mmap` 创建的这段内存空间。
2. 函数定义如下：

    ```cpp
    #include <sys/mman.h>
    void* mmap(void *start, size_t length, int prot, int flags, int fd, off_t offset);
    int munmap(void *start, size_t length);

    start: 允许用户使用某个特定的地址作为这段内存的起始地址。如果被设置成 NULL，则系统自动分配一个地址 
    length: 指定内存段的长度
    prot: 设置内存段的访问权限
    flags: 空间内存段内容被修改后程序的行为
    fd: 被映射文件对应的文件描述符
  
    mmap 函数成功返回指向目标内存区域的指针，失败则返回MAP_FAILED（(void*)-1）并设置errno
    munmap 函数成功返回0，失败返回-1并设置errno
    ```

## 6.6 splice 函数

1. `splice` 函数由于在两个文件描述符之间移动数据，**零拷贝操作**
2. 函数定义如下：

    ```cpp
    #include <fcntl.h>
    ssize_t splice(int fd_in, loff_t* off_in, int fd_out, loff_t* off_out, size_t len, unsigned int flags);

    fd_in: 待输入数据的文件描述符。
           如果fd_in是一个管道文件描述符，那么参数off_in必须被设置为NULL。
           如果fd_in不是一个管道文件描述符，那么off_in表示从输入数据流的何处开始读取数据。
    off_in: 当fd_in不是管道文件描述符时，off_in被设置为NULL表示从输入数据流的当前偏移位置读入，off_in不为NULL，则它将指出具体的偏移位置
    fd_out/off_out: 含义与fd_in/off_in相同，不过用于输出数据流
    len: 指定移动数据的长度
    flags: 控制数据如何移动

    成功返回移动字节的数量，失败返回-1并设置errno
    ```

## 6.7 tee 函数

1. `tee` 函数在两个管道文件描述符之间复制数据，**零拷贝操作**
2. 函数原型如下：

    ```cpp
    #include <fcntl.h>
    ssize_t tee(int fd_in, int fd_out, size_t len, unsigned int flags);

    fd_in、fd_out: 管道文件描述符
    成功时返回在两个文件描述符之间复制的数据数量（字节数）、失败时返回-1并设置errno
    ```

## 6.8 fcntl 函数

1. 将信号和文件描述符关联的方法，就是使用 fcntl 函数为目标文件描述符制定宿主进程或进程组，那么被指定的宿主进程或进程组将捕获这两个信号。
2. 函数的定义：

    ```cpp
    #include <fcntl.h>
    int fcntl(int fd, int cmd, ...);

    fd: 被操作的文件描述符
    cmd: 指定执行何种类型的操作
    ```
