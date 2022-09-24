#include "Server.h"

/// <summary>
/// �÷�
/// </summary>
/// <param name="argv">������</param>
void usage(const char* argv)
{
	printf("%s:[ip][port]\n", argv);
}

int main(int argc, char** argv)
{
	if (argc != 3) //�����������Ƿ���ȷ
	{
		usage(argv[0]);
		exit(1);
	}

	//��ʼ������
	Server* server = new Server(argv[1], atoi(argv[2]));
	int listen_sock = server->startup(); //����һ�����˱��� ip �Ͷ˿ںŵ��׽���������

	//��ʼ������״̬
	ConnectStat* stat = new ConnectStat(listen_sock);

	//��ʼ���첽�¼�������epoll
	Comm* comm = new Comm(102400);
	comm->comm_init();
	comm->commUpdateReadHandler(listen_sock, Server::accept_connection, (void*)stat);

	server->set_comm(comm);

	int ret = -1;
	do
	{
		//����ѭ�������¼�
		ret = comm->comm_select(2000);
	} while (ret <= 0);

	comm->comm_select_shutdown();

	safe_free(server);
	safe_free(comm);
	safe_free(stat);
}	
