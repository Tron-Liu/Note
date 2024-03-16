/**
 * @file: test3.cpp
 * @author: Tron-Liu (Tron-Liu@foxmail.com)
 * @brief: 使用 sendfile 传输文件
 * @date: 2022-09-18
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
  if (argc <= 3) {
    printf("usage: %s ip_address port_number filename\n.", basename(argv[1]));
    return 1;
  }

  const char* ip = argv[1];
  int port = atoi(argv[2]);
  const char* file_name = argv[3];

  int filefd = open(file_name, O_RDONLY);
  assert(filefd > 0);
  struct stat stat_buf;
  fstat(filefd, &stat_buf);

  struct sockaddr_in address;
  bzero(&address, sizeof(address));
  address.sin_family = AF_INET;
  inet_pton(AF_INET, ip, &address.sin_addr);
  address.sin_port = htons(port);

  int sock = socket(PF_INET, SOCK_STREAM, 0);
  assert(sock >= 0);

  int ret = bind(sock, (struct sockaddr*)&address, sizeof(address));
  assert(ret != -1);

  ret = listen(sock, 5);
  assert(ret != -1);

  struct sockaddr_in client;
  socklen_t client_addrlength = sizeof(client);
  int connfd = accept(sock, (struct sockaddr*)&client, &client_addrlength);
  if (connfd < 0)
    printf("errno is %d.\n", errno);
  else {
    sendfile(connfd, filefd, NULL, stat_buf.st_size);
    close(connfd);
  }

  close(sock);
  return 0;
}
