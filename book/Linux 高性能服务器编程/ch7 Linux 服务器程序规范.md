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
