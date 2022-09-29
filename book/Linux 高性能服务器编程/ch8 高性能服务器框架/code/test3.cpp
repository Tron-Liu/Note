/**
 * @file 8-3.cpp
 * @author Tron-Liu (Tron-Liu@foxmail.com)
 * @brief HTTP 请求的读取和分析
 * @version 0.1
 * @date 2022-09-09
 *
 * @copyright Copyright (c) 2022
 *
 */

// HTTP 协议并未提供头部长度字段。根据协议规定，我们判断 HTTP 头部结束的依据是
// 遇见一个空行，该空行仅包含一对回车换行符 <CR><LF>

// 该程序使用主、从两个有限状态机实现了最简单的 HTTP 请求的读取和分析
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFFER_SIZE 4096  // 读缓冲区大小

// 主状态机的两种可能状态
// 分别表示当前正在分析请求行，当在正在分析头部字段
enum CHECK_STATE { CHECK_STATE_REQUESTLINE = 0, CHECK_STATE_HEADER };

// 从状态机的三种可能状态，即行的读取状态
// 分别表示：读取到一个完整的行、行出错和行数据尚且不完整
enum LINE_STATUS { LINE_OK = 0, LINE_BAD, LINE_OPEN };

// 服务器处理 HTTP 请求的结果
enum HTTP_CODE {
  NO_REQUEST,         // 请求不完整，需要继续读取客户数据
  GET_REQUEST,        // 获得一个完整的客户请求
  BAD_REQUEST,        // 服务器内部错误
  FORBIDDEN_REQUEST,  // 客户对资源没有足够的访问权限
  INTERNAL_ERROR,     // 服务器内部错误
  CLOSED_CONNECTION   // 客户端已经关闭连接
};

// 为了简化问题，我们没有给客户端发送一个完整的 HTTP 应答报文
// 而只是根据服务器的处理结果发送如下成功或失败信息
static const char* szret[] = {"I get a corret result.\n", "Something wrong.\n"};

// 从状态机，用于解析出一行内容
LINE_STATUS parse_line(char* buffer, int& checked_index, int& read_index) {
  char temp;

  // checked_index 指向 buffer （应用程序的读缓冲区）中当前正在分析的字节
  // read_index 指向 buffer 中客户端数据的尾部的下一字节
  // buffer 中第 0～checked_index 字节都已分析完毕，
  //          第 checked_index ~ (read_index-1) 字节由下面的循环挨个分析
  for (; checked_index < read_index; checked_index++) {
    // 获得当前要分析的字节
    temp = buffer[checked_index];
    // 如果当前字节是 "\r"，即回车符，则说明可能读取到一个完整的行
    if (temp == '\r') {
      // 如果 "\r" 字符碰巧是目前 buffer 中的最后一个已经被读入的客户数据
      // 那么这次分析没有读取到一个完整的行，返回 LINE_OPEN
      // 以表示还需要继续客户数据才能进一步分析
      if ((checked_index + 1) == read_index) return LINE_OPEN;
      // 如果下一个字符是 "\n", 则说明我们成功读取到一个完整的行
      else if (buffer[checked_index + 1] == '\n') {
        buffer[checked_index++] = '\0';
        buffer[checked_index++] = '\0';
        return LINE_OK;
      }
      // 否则的话，说明客户发送的 HTTP 请求存在语法问题
      return LINE_BAD;
    }
    // 如果当前的字符是 "\n"，即换行符，则也说明可能读取到一个完整的行
    else if (temp == '\n') {
      if ((checked_index > 1) && buffer[checked_index - 1] == '\r') {
        buffer[checked_index - 1] = '\0';
        buffer[checked_index++] = '\0';
        return LINE_OK;
      }
      return LINE_BAD;
    }
  }
  // 如果所有内容都分析完毕也没遇到 "\r" 字符， 则返回 LINE_OPEN
  // 以表示还需要继续客户数据才能进一步分析
  return LINE_OPEN;
}

// 分析请求行
HTTP_CODE parse_requestline(char* temp, CHECK_STATE& checkstate) {
  char* url = strpbrk(temp, " \t");
  if (!url) return BAD_REQUEST;
  *url++ = '\0';

  char* method = temp;
  if (strcasecmp(method, "GET") == 0)  // 仅支持 GET 方法
    printf("The request method is GET.\n");
  else
    return BAD_REQUEST;

  /**
   * size_t strspn(const char* str1, const char* str2)
   * str1: 要被检索的字符串
   * str2: 包含要在 str1 中进行匹配的字符列表
   *
   * 该函数返回 str1 中第一个不在字符串 str2 中出现的字符下标
   */
  url += strspn(url, " \t");
  char* version = strpbrk(url, " \t");
  if (!version) return BAD_REQUEST;
  *version++ = '\0';
  version += strspn(version, " \t");

  if (strcasecmp(version, "HTTP/1.1") != 0) return BAD_REQUEST;

  // 检查 URL 是否合法
  if (strncasecmp(url, "http://", 7) == 0) {
    url += 7;
    url = strchr(url, '/');
  }

  if (!url || url[0] != '/') return BAD_REQUEST;
  printf("The request URL is: %s\n", url);

  // HTTP 请求行处理完毕，状态转移到头部字段的分析
  checkstate = CHECK_STATE_HEADER;
  return NO_REQUEST;
}

// 分析头部字段
HTTP_CODE parse_headers(char* temp) {
  // 遇到一个空行，说明我们得到一个正确的 HTTP 请求
  if (temp[0] == '\0') return GET_REQUEST;
  // 处理 Host 头部字段
  else if (strncasecmp(temp, "Host:", 5) == 0) {
    temp += 5;
    temp += strspn(temp, "\t");
    printf("The request host is: %s\n", temp);
  }
  // 其他头部字段都不处理
  else
    printf("I can not handle this header.\n");
  return NO_REQUEST;
}

// 分析 HTTP 请求的入口函数
HTTP_CODE parse_content(char* buffer, int& checked_index,
                        CHECK_STATE& check_state, int& read_index,
                        int& start_line) {
  // 记录当前行的读取状态
  LINE_STATUS line_status = LINE_OK;
  // 记录 HTTP 请求的处理结果
  HTTP_CODE ret_code = NO_REQUEST;
  // 主状态机，用于从 buffer 中取出所有完整的行
  while ((line_status = parse_line(buffer, checked_index, read_index)) ==
         LINE_OK) {
    // start_line 是行在 buffer 中的起始位置
    char* temp = buffer + start_line;
    // 记录下一行的起始位置
    start_line = checked_index;
    // checkstate 记录主状态机当前的状态
    switch (check_state) {
      case CHECK_STATE_REQUESTLINE: {  // 第一个状态，分析请求行
        ret_code = parse_requestline(temp, check_state);

        if (ret_code == BAD_REQUEST) return BAD_REQUEST;

        break;
      }
      case CHECK_STATE_HEADER: {  // 第二个状态，分析头部字段
        ret_code = parse_headers(temp);

        if (ret_code == BAD_REQUEST)
          return BAD_REQUEST;
        else if (ret_code == GET_REQUEST)
          return GET_REQUEST;

        break;
      }
      default: {
        return INTERNAL_ERROR;
      }
    }
  }
  // 若没有读取到一个完整的行，则表示还需要继续读取客户数据才能进一步分析
  if (line_status == LINE_OPEN)
    return NO_REQUEST;
  else
    return BAD_REQUEST;
}

int main(int argc, char* argv[]) {
  if (argc <= 2) {
    printf("usage: %s ip_address port_number backlog\n", basename(argv[0]));
    return 1;
  }

  const char* ip = argv[1];
  int port = atoi(argv[2]);

  struct sockaddr_in address;
  bzero(&address, sizeof(address));
  address.sin_family = AF_INET;
  inet_pton(AF_INET, ip, &address.sin_addr);
  address.sin_port = htons(port);

  int listenfd = socket(PF_INET, SOCK_STREAM, 0);
  assert(listenfd >= 0);

  int ret = bind(listenfd, (struct sockaddr*)&address, sizeof(address));
  assert(ret != -1);

  ret = listen(listenfd, 5);
  assert(ret != -1);

  struct sockaddr_in client;
  socklen_t client_addrlength = sizeof(client);
  int connfd = accept(listenfd, (struct sockaddr*)&client, &client_addrlength);
  if (connfd < 0)
    printf("errno is %d.\n", errno);
  else {
    char buffer[BUFFER_SIZE];  // 读缓冲区
    memset(buffer, '\0', BUFFER_SIZE);

    int data_read = 0;
    int read_index = 0;     // 当前已经读取了多少字节的客户数据
    int checked_index = 0;  // 当前已经分析完了多少字节的客户数据
    int start_line = 0;     // 行在 buffer 中的起始位置

    // 设置主状态机的初始状态
    CHECK_STATE check_state = CHECK_STATE_REQUESTLINE;

    while (1) {  // 循环读取客户数据并分析
      data_read =
          recv(connfd, buffer + read_index, BUFFER_SIZE - read_index, 0);

      if (data_read == -1) {
        printf("reading failed.\n");
        break;
      } else if (data_read == 0) {
        printf("remote client has closed the connection.\n");
        break;
      }
      read_index += data_read;

      //分析目前已经获得的所有客户数据
      HTTP_CODE result = parse_content(buffer, checked_index, check_state,
                                       read_index, start_line);
      if (result == NO_REQUEST)
        continue;
      else if (result == GET_REQUEST) {
        send(connfd, szret[0], strlen(szret[0]), 0);
        break;
      } else {
        send(connfd, szret[1], strlen(szret[1]), 0);
        break;
      }
    }
    close(connfd);
  }

  close(listenfd);
  return 0;
}