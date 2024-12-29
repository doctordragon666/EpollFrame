#ifndef _GLOBAL_H_
#define _GLOBAL_H_

// 包含的头文件
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <string.h>

//调试开关
static int Debug = 1;

//调试信息
#define EPOLL_OK		  (0)
#define EPOLL_ERROR	 (-1)

//调试函数
#define DEBUG if(Debug) printf
#define ERROR perror

typedef void epollCb(int, void*);//回调函数，参数类型int ,void，返回值空

enum MyepollEventType
{
    EPOLL_READ_EVENT = 1,
    EPOLL_WRITE_EVENT = 2,
    EPOLL_TIMEOUT_EVENT = 4,
    EPOLL_ERROR_EVENT = 8,
    EPOLL_ALL = 15,
    EPOLL_MAIN_EVENT = 16
};

/// @brief 为什么设计成两个指针，因为epoll不能在一个文件句柄上设置读事件和写事件，所以需要两个指针
struct MyepollEvent
{
    MyepollEventType type;
    int fd;
    epollCb* write_cb;
    epollCb* read_cb;
    void* read_data;
    void* write_data;
};

#endif // !_GLOBAL_H_