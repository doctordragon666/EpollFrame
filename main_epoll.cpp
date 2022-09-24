#include "Server.h"

/// <summary>
/// 用法
/// </summary>
/// <param name="argv">程序名</param>
void usage(const char* argv)
{
	printf("%s:[ip][port]\n", argv);
}

int main(int argc, char** argv)
{
	if (argc != 3) //检测参数个数是否正确
	{
		usage(argv[0]);
		exit(1);
	}

	//初始化连接
	Server* server = new Server(argv[1], atoi(argv[2]));
	int listen_sock = server->startup(); //创建一个绑定了本地 ip 和端口号的套接字描述符

	//初始化连接状态
	ConnectStat* stat = new ConnectStat(listen_sock);

	//初始化异步事件处理框架epoll
	Comm* comm = new Comm(102400);
	comm->comm_init();
	comm->commUpdateReadHandler(listen_sock, Server::accept_connection, (void*)stat);

	server->set_comm(comm);

	int ret = -1;
	do
	{
		//不断循环处理事件
		ret = comm->comm_select(2000);
	} while (ret <= 0);

	comm->comm_select_shutdown();

	safe_free(server);
	safe_free(comm);
	safe_free(stat);
}	
