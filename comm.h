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
	/// ��ʼ���������飬Ȼ���ʼ��epoll���ӣ�����ʼ��Comm_epoll��
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

	/// <summary>
	/// ��鳬ʱ�ľ�������ó�ʱ����
	/// </summary>
	/// <param name=""></param>
	void checkTimeouts(void);

	/// <summary>
	/// ��epoll�л�ȡ�¼�
	/// </summary>
	/// <param name="msec">��ʱֵ</param>
	/// <returns>������Ϣ</returns>
	int comm_select(int msec);

	/// <summary>
	/// �ر����ӣ������ͷ�fd_table��
	/// </summary>
	/// <param name=""></param>
	void comm_select_shutdown(void);

	/// <summary>
	/// ����д�¼�
	/// </summary>
	/// <param name="fd">���õľ��</param>
	/// <param name="handler">�ص�����</param>
	/// <param name="data">����״̬void*</param>
	void commUpdateWriteHandler(int fd, PF* handler, void* data);
	
	/// <summary>
	/// ����fd�����еĶ��¼�+1���Լ����ݲ����ͻص�����
	/// </summary>
	/// <param name="fd">������socket������</param>
	/// <param name="handler">���¼��Ļص�����</param>
	/// <param name="data">����</param>
	void commUpdateReadHandler(int fd, PF* handler, void* data);

private:
	int Biggest_FD = 1024;  /* Ĭ�ϵ�����ļ����������� 1024 */
	int MAX_POLL_TIME = 1000;	// epoll����ȡ�¼��ļ��
	CommEpoll* comm_epoll;//����epoll�Ľӿ�

public:
	fde* fd_table = nullptr;//����������ⲿ�����޸����ֵ���������Ž�ģʽ������
};

#endif // !_COMM_H