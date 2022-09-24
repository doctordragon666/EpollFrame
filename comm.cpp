#include "comm.h"

Comm::Comm(int max_fd)
{
    if (max_fd > 0) Biggest_FD = max_fd;
    
    printf("comm init!!!\n");
}

Comm::~Comm()
{
    Biggest_FD = 1024;
    safe_free(fd_table);
}

void Comm::comm_init()
{
    this->fd_table = (fde*)calloc(Biggest_FD, sizeof(fde));

    if (!fd_table)
        fprintf(stderr, "calloc fd table fail.");

    if (Debug) fprintf(stdout, "%p has create %d connection\n", this->fd_table, Biggest_FD);
    this->comm_epoll = new CommEpoll(this->fd_table);

    comm_epoll->do_epoll_init(Biggest_FD);
}

time_t Comm::commSetTimeout(int fd, int timeout, PF* handler, void* data)
{
    fde* F;
    DEBUG(5) ("commSetTimeout: FD %d timeout %d\n", fd, timeout);
    assert(fd >= 0);
    assert(fd < Biggest_FD);
    F = &fd_table[fd];


    if (timeout < 0) {
        F->timeout_handler = NULL;
        F->timeout_data = NULL;
        return F->timeout = (time_t)0;
    }
    assert(handler || F->timeout_handler);
    if (handler || data) {
        F->timeout_handler = handler;
        F->timeout_data = data;
    }
    return F->timeout = sys_curtime + (time_t)timeout;
}

/// <summary>
/// �Ͽ����ӣ������������
/// </summary>
/// <param name=""></param>
void Comm::comm_close(int fd)
{
    assert(fd > 0);
    fde* F = &fd_table[fd];
    if (F) memset((void*)F, '\0', sizeof(fde));
    comm_epoll->epollSetEvents(fd, 0, 0);//������socket�ϵ��¼�
    close(fd);
}

/// <summary>
/// ��鳬ʱ�ľ�������ó�ʱ����
/// </summary>
/// <param name=""></param>
void Comm::checkTimeouts(void)
{
    int fd;
    fde* F = NULL;
    PF* callback;
    DEBUG(3)("checktimeouts\n");

    for (fd = 0; fd <= Biggest_FD; fd++) {
        F = &fd_table[fd];

        if (F->timeout == 0)
            continue;
        if (F->timeout > sys_curtime)
            continue;
        DEBUG(5) ("checkTimeouts: FD %d Expired\n", fd);

        if (F->timeout_handler) {
            DEBUG(5) ("checkTimeouts: FD %d: Call timeout handler\n", fd);
            callback = F->timeout_handler;
            F->timeout_handler = NULL;
            callback(fd, F->timeout_data);
        }
        else {
            DEBUG(5) ("checkTimeouts: FD %d: Forcing comm_close()\n", fd);
            comm_close(fd);
        }
    }
}

/// <summary>
/// ��epoll�л�ȡ�¼�
/// </summary>
/// <param name="msec">��ʱֵ</param>
/// <returns>������Ϣ</returns>
int Comm::comm_select(int msec)
{
    static double last_timeout = 0.0;//��̬��������һ������ʱ��
    int rc;

    DEBUG(3)("comm_select: timeout %d\n", msec);

    if (msec > MAX_POLL_TIME)
        msec = MAX_POLL_TIME;


    //statCounter.select_loops++;
    /* ÿһ���Ӽ��һ�³�ʱ */
    if (last_timeout + 0.999 < current_dtime) {
        last_timeout = current_dtime;
        checkTimeouts();
    }
    else {
        double max_timeout = (last_timeout + 1.0 - current_dtime) * 1000;
        if (max_timeout < msec)
            msec = (int)max_timeout;
    }

    rc = comm_epoll->do_epoll_select(msec);

    getCurrentTime();

    if (rc == COMM_TIMEOUT)
        DEBUG(5) ("comm_select: time out\n");

    return rc;

}

void Comm::commUpdateWriteHandler(int fd, PF* handler, void* data)
{
    fd_table[fd].read_handler = handler;
    fd_table[fd].read_data = data;

    for (int i = 0; i <= fd; i++)
    {
        printf("%p", fd_table[i].read_handler);
    }

    this->comm_epoll->epollSetEvents(fd, 0, 1);
    if (Debug) fprintf(stdout, "%d has update read event,has read handler%p,has read_data%p\n", fd, fd_table[fd].read_handler, fd_table[fd].read_data);
}

/// <summary>
/// ����fd�����е�д�¼�+1���Լ����ݲ����ͻص�����
/// </summary>
/// <param name="fd">������socket������</param>
/// <param name="handler">д�¼��Ļص�����</param>
/// <param name="data">����</param>
void Comm::commUpdateReadHandler(int fd, PF* handler, void* data)
{
    if (fd_table == nullptr)
        return;
    fd_table[fd].write_handler = handler;
    fd_table[fd].write_data = data;
    DEBUG(10) ("has set %d event\n", fd);

    comm_epoll->epollSetEvents(fd, 1, 0);
}

void Comm::comm_select_shutdown(void)
{
    comm_epoll->do_epoll_shutdown();
    if (fd_table) free(fd_table);
}