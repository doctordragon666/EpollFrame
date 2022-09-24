#ifndef _COMM_H
#define _COMM_H
#include "global.h"
#include "comm_epoll.h"

class Comm
{
public:
	Comm(int max_fd);
	~Comm();

	/// <summary>
	/// 初始化连接数组，然后初始化epoll连接
	/// </summary>
	void comm_init();

	/// <summary>
	/// 设置超时函数，以及超时时间
	/// </summary>
	/// <param name="fd">要设置的socket描述符</param>
	/// <param name="timeout">超时时间</param>
	/// <param name="handler">超时处理函数</param>
	/// <param name="data">数据</param>
	/// <returns>超时的具体时间</returns>
	time_t commSetTimeout(int fd, int timeout, PF* handler, void* data);

	/// <summary>
	/// 关闭连接
	/// </summary>
	/// <param name="fd">socket文件描述符</param>
	void comm_close(int fd);
	void checkTimeouts(void);
	int comm_select(int msec);
	void comm_select_shutdown(void);
	void commUpdateWriteHandler(int fd, PF* handler, void* data);
	fde* fd_table = nullptr;//句柄表，允许外部变量修改这个值，不适用桥接模式来更新

	/// <summary>
	/// 更新fd数组中的读事件+1，以及传递参数和回调函数
	/// </summary>
	/// <param name="fd">操作的socket描述符</param>
	/// <param name="handler">读事件的回调函数</param>
	/// <param name="data">数据</param>
	void commUpdateReadHandler(int fd, PF* handler, void* data);

private:
	int Biggest_FD = 1024;  /* 默认的最大文件描述符数量 1024 */
	int MAX_POLL_TIME = 1000;	// epoll最多的次数 
	CommEpoll* comm_epoll;
};

#endif // !_COMM_H