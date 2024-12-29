// /*************************************************
// Date: 2024年12月1日14点09分
// Description:测试自己的API
// *************************************************/

// #include "Server.h"
// #include <memory>

// int main(int argc, char** argv)
// {
// 	if (argc != 3) //检测参数个数是否正确
// 	{
// 		printf("%s [ip] [port]\n", argv[0]);
// 		exit(1);
// 	}

// 	//初始化连接
// 	Server* server = new Server(argv[1], atoi(argv[2]));
// 	int listen_sock = server->startup(); //创建一个绑定了本地 ip 和端口号的套接字描述符

// 	//初始化连接状态
// 	ConnectStat* stat = new ConnectStat(listen_sock);

// 	//初始化异步事件处理框架epoll
// 	Network* comm = new Network(1024);
// 	comm->comm_init();
// 	comm->commUpdateReadHandler(listen_sock, Server::accept_connection, (void*)stat);

// 	//设置server的连接状态
// 	server->set_comm(comm);

// 	int ret = -1;
// 	do
// 	{
// 		//不断循环处理事件
// 		ret = comm->comm_select(2000);
// 	} while (ret <= 0);

// 	comm->comm_select_shutdown();

// 	safe_free(comm);
// 	safe_free(stat);
// }	
