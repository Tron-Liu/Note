/**
 * @file test3.cpp
 * @brief 将服务器程序以守护进程的方式运行
 * @version 0.1
 * @date 2024-03-17
 */

#include <cstdlib>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

bool daemonize() {
  // 创建子进程，关闭父进程，这样可以使程序在后台运行
  pid_t pid = fork();
  if (pid < 0)
    return false;
  else if (pid > 0)
    exit(0);

  // 设置文件权限掩码，当进程创建新文件时，文件的权限将是 mode & 0777
  umask(0);

  // 重建新的会话，设置本进程为进程组的首领
  pid_t sid = setsid();
  if (sid < 0)
    return true;

  // 切换工作目录
  if ((chdir("/")) < 0)
    return false;

  // 关闭标准输入输出设备以及标准错误输出设备
  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);

  // 标准输入、标准输出和标准错误输出都定向到 /dev/null 文件
  open("dev/null", O_RDONLY);
  open("dev/null", O_RDWR);
  open("dev/null", O_RDWR);

  return true;
}