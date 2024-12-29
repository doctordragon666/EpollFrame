#include "EpollManager.h"
EpollManager* EpollManager::instance = nullptr;

EpollManager::EpollManager()
{
    epoll_instance = -1;//提示没有进行初始化
}

EpollManager::~EpollManager()
{
    if (epoll_instance != -1)
    {
        close(epoll_instance);
    }
}

bool EpollManager::init()
{
    epoll_instance = epoll_create(MAX_EVENTS);
    if (epoll_instance == -1)
    {
        perror("epoll_create");
        return false;
    }
    fprintf(stderr, "create success.epoll_instance:%d\n", epoll_instance);
    return true;
}

void EpollManager::set_write_event(int fd, struct MyepollEvent* arg)
{
    if (fd < 0 || fd >= MAX_EVENTS || arg == nullptr || arg->type != EPOLL_WRITE_EVENT)
    {
        DEBUG("fd:%d is invalid or event type:%d is invalid\n", fd, arg->type);
        return;
    }

    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLOUT | EPOLLHUP | EPOLLERR;
    ev.data.ptr = arg;
    // if (fd_table[fd].events & EPOLLIN)
    // {
        //已经有读事件，那么要加上
        // ev.events |= EPOLLIN;
        // ev.data.ptr = new MyepollEventPlus(fd_table[fd].data.ptr);
    if (epoll_ctl(epoll_instance, EPOLL_CTL_MOD, fd, &ev) == -1)
    {
        if (epoll_ctl(epoll_instance, EPOLL_CTL_ADD, fd, &ev) == -1)
        {
            ERROR("epoll_ctl");
            return;
        }
        DEBUG("set_write_event fd:%d\n", arg->fd);
        return;
    }

    DEBUG("set_write_event fd:%d\n", arg->fd);
}

void EpollManager::set_read_event(int fd, struct MyepollEvent*& arg)
{
    if (fd < 0 || fd >= MAX_EVENTS || arg == nullptr || !(arg->type == EPOLL_READ_EVENT || arg->type == EPOLL_MAIN_EVENT))
    {
        DEBUG("fd:%d is invalid or event type:%d is invalid\n", fd, arg->type);
        return;
    }
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLIN | EPOLLHUP | EPOLLERR;
    if (arg->type == EPOLL_MAIN_EVENT) {
        ev.events |= EPOLLET;
    }
    ev.data.ptr = arg;

    if (epoll_ctl(epoll_instance, EPOLL_CTL_ADD, fd, &ev) == -1)
    {
        ERROR("epoll_ctl");
        return;
    }
    DEBUG("set_read_event fd:%d\n", arg->fd);
}

void EpollManager::set_event(int fd, MyepollEvent* arg)
{
    if (fd < 0 || fd >= MAX_EVENTS || arg == nullptr)
    {
        DEBUG("fd:%d is invalid or event type:%d is invalid\n", fd, arg->type);
        return;
    }
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLIN | EPOLLHUP | EPOLLERR | EPOLLOUT;//该函数默认设置所有的事件
    ev.data.ptr = arg;

    if (epoll_ctl(epoll_instance, EPOLL_CTL_ADD, fd, &ev) == -1)
    {
        ERROR("epoll_ctl");
        return;
    }
    DEBUG("set_event fd:%d\n", arg->fd);
}

void EpollManager::add_server(int fd, epollCb accept_cb)
{
    struct MyepollEvent* arg = new MyepollEvent;
    arg->fd = fd;
    arg->type = EPOLL_MAIN_EVENT;
    arg->read_cb = accept_cb;
    arg->read_data = nullptr;
    set_read_event(fd, arg);
}

void EpollManager::remove_event(int fd)
{
    perror("test");
}

void EpollManager::set_timeout_event(int fd, MyepollEvent* arg)
{
}

int EpollManager::select()
{
    int nevents = epoll_wait(epoll_instance, fd_table, MAX_EVENTS, 1000);
    if (nevents < 0)
    {
        ERROR("EpollManager::select epoll failure");
        return EPOLL_ERROR;
    }
    else if (nevents == 0)
    {
        DEBUG("no events\n");
        return EPOLL_OK;
    }
    for (int i = 0; i < nevents; i++)
    {
        MyepollEvent* arg = (MyepollEvent*)fd_table[i].data.ptr;
        if (fd_table[i].events & EPOLLIN)
        {
            DEBUG("\nfd:%d read event\n", arg->fd);
            arg->read_cb(arg->fd, arg->read_data);
        }
        else if (fd_table[i].events & EPOLLOUT)
        {
            DEBUG("\nfd:%d write event\n", arg->fd);
            arg->write_cb(arg->fd, arg->write_data);
        }
    }
    return EPOLL_OK;
}

void EpollManager::del_event(int fd)
{
    if (fd < 0 || fd >= MAX_EVENTS) return;
    epoll_ctl(epoll_instance, EPOLL_CTL_DEL, fd, nullptr);
}
