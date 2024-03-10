/**
 * @file test4.cpp
 * @author Tron-Liu (Tron-Liu@foxmail.com)
 * @brief 使用共享内存的聊天室服务器程序
 * @version 0.1
 * @date 2024-03-10
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <arpa/inet.h>
#include <iostream>
#include <libgen.h>
#include <netinet/in.h>
#include <sys/socket.h>

int main(int argc, char *argv[]) {
  if (argc <= 2) {
    printf("usage: ./%s ip_address port_number\n", basename(argv[0]));
    return 1;
  }

  const char *ip = argv[1];
  int port = atoi(argv[2]);

  int ret = 0;
  struct sockaddr_in address;
  address.sin_family = AF_INET;
  inet_pton(AF_INET, ip, &address.sin_addr);
  address.sin_port = htons(port);
}