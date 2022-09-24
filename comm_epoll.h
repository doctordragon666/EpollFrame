#ifndef _COMM_EPOLL_H_
#define _COMM_EPOLL_H_

#include <sys/epoll.h>
#include "global.h"

/// <summary>
/// epoll封装接口
/// </summary>
class CommEpoll
{
public:
	CommEpoll(fde*& fd_table);
	~CommEpoll();

	/// <summary>
	/// 返回epoll操作的字符串格式
	/// </summary>
	/// <param name="x">EPOLL_CTL_ADD的操作符号</param>
	/// <returns>EPOLL_CTL_ADD字符串</returns>
	static const char* epolltype_atoi(int x);

	/// <summary>
	/// 调用fd处理函数
	/// </summary>
	/// <param name="fd">对应的socket描述符</param>
	/// <param name="read_event">读标志，需要执行读事件时置为1</param>
	/// <param name="write_event">写标志，需要执行写事件时置为1</param>
	void comm_call_handlers(int fd, int read_event, int write_event);

	/// <summary>
	/// epoll初始化，并且初始化一个epoll状态
	/// </summary>
	/// <param name="max_fd">最大句柄数</param>
	void do_epoll_init(int max_fd);

	/// <summary>
	/// 关闭epoll连接，释放连接状态
	/// </summary>
	void do_epoll_shutdown();

	/// <summary>
	/// 等待epoll事件出现并且处理
	/// </summary>
	/// <param name="msec">检查的时间间隔</param>
	/// <returns>COMM类型的调试信息</returns>
	int do_epoll_select(int msec);

	/// <summary>
	/// epoll设置事件
	/// </summary>
	/// <param name="fd">文件描述符</param>
	/// <param name="need_read">是否需要读取</param>
	/// <param name="need_write">是否需要写入</param>
	void epollSetEvents(int fd, int need_read, int need_write);
private:
	fde* m_fde_table;//comm的fde表，减少耦合不使用comm对象
	struct epoll_event events[MAX_EVENTS];//存放epoll事件的数组
	int epoll_instance;//epoll实例句柄
	int epoll_fds;//epoll事件的总和
	unsigned int* epoll_state; // 保存每个epoll 的事件状态 
};

#endif // !_COMM_EPOLL_H_