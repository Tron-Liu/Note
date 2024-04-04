# 第13章 多进程编程

## 13.1 fork 系统调用

1. Linux 下创建新进程的系统调用是 fork。该函数的每次调用都返回两次，在父进程中返回的是子进程的 PID，在子进程中则返回 0。该返回值是后续代码判断当前进程是父进程还是子进程的依据，其定义如下：

   ```c
   #include <sys/types.h>
   #include <unistd.h>

   pid_t fork(void);

   // 失败返回 -1
   ```

## 13.2 exec 系列系统调用

1. 有时我们需要在子进程中执行其他程序，即替换当前进程映像，这就需要使用 `exec` 系列函数

   ```c
   #include <unistd.h>
   extern char** environ;

   // path: 可执行文件的完整路径
   // file: 可以接收文件名，该文件的具体位置则在环境变量 PATH 中搜寻
   // arg : 可变参数
   // argv: 接收参数数组，与arg会被传递给新程序的main函数
   // envp: 用于设置新程序的环境变量
   int execl(const char* path, const char* arg, ...);
   int execlp(const char* file, const char* arg, ...);
   int execle(const char* path, const char* arg, ..., char* const envp[]);
   int execv(const char* file, char* const argv[]);
   int execvp(const char* file, char* const argv[]);
   int execve(const char* path, char* const argv[], char* const envp[]);

   // 出错返回-1，并设置errno
   ```

## 13.3 处理僵尸进程

1. 对于多进程程序而言，父进程一般需要跟踪子进程的退出状态。因此，当子进程结束运行时，内核不会立即释放该进程的进程表表项，以满足父进程后续对该子进程退出信息的查询（如果父进程还在运行）。**在子进程结束运行之后，父进程读取其退出状态之前，我们称该子进程处于僵尸态**
2. 另一种使子进程进入僵尸态的情况是：父进程结束或者异常终止，而子进程继续运行。在父进程退出之后，子进程退出之前，该子进程处于僵尸态
3. 下面这对函数在父进程中调用，以等待子进程的结束，并获取子进程的返回信息，从而避免了僵尸进程的产生，或者使子进程的僵尸态立即结束

   ```c
   #include <sys/types.h>
   #include <sys/wait.h>

   // wait 函数将阻塞进程，直到该进程的某个子进程结束运行为止
   // 返回结束运行的子进程的 PID，并将该子进程的退出状态信息存储于 stat_loc 参数指向的内存中
   pid_t wait(int* stat_loc);
   //  pid: waitpid 只等待由 pid 指定的子进程，如果 pid=-1， 则和 wait 函数相同，即等待任意一个子进程结束
   // stat_loc: 与 wait 函数的 stat_loc 参数相同
   // options: 一般取 WNOHANG，表示 waitpid 调用是非阻塞的
   pid_t waitpid(pid_t pid, int* stat_loc, int options);
   // 如果 pid 指定的目标子进程还没有结束或意外终止，则 waitpid 立即返回 0
   // 如果目标子进程确实正常退出了，则 waitpid 返回该子进程的 PID
   // 调用失败时返回 -1，并设置 errno
   ```

## 13.4 管道

1. 管道能在父子进程间传递数据，利用的是 fork 调用之后两个管道文件描述符都保持打开

## 13.5 信号量

### 13.5.1 信号量原语

1. 假设有信号量 SV，则对它的 P、V 操作含义如下：
   * P(SV): 如果 SV 的值大于 0，就将它减 1；如果 SV 的值为 0，则挂起进程的执行
   * V(SV): 如果有其他进程因为等待 SV 而挂起，则唤醒；如果没有，则将 SV 加一

## 13.7 消息队列

### 13.7.1 msgget 系统调用

1. msgget 系统调用创建一个消息队列，或者获取一个已有的消息队列

### 13.7.2 msgsnd 系统调用

1. msgsnd 系统调用把一条消息加到消息队列中

### 13.7.3 msgrcv 系统调用

1. msgrcv 系统调用从消息队列中获取消息

### 13.7.4 msgctl 系统调用

1. msgctl 系统调用控制消息队列的某些属性

## 13.8 IPC 命令

1. 使用 `sudo ipcs` 命令可观察当前系统上拥有哪些共享资源实例
2. 使用 `ipcrm` 命令可删除遗留在系统中的共享资源

## 13.9 在进程间传递文件描述符

传递一个文件描述符并不是传递一个文件描述符的值，而是要在接收进程中创建一个新的文件描述符，并且该文件描述符和发送进程中被传递的文件描述符指向内核中相同的文件表项

在 Linux 下，可以利用 UNIX 域 socket 在进程间传递特殊的辅助数据，以实现文件描述符的传递】
