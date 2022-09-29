/**
 * @file: 5-6.cpp
 * @author: Tron-Liu (Tron-Liu@foxmail.com)
 * @brief: 发送带外数据
 * @date: 2022-09-15
 *
 * @copyright Copyright (c) 2022
 *
 **/

#include <arpa/inet.h>
#include <assert.h>
#include <libgen.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
  if (argc <= 2) {
    printf("usage: %s ip_address port_number\n", basename(argv[0]));
    return 1;
  }
  const char* ip = argv[1];
  int port = atoi(argv[2]);

  struct sockaddr_in server_address;
  bzero(&server_address, sizeof(server_address));
  server_address.sin_family = AF_INET;
  inet_pton(AF_INET, ip, &server_address.sin_addr);
  server_address.sin_port = htons(port);

  int sockfd = socket(PF_INET, SOCK_STREAM, 0);
  assert(sockfd >= 0);
  if (connect(sockfd, (struct sockaddr*)&server_address,
              sizeof(server_address)) < 0) {
    printf("connection failed\n");
  } else {
    char* normal_data = "123";
    char* oob_data = "abc";

    send(sockfd, normal_data, strlen(normal_data), 0);
    send(sockfd, oob_data, strlen(oob_data), MSG_OOB);
    send(sockfd, normal_data, strlen(normal_data), 0);
  }

  close(sockfd);
  return 0;
}
