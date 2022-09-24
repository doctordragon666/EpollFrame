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
	/// ��ʼ���������飬Ȼ���ʼ��epoll����
	/// </summary>
	void comm_init();

	/// <summary>
	/// ���ó�ʱ�������Լ���ʱʱ��
	/// </summary>
	/// <param name="fd">Ҫ���õ�socket������</param>
	/// <param name="timeout">��ʱʱ��</param>
	/// <param name="handler">��ʱ������</param>
	/// <param name="data">����</param>
	/// <returns>��ʱ�ľ���ʱ��</returns>
	time_t commSetTimeout(int fd, int timeout, PF* handler, void* data);

	/// <summary>
	/// �ر�����
	/// </summary>
	/// <param name="fd">socket�ļ�������</param>
	void comm_close(int fd);
	void checkTimeouts(void);
	int comm_select(int msec);
	void comm_select_shutdown(void);
	void commUpdateWriteHandler(int fd, PF* handler, void* data);
	fde* fd_table = nullptr;//����������ⲿ�����޸����ֵ���������Ž�ģʽ������

	/// <summary>
	/// ����fd�����еĶ��¼�+1���Լ����ݲ����ͻص�����
	/// </summary>
	/// <param name="fd">������socket������</param>
	/// <param name="handler">���¼��Ļص�����</param>
	/// <param name="data">����</param>
	void commUpdateReadHandler(int fd, PF* handler, void* data);

private:
	int Biggest_FD = 1024;  /* Ĭ�ϵ�����ļ����������� 1024 */
	int MAX_POLL_TIME = 1000;	// epoll���Ĵ��� 
	CommEpoll* comm_epoll;
};

#endif // !_COMM_H