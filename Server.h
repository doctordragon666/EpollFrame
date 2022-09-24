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

/// <summary>
/// ���̸���
/// 1���ȵȴ��ص�accept_connection
/// 2��ִ��do_welcome_handler
/// 3��ִ��do_echo_handler
/// 4��ִ��do_echo_response
/// ѭ��ִ��3��4
/// </summary>
class Server
{
public:
	Server(char* _ip, int _port);
	~Server();

	/// <summary>
	/// ����ͨ�ýӿ�����
	/// </summary>
	/// <param name="comm"></param>
	static void set_comm(Comm*& comm);

	/// <summary>
	/// ��������
	/// </summary>
	/// <param name="fd"></param>
	/// <param name="data"></param>
	static void accept_connection(int fd, void* data);

	/// <summary>
	/// ����һ���׽��֣��󶨣���������
	/// </summary>
	/// <param name="_ip">ip��ַ</param>
	/// <param name="_port">�˿�</param>
	/// <returns>�����������׽���</returns>
	int startup();

	/// <summary>
	/// ��������
	/// </summary>
	/// <param name="fd"></param>
	/// <param name="data"></param>
	static void do_echo_handler(int fd, void* data);

	/// <summary>
	/// ���ͻ�ӭ��Ϣ�����echo������״̬
	/// </summary>
	/// <param name="fd"></param>
	/// <param name="data"></param>
	static void do_welcome_handler(int fd, void* data);

	/// <summary>
	/// ��Ӧ����
	/// </summary>
	/// <param name="fd">���������</param>
	/// <param name="data">����</param>
	static void do_echo_response(int fd, void* data);

	/// <summary>
	/// ��ʱ����
	/// </summary>
	/// <param name="fd">Ҫ�رյ�����</param>
	/// <param name="data">������ָ���ӽṹ��</param>
	static void do_echo_timeout(int fd, void* data);
private:
	char* m_ip;//������IP
	int m_port;//�������˿�
	static Comm* m_comm;//ͨ��������
	static int m_timeout;//Ĭ�ϳ�ʱ����
};

#endif // !_SERVER_H_