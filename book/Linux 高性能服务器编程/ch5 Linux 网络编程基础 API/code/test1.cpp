/**
 * @file: 5-1.c
 * @author: Tron-Liu (Tron-Liu@foxmail.com)
 * @brief:
 * @date: 2022-09-15
 *
 * @copyright Copyright (c) 2022
 *
 **/

#include <stdio.h>

void byte_order() {
  union {
    short value;
    char union_bytes[sizeof(short)];
  } test;

  test.value = 0x0102;
  if (test.union_bytes[0] == 1 && test.union_bytes[1] == 2)
    printf("big endian\n");
  else if (test.union_bytes[0] == 2 && test.union_bytes[1] == 1)
    printf("little endian\n");
  else
    printf("unkown...\n");
}

int main() {
  byte_order();

  return 0;
}
