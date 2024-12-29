#ifndef EPOLL_MANAGER_H
#define EPOLL_MANAGER_H

#include "global.h"
#include <sys/epoll.h>

#define MAX_EVENTS 1024
class EpollManager{
public:
    ~EpollManager();
    bool init();
    void set_write_event(int fd, struct MyepollEvent* arg);
    void set_read_event(int fd, struct MyepollEvent*& arg);
    void set_event(int fd, struct MyepollEvent* arg);
    void add_server(int fd, epollCb accept_cb);
    void remove_event(int fd);
    void set_timeout_event(int fd, struct MyepollEvent* arg);
    int select();
    void del_event(int fd);
    static EpollManager* get_instance()
    {
        if(instance == nullptr)
            instance = new EpollManager();
        return instance;
    }
private:
    static EpollManager* instance;
    EpollManager();
    struct epoll_event fd_table[MAX_EVENTS];
    int epoll_instance;
};

#define EM EpollManager::get_instance()
#endif