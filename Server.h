#ifndef _SERVER_H_
#define _SERVER_H_

#include "global.h"
#include "comm.h"

struct ConnectStat
{
	int fd;
	char send_buf[BUFLEN];
	PF* handler; //��ͬҳ��Ĵ�����

	/// <summary>
	/// ��ʼ������
	/// </summary>
	/// <param name="fd">���Ӿ��</param>
	/// <returns></returns>
	ConnectStat(int fd)
	{
		this->fd = fd;
	}
	/// <summary>
	/// ���÷�����
	/// </summary>
	/// <param name="fd">���������</param>
	void set_nonblock()
	{
		int fl = fcntl(this->fd, F_GETFL);
		fcntl(this->fd, F_SETFL, fl | O_NONBLOCK);
	}
};

class Server
{
public:
	Server(char* _ip, int _port);
	~Server();
	static void set_comm(Comm*& comm);
	// echo ����ʵ����ش���
	static void accept_connection(int fd, void* data);

	/// <summary>
	/// ����һ���׽��֣��󶨣���������
	/// </summary>
	/// <param name="_ip">ip��ַ</param>
	/// <param name="_port">�˿�</param>
	/// <returns>�����������׽���</returns>
	int startup();

	static void do_echo_handler(int fd, void* data);

	static void do_welcome_handler(int fd, void* data);

	static void do_echo_response(int fd, void* data);

	/// <summary>
	/// ��ʱ����
	/// </summary>
	/// <param name="fd">Ҫ�رյ�����</param>
	/// <param name="data">������ָ���ӽṹ��</param>
	static void do_echo_timeout(int fd, void* data);
private:
	char* m_ip;
	int m_port;
	static Comm* m_comm;
};



#endif // !_SERVER_H_