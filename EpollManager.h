#ifndef EPOLL_MANAGER_H
#define EPOLL_MANAGER_H

/****************************************
 * EpollManager封装的单例模式
 * 用于管理epoll事件，提供初始化设置事件
 * 添加服务器，选择和删除事件等操作
 ***************************************/

#include "global.h"
#include <sys/epoll.h>

#define MAX_EVENTS 1024
class EpollManager{
public:
    ~EpollManager();
    bool init();
    void set_write_event(int fd, struct MyepollEvent*& arg);
    void set_read_event(int fd, struct MyepollEvent*& arg);
    void set_event(int fd, struct MyepollEvent* arg);
    void add_server(int fd, epollCb accept_cb);
    int select(int timeout = 1000);
    void del_event(int fd);
    EpollManager(const EpollManager&) = delete;
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

#define EM EpollManager::get_instance() //  定义EM宏，用于获取EpollManager实例
#endif