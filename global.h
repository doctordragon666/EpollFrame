#ifndef _GLOBAL_H_
#define _GLOBAL_H_

//包含的头文件
#include <sys/time.h>		//打印事件戳
#include <sys/types.h>
#include <sys/resource.h>	
#include <sys/socket.h>		//socket服务
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <string.h>

//调试开关
static int Debug = 1;

//通用的数组长度
#define BUFLEN 1024

//最大事件数量
#define MAX_EVENTS 256 


//comm调试信息
#define COMM_OK		  (0)
#define COMM_ERROR	 (-1)
#define COMM_NOMESSAGE	 (-3)
#define COMM_TIMEOUT	 (-4)
#define COMM_SHUTDOWN	 (-5)
#define COMM_INPROGRESS  (-6)
#define COMM_ERR_CONNECT (-7)
#define COMM_ERR_DNS     (-8)
#define COMM_ERR_CLOSING (-9)
#define INFO (4)

//调试函数
#define DEBUG(level)    if(Debug && level > INFO) printf

//安全释放
#define safe_free(x)	if (x) { delete (x); x = nullptr; }


/*系统时间相关,设置成全局变量，供所有模块使用*/
static struct timeval current_time;//当前时间，单位秒
static double current_dtime;//当前的时间，分
static time_t sys_curtime;//系统时间

typedef void PF(int, void*);//回调函数，参数类型int ,void，返回值空

typedef struct _fde {
	unsigned int type;
	u_short local_port;//本地端口
	u_short remote_port;//远程（客户端）端口
	struct in_addr local_addr;//本地地址

	char ipaddr[16];		/* dotted decimal address of peer */


	PF* read_handler;//读函数
	void* read_data;
	PF* write_handler;
	void* write_data;
	PF* timeout_handler;
	time_t timeout;//超时具体的时间
	void* timeout_data;//超时数据
}fde;//fde句柄结构体

/// <summary>
/// 获取当前时间
/// </summary>
/// <param name=""></param>
/// <returns>当前时间的结构体</returns>
inline time_t getCurrentTime(void)
{
	gettimeofday(&current_time, NULL);
	current_dtime = (double)current_time.tv_sec +
		(double)current_time.tv_usec / 1000000.0;
	DEBUG(3) ("has get current time %ld\n", sys_curtime);
	return sys_curtime = current_time.tv_sec;
}

/// <summary>
/// 是否为忽略的错误
/// </summary>
/// <param name="ierrno">错误号</param>
/// <returns>是或者否</returns>
inline int ignoreErrno(int ierrno)
{
	switch (ierrno) {
	case EINPROGRESS:
	case EWOULDBLOCK:
#if EAGAIN != EWOULDBLOCK
	case EAGAIN:
#endif
	case EALREADY:
	case EINTR:
#ifdef ERESTART
	case ERESTART:
#endif
		return 1;
	default:
		return 0;
	}
}

/// <summary>
/// 获取当前错误描述并打印
/// </summary>
/// <param name=""></param>
/// <returns>标准错误格式</returns>
inline const char* xstrerror(void)
{
	static char xstrerror_buf[BUFSIZ];
	const char* errmsg;

	errmsg = strerror(errno);

	if (!errmsg || !*errmsg)
		errmsg = "Unknown error";

	snprintf(xstrerror_buf, BUFSIZ, "(%d) %s", errno, errmsg);
	return xstrerror_buf;
}

#endif // !_GLOBAL_H_