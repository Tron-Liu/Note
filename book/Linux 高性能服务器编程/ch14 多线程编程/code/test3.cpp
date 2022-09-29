/**
 * @file test3.cpp
 * @author Tron-Liu (Tron-Liu@foxmail.com)
 * @brief 在多线程程序中调用 fork 函数
 * @version 0.1
 * @date 2022-09-28
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <wait.h>

pthread_mutex_t mutex;

void* another(void* arg)
{
    printf("in child thread, lock the mutex.\n");
    pthread_mutex_lock(&mutex);
}