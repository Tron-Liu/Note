# 第13章 多进程编程

## 13.1 fork 系统调用

1. Linux 下创建新进程的系统调用是 fork。该函数的每次调用都返回两次，在父进程中返回的是子进程的 PID，在子进程中则返回 0。该返回值是后续代码判断当前进程是父进程还是子进程的依据
2. 其定义如下：

   ```c
   #include <sys/types.h>
   #include <unistd.h>

   pid_t fork(void);

   失败返回 -1
   ```

## 13.2 exec 系列系统调用

1. 有时我们需要在子进程中执行其他程序，即替换当前进程映像，这就需要使用 `exec` 系列函数

   ```c
   #include <unistd.h>
   extern char** environ;

   int execl(const char* path, const char* arg, ...);
   int execlp(const char* file, const char* arg, ...);
   int execle(const char* path, const char* arg, ..., char* const envp[]);
   int execv(const char* file, char* const argv[]);
   int execvp(const char* file, char* const argv[]);
   int execve(const char* path, char* const argv[], char* const envp[]);

   path: 可执行文件的完整路径
   file: 可以接收文件名，该文件的具体位置则在环境变量 PATH 中搜寻
   arg: 可变参数
   argv: 接收参数数组，与arg会被传递给新程序的main函数

   出错返回-1，并设置errno
   ```

## 13.3 处理僵尸进程

1. 对于多进程程序而言，父进程一般需要跟踪子进程的退出状态。因此，当子进程结束运行时，内核不会立即释放该进程的进程表表项，以满足父进程后续对该子进程退出信息的查询（如果父进程还在运行）。**在子进程结束运行之后，父进程读取其退出状态之前，我们称该子进程处于僵尸态**
2. 另一种使子进程进入僵尸态的情况是：父进程结束或者异常终止，而子进程继续运行

## 13.5 信号量

### 13.5.1 信号量原语

1. 假设有信号量 SV，则对它的 P、V 操作含义如下：
   * P(SV): 如果 SV 的值大于 0，就将它减 1；如果 SV 的值为 0，则挂起进程的执行
   * V(SV): 如果有其他进程因为等待 SV 而挂起，则唤醒；如果没有，则将 SV 加一
