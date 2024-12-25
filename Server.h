#ifndef _SERVER_H_
#define _SERVER_H_

#include "global.h"
#include "comm.h"

struct ConnectStat
{
	int fd;
	char send_buf[BUFLEN];
	PF* handler; //不同页面的处理函数

	/// <summary>
	/// 初始化连接
	/// </summary>
	/// <param name="fd">连接句柄</param>
	/// <returns></returns>
	ConnectStat(int fd)
	{
		this->fd = fd;
	}
	/// <summary>
	/// 设置非阻塞
	/// </summary>
	/// <param name="fd">服务器句柄</param>
	void set_nonblock()
	{
		int fl = fcntl(this->fd, F_GETFL);
		fcntl(this->fd, F_SETFL, fl | O_NONBLOCK);
	}
};

/// <summary>
/// 流程概述
/// 1、先等待回调accept_connection
/// 2、执行do_welcome_handler
/// 3、执行do_echo_handler
/// 4、执行do_echo_response
/// 循环执行3，4
/// </summary>
class Server
{
public:
	Server(char* _ip, int _port);
	~Server();

	/// <summary>
	/// 设置通用接口连接
	/// </summary>
	/// <param name="comm"></param>
	static void set_comm(Comm*& comm);

	/// <summary>
	/// 接受连接
	/// </summary>
	/// <param name="fd"></param>
	/// <param name="data"></param>
	static void accept_connection(int fd, void* data);

	/// <summary>
	/// 创建一个套接字，绑定，检测服务器
	/// </summary>
	/// <param name="_ip">ip地址</param>
	/// <param name="_port">端口</param>
	/// <returns>返回这样的套接字</returns>
	int startup();

	/// <summary>
	/// 回声处理
	/// </summary>
	/// <param name="fd"></param>
	/// <param name="data"></param>
	static void do_echo_handler(int fd, void* data);

	/// <summary>
	/// 发送欢迎消息后进入echo服务器状态
	/// </summary>
	/// <param name="fd"></param>
	/// <param name="data"></param>
	static void do_welcome_handler(int fd, void* data);

	/// <summary>
	/// 响应请求
	/// </summary>
	/// <param name="fd">服务器句柄</param>
	/// <param name="data">数据</param>
	static void do_echo_response(int fd, void* data);

	/// <summary>
	/// 超时处理
	/// </summary>
	/// <param name="fd">要关闭的连接</param>
	/// <param name="data">在这里指连接结构体</param>
	static void do_echo_timeout(int fd, void* data);
private:
	char* m_ip;//服务器IP
	int m_port;//服务器端口
	static Comm* m_comm;//通用连接类
	static int m_timeout;//默认超时处理
};

#endif // !_SERVER_H_