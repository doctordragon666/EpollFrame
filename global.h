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
// #define COMM_NOMESSAGE	 (-3)
// #define COMM_TIMEOUT	 (-4)
// #define COMM_SHUTDOWN	 (-5)
// #define COMM_INPROGRESS  (-6)
// #define COMM_ERR_CONNECT (-7)
// #define COMM_ERR_DNS     (-8)
// #define COMM_ERR_CLOSING (-9)
// #define INFO (4)

//调试函数
#define DEBUG if(Debug) printf
#define ERROR perror

// //安全释放
// #define safe_free(x)	if (x) { delete (x); x = nullptr; }


// /*系统时间相关,设置成全局变量，供所有模块使用*/
// static struct timeval current_time;//当前时间，单位秒
// static double current_dtime;//当前的时间，分
// static time_t sys_curtime;//系统时间

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


// typedef struct _fde {
// 	unsigned int type;
// 	u_short local_port;//本地端口
// 	u_short remote_port;//远程（客户端）端口
// 	struct in_addr local_addr;//本地地址

// 	char ipaddr[16];		/* dotted decimal address of peer */


// 	PF* read_handler;//读函数
// 	void* read_data;
// 	PF* write_handler;
// 	void* write_data;
// 	PF* timeout_handler;
// 	time_t timeout;//超时具体的时间
// 	void* timeout_data;//超时数据
// }fde;//fde句柄结构体

// inline time_t getCurrentTime(void)
// {
// 	gettimeofday(&current_time, NULL);
// 	current_dtime = (double)current_time.tv_sec +
// 		(double)current_time.tv_usec / 1000000.0;
// 	DEBUG(3) ("has get current time %ld\n", sys_curtime);
// 	return sys_curtime = current_time.tv_sec;
// }


// inline int ignoreErrno(int ierrno)
// {
// 	switch (ierrno) {
// 	case EINPROGRESS:
// 	case EWOULDBLOCK:
// #if EAGAIN != EWOULDBLOCK
// 	case EAGAIN:
// #endif
// 	case EALREADY:
// 	case EINTR:
// #ifdef ERESTART
// 	case ERESTART:
// #endif
// 		return 1;
// 	default:
// 		return 0;
// 	}
// }


// inline const char* xstrerror(void)
// {
// 	static char xstrerror_buf[BUFSIZ];
// 	const char* errmsg;

// 	errmsg = strerror(errno);

// 	if (!errmsg || !*errmsg)
// 		errmsg = "Unknown error";

// 	snprintf(xstrerror_buf, BUFSIZ, "(%d) %s", errno, errmsg);
// 	return xstrerror_buf;
// }

#endif // !_GLOBAL_H_