/**
 * @file test1.cpp
 * @brief 测试进程的 UID 和 EUID 的区别
 * @version 0.1
 * @date 2024-03-16
 */

#include <unistd.h>
#include <stdio.h>

int main()
{
  uid_t uid = getuid();
  uid_t euid = geteuid();

  printf("userid is: %d, effective userid is: %d\n", uid, euid);

  return 0;
}