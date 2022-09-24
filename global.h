#pragma once

//������ͷ�ļ�
#include <sys/time.h>
#include <sys/resource.h>	
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <assert.h>
#include <string.h>
#include <sys/epoll.h>

#define FD_DESC_SZ		64
static int Debug = 1;

//comm������Ϣ
#define COMM_OK		  (0)
#define COMM_ERROR	 (-1)
#define COMM_NOMESSAGE	 (-3)
#define COMM_TIMEOUT	 (-4)
#define COMM_SHUTDOWN	 (-5)
#define COMM_INPROGRESS  (-6)
#define COMM_ERR_CONNECT (-7)
#define COMM_ERR_DNS     (-8)
#define COMM_ERR_CLOSING (-9)
#define INFO (4)


//�������
#define DEBUG_LEVEL  0
#define DEBUG_ONLY   8
#define DEBUG(level)    if(Debug && level > INFO) printf

#define safe_free(x)	if (x) { delete (x); x = nullptr; }


/*ϵͳʱ�����,���ó�ȫ�ֱ�����������ģ��ʹ��*/
static struct timeval current_time;//��ǰʱ�䣬��λ��
static double current_dtime;//��ǰ��ʱ�䣬��
static time_t sys_curtime;//ϵͳʱ��

/// <summary>
/// ��ȡ��ǰʱ��
/// </summary>
/// <param name=""></param>
/// <returns>��ǰʱ��Ľṹ��</returns>
inline time_t getCurrentTime(void)
{
    gettimeofday(&current_time, NULL);
    current_dtime = (double)current_time.tv_sec +
        (double)current_time.tv_usec / 1000000.0;
    DEBUG(3) ("has get current time %ld\n",sys_curtime);
    return sys_curtime = current_time.tv_sec;
}

typedef void PF(int, void*);//�ص���������������int ,void������ֵ��

typedef struct _fde {
    unsigned int type;
    u_short local_port;//���ض˿�
    u_short remote_port;//Զ�̣��ͻ��ˣ��˿�
    struct in_addr local_addr;//���ص�ַ

    char ipaddr[16];		/* dotted decimal address of peer */


    PF* read_handler;//������
    void* read_data;
    PF* write_handler;
    void* write_data;
    PF* timeout_handler;
    time_t timeout;//��ʱʱ��
    void* timeout_data;//��ʱ����
}fde;

#define BUFLEN 1024
#define MAX_EVENTS 256 


/// <summary>
/// �Ƿ�Ϊ���ԵĴ���
/// </summary>
/// <param name="ierrno">�����</param>
/// <returns>�ǻ��߷�</returns>
inline int ignoreErrno(int ierrno)
{
    switch (ierrno) {
    case EINPROGRESS:
    case EWOULDBLOCK:
#if EAGAIN != EWOULDBLOCK
    case EAGAIN:
#endif
    case EALREADY:
    case EINTR:
#ifdef ERESTART
    case ERESTART:
#endif
        return 1;
    default:
        return 0;
    }
}

/// <summary>
/// ��ȡ��ǰ������������ӡ
/// </summary>
/// <param name=""></param>
/// <returns>��׼�����ʽ</returns>
inline const char* xstrerror(void)
{
    static char xstrerror_buf[BUFSIZ];
    const char* errmsg;

    errmsg = strerror(errno);

    if (!errmsg || !*errmsg)
        errmsg = "Unknown error";

    snprintf(xstrerror_buf, BUFSIZ, "(%d) %s", errno, errmsg);
    return xstrerror_buf;
}