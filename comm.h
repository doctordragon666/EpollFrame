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
	/// 初始化连接数组，然后初始化epoll连接，最后初始化Comm_epoll类
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

	/// <summary>
	/// 检查超时的句柄，调用超时处理
	/// </summary>
	/// <param name=""></param>
	void checkTimeouts(void);

	/// <summary>
	/// 从epoll中获取事件
	/// </summary>
	/// <param name="msec">超时值</param>
	/// <returns>调试信息</returns>
	int comm_select(int msec);

	/// <summary>
	/// 关闭连接，并且释放fd_table表
	/// </summary>
	/// <param name=""></param>
	void comm_select_shutdown(void);

	/// <summary>
	/// 更新写事件
	/// </summary>
	/// <param name="fd">设置的句柄</param>
	/// <param name="handler">回调函数</param>
	/// <param name="data">连接状态void*</param>
	void commUpdateWriteHandler(int fd, PF* handler, void* data);
	
	/// <summary>
	/// 更新fd数组中的读事件+1，以及传递参数和回调函数
	/// </summary>
	/// <param name="fd">操作的socket描述符</param>
	/// <param name="handler">读事件的回调函数</param>
	/// <param name="data">数据</param>
	void commUpdateReadHandler(int fd, PF* handler, void* data);

private:
	int Biggest_FD = 1024;  /* 默认的最大文件描述符数量 1024 */
	int MAX_POLL_TIME = 1000;	// epoll最大获取事件的间隔
	CommEpoll* comm_epoll;//调用epoll的接口

public:
	fde* fd_table = nullptr;//句柄表，允许外部变量修改这个值，不适用桥接模式来更新
};

#endif // !_COMM_H