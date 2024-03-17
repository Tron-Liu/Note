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

1. rsyslogd 守护进程既能接受用户进程输出的日志，又能接受日志。用户进程是通过调用 syslog 函数生成系统日志的，该函数将日志输出到一个 UNIX 本地域 socket 类型的文件 /dev/log 中，rsyslogd 则监听该文件以获取用户进程的输出。rsyslogd 守护进程在接收到用户进程或内核输入的日志后，会把它们输出到某些特定的日志文件

### 7.1.2 syslog 函数

1. 应用程序使用 `syslog` 函数与 `rsyslogd` 守护进程通信，`syslog` 函数的定义如下：

    ```c
    #include <syslog.h>

    // priority: 设施值（LOG_USER）与日志级别的按位或
    void syslog(int priority, const char* message, ...);
    ```

2. 日志级别

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

3. 改变 syslog 的默认输出方式，进一步结构化日志内容

    ```c
    #include <syslog.h>

    //    ident: 指定的字符串将被添加到日志消息的日期和时间之后，通常被设置为程序的名字
    //   logopt: 对后续 syslog 调用的行为进行配置，可取下列值的按位或
    //           #define LOG_PID     0x01    // 在日志消息中包含程序 PID
    //           #define LOG_CONS    0x02    // 如果消息不能记录到日志文件，则打印至终端
    //           #define LOG_ODELAY  0x04    // 延迟打开日志给你直到第一次调用 syslog
    //           #define LOG_NDELAY  0x08    // 不延迟打开日志功能
    // facility: 可用来修改 syslog 函数中的默认设施值
    void openlog(const char* ident, int logopt, int facility);

    ```

4. 设置 syslog 的日志掩码

   ```c
   #include <syslog.h>

   // maskpri: 设置日志掩码值，该函数始终会成功，它返回调用进程先前的日志掩码值
   int setlogmask(int maskpri);
   ```

5. 关闭日志功能

   ```c
   #include <syslog.h>

   void closelog();
   ```

## 7.2 用户信息

### 7.2.1 UID、EUID、GID 和 EGID

1. 获取和设置当前进程的真实用户ID（UID）、有效ID（EUID）、真实组ID（GID）和有效组ID（EGID）

   ```c
   #include <sys/types.h>
   #include <unistd.h>

   uid_t  getuid();
   uid_t geteuid();
   gid_t  getgid();
   gid_t getegid();
   int  setuid(uid_t uid);
   int seteuid(uid_t uid);
   int  setgid(gid_t gid);
   int setegid(gid_t gid);
   ```

2. EUID 存在的目的是方便资源访问，对于 set-user-id 程序而言，程序的 EUID 会变成程序所有者的 UID

## 7.3 进程间关系

### 7.3.1 进程组

1. Linux 下每个进程都隶属于一个进程组，因此它们除了 PID 信息外，还有进程组 ID（PGID）

   ```c
   #include <unistd.h>
   
   // 获取指定进程的 PGID
   pid_t getpgid(pid_t pid);
   // 成功返回进程 pid 所属进程组的 PGID，失败则返回 -1 并设置 errno

   // 设置 PGID，将 PID 为 pid 的进程的 PGID 设置为 pid
   // 如果 pid == pgid，则由 pid 指定的进程将被设置为进程组首领
   int setpgid(pid_t pid, pid_t pgid);
   // 成功返回 0，失败则返回 -1 并设置 errno

   // 一个进程只能设置自己或者其子进程的 PGID。
   // 并且，当子进程调用 exec 系列函数后，也不能再在父进程中对它设置 PGID
   ```

2. 每个进程组都有一个首领进程，其 PID 与 PGID 相同。进程组将一直存在，直到其中所有进程都退出，或者加入到其他进程组

### 7.3.2 会话

1. 一些有关联的进程组将形成一个绘画（session）

   ```c
   #include <unistd.h>

   pid_t setsid(void);
   // 该函数不能由进程组的首领进程调用，否则将产生一个错误
   // 对于非组首领的进程，调用该函数不仅创建新会话，而且会有如下额外效果：
   //    调用进程会成为会话的首领，此时该进程是新会话的唯一成员
   //    新建一个进程组，其 PGID 就是调用进程的 PID，调用进程成为该组的首领
   //    调用进程将甩开终端（如果有的话）

   // 该函数成功时返回新的进程组 PGID，失败则返回 -1 并设置 errno

   // 读取 SID
   pid_t getsid(pid_t pid);
   ```

## 7.4 系统资源限制

1. 获取和设置 Linux 系统资源限制的函数对：

   ```c
   #include <sys/resource.h>

   int getrlimit(int resource, struct rlimit *rlim);
   int setrlimit(int resource, const struct rlimit *rlim);

   struct rlimit {
     rlim_t rlim_cur;
     rlim_t rlim_max;
   }
   // rlim_t: 整数类型，描述资源级别
   // rlim_cur: 指定资源的软限制（建议性的，最好不要超越的限制，超越的话，系统可能向进程发送信号以终止其运行）
   // rlim_max: 指定资源的硬限制，普通程序可以减少硬限制，root 身份运行的程序才能增加硬限制
   ```

## 7.5 改变工作目录和根目录

1. 获取和改变进程工作目录的函数对：

   ```c
   #include <unistd.h>

   char* getcwd(char* buf, size_t size);
   int chdir(const char* path);
   ```

2. 改变进程根目录的函数：

   ```c
   #include <unistd.h>

   int chroot(const char* path);
   ```

3. chroot 并不改变进程的当前工作目录，所以调用 chroot 之后，仍然需要使用 chdir("/") 来将工作目录切换至新的根目录。改变进程的根目录之后，程序可能无法访问类似 /dev 的文件（和目录），因为这些文件（和目录）并非处于新的根目录之下，不过可以利用进程原先打开的文件描述符来访问调用 chroot 之后不能直接访问的文件（和目录）
4. 只有特权进程才能改变跟目录
