#include "comm.h"

Comm::Comm(int max_fd)
{
    if (max_fd > 0) Biggest_FD = max_fd;
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
    }//如果设置的超时小于零返回空fde
    assert(handler || F->timeout_handler);
    if (handler || data) {
        F->timeout_handler = handler;
        F->timeout_data = data;
    }
    return F->timeout = sys_curtime + (time_t)timeout;
}

void Comm::comm_close(int fd)
{
    assert(fd > 0);
    fde* F = &fd_table[fd];
    if (F) memset((void*)F, '\0', sizeof(fde));
    comm_epoll->epollSetEvents(fd, 0, 0);//清空这个socket上的事件
    close(fd);
}

void Comm::checkTimeouts(void)
{
    int fd;
    fde* F = nullptr;
    PF* callback;
    DEBUG(3)("checktimeouts\n");

    for (fd = 0; fd <= Biggest_FD; fd++) {
        F = &fd_table[fd];

        if (F->timeout == 0)
            continue;
        if (F->timeout > sys_curtime)
            continue;//找下一个超时的
        DEBUG(5) ("checkTimeouts: FD %d Expired\n", fd);

        if (F->timeout_handler) {
            DEBUG(5) ("checkTimeouts: FD %d: Call timeout handler\n", fd);
            callback = F->timeout_handler;
            F->timeout_handler = nullptr;
            callback(fd, F->timeout_data);
        }
        else {
            DEBUG(5) ("checkTimeouts: FD %d: Forcing comm_close()\n", fd);
            comm_close(fd);
        }
    }
}

int Comm::comm_select(int msec)
{
    static double last_timeout = 0.0;//静态函数，上一次运行时间
    int rc;

    DEBUG(3)("comm_select: timeout %d\n", msec);

    msec = msec > MAX_POLL_TIME ? MAX_POLL_TIME : msec;

    /* 每一秒钟检查一下超时 */
    if (last_timeout + 0.999 < current_dtime) {
        last_timeout = current_dtime;
        checkTimeouts();
    }//如果距离上一个时间过去了一秒，检查超时事件，并设置为当前时间
    else {
        double max_timeout = (last_timeout + 1.0 - current_dtime) * 1000;
        if (max_timeout < msec)
            msec = (int)max_timeout;
    }//否则求出还有多少时间到下一秒，并且将这个时间设置为等待时间

    rc = comm_epoll->do_epoll_select(msec);

    getCurrentTime();//更新current_dtime

    if (rc == COMM_TIMEOUT)
        DEBUG(3) ("comm_select: time out\n");

    return rc;
}

void Comm::commUpdateWriteHandler(int fd, PF* handler, void* data)
{
    if (fd_table == nullptr || fd < 0)
        return;
    fd_table[fd].read_handler = handler;
    fd_table[fd].read_data = data;

    this->comm_epoll->epollSetEvents(fd, 0, 1);
    if (Debug) fprintf(stdout, "%d has update read event,has read handler%p,has read_data%p\n", fd, fd_table[fd].read_handler, fd_table[fd].read_data);
}

void Comm::commUpdateReadHandler(int fd, PF* handler, void* data)
{
    if (fd_table == nullptr || fd < 0)
        return;
    fd_table[fd].write_handler = handler;
    fd_table[fd].write_data = data;
    if (Debug) fprintf(stdout, "%d has update read event,has read handler%p,has read_data%p\n", fd, fd_table[fd].read_handler, fd_table[fd].read_data);

    comm_epoll->epollSetEvents(fd, 1, 0);
}

void Comm::comm_select_shutdown(void)
{
    comm_epoll->do_epoll_shutdown();
    if (fd_table) free(fd_table);
}