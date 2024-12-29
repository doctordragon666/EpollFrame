/*************************************************
Date: 2024年12月1日10点59分
Description: epoll实现http服务器
graph LR
A[客户端连接] --> B[将客户端的fd加入epoll]
B --> C[等待客户端发送请求]
C --> D[读取请求内容到缓冲区]
D --> E[根据请求内容调用不同的处理函数]
E --> F[将响应内容写入缓冲区]

原理说明：使用epoll监听客户端的连接，当客户端连接时，
将客户端的fd加入epoll，然后等待客户端发送请求，
当客户端发送请求时，将请求内容读取到缓冲区，
然后根据请求内容，调用不同的处理函数，
处理函数将响应内容写入缓冲区

函数说明：
stat_init：初始化连接状态
connect_handle：处理客户端连接
do_http_request：处理http请求
welcome_response_handler：处理欢迎页面请求
commit_respone_handler：处理提交页面请求
*************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <errno.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include "Network.h"

typedef struct _ConnectStat ConnectStat;

typedef void (*response_handler)(ConnectStat* stat);
const int debug = 0;

struct _ConnectStat
{
    int fd;
    char name[64];
    char age[64];
    struct epoll_event _ev;
    int status;               // 0 -未登录   1 - 已登陆
    response_handler handler; //不同页面的处理函数
};

ConnectStat* stat_init(int fd);
void connect_handle(int new_fd);
void do_http_request(ConnectStat* stat);
void welcome_response_handler(ConnectStat* stat);
void commit_respone_handler(ConnectStat* stat);

const char* main_header = "HTTP/1.0 200 OK\r\nServer: Dragon Server\r\nContent-Type: text/html\r\nConnection: Close\r\n";

static int epfd = 0;

/// @brief 初始化连接状态
/// @param fd 服务器句柄
/// @return 返回包含服务器句柄未登陆的连接状态
ConnectStat* stat_init(int fd)
{
    ConnectStat* temp = (ConnectStat*)malloc(sizeof(ConnectStat));

    if (!temp)
    {
        fprintf(stderr, "malloc failed. reason: %m\n");
        return NULL;
    }

    memset(temp, '\0', sizeof(ConnectStat));
    temp->fd = fd;
    temp->status = 0;
    temp->handler = welcome_response_handler;
    return temp;
}

/// @brief 初始化连接，然后等待浏览器发送请求
/// @param new_fd 客户端的fd
void connect_handle(int new_fd)
{
    ConnectStat* stat = stat_init(new_fd);
    set_nonblock(new_fd);

    stat->_ev.events = EPOLLIN;
    stat->_ev.data.ptr = stat;

    epoll_ctl(epfd, EPOLL_CTL_ADD, new_fd, &stat->_ev); //将客户端句柄添加到epoll中
}

/// @brief 处理请求
/// @param stat
void do_http_request(ConnectStat* stat)
{
    //读取和解析http 请求
    char buf[4096];
    char* pos = NULL;
    // while  header \r\n\r\ndata
    ssize_t _s = read(stat->fd, buf, sizeof(buf) - 1);
    if (_s > 0)
    {
        buf[_s] = '\0';
        printf("receive from client:%s\n", buf);

        pos = buf;

        // GET /favicon.ico HTTP/1.1比较前三个字符
        if (!strncasecmp(pos, "GET", 3))
        {
            stat->handler = welcome_response_handler;
        }
        else if (!strncasecmp(pos, "Post", 4))
        {
            //获取 uri
            printf("---Post----\n");
            pos += strlen("Post");
            while (*pos == ' ' || *pos == '/')
                ++pos;

            if (!strncasecmp(pos, "commit", 6))
            { //获取名字和年龄
                int len = 0;

                printf("post commit --------\n");
                pos = strstr(buf, "\r\n\r\n");
                char* end = NULL;
                if (end = strstr(pos, "name="))
                {
                    pos = end + strlen("name=");
                    end = pos;
                    while (('a' <= *end && *end <= 'z') || ('A' <= *end && *end <= 'Z') || ('0' <= *end && *end <= '9'))
                        end++;
                    len = end - pos;
                    if (len > 0)
                    {
                        memcpy(stat->name, pos, end - pos);
                        stat->name[len] = '\0';
                    }
                }

                if (end = strstr(pos, "age="))
                {
                    pos = end + strlen("age=");
                    end = pos;
                    while ('0' <= *end && *end <= '9')
                        end++;
                    len = end - pos;
                    if (len > 0)
                    {
                        memcpy(stat->age, pos, end - pos);
                        stat->age[len] = '\0';
                    }
                }
                stat->handler = commit_respone_handler;
            }
            else
            {
                stat->handler = welcome_response_handler;
            }
        }
        else
        {
            stat->handler = welcome_response_handler;
        }

        //生成处理结果 html ,write
        stat->_ev.events = EPOLLOUT;
        epoll_ctl(epfd, EPOLL_CTL_MOD, stat->fd, &stat->_ev); //二次托管
    }
    else if (_s == 0) // client:close
    {
        printf("client: %d close\n", stat->fd);
        epoll_ctl(epfd, EPOLL_CTL_DEL, stat->fd, NULL);
        close(stat->fd);
        free(stat);
    }
    else
    {
        perror("read");
    }
}

/// @brief 欢迎界面
/// @param stat
void welcome_response_handler(ConnectStat* stat)
{
    const char* welcome_content = "\
<html lang=\"zh-CN\">\n\
<head>\n\
<meta content=\"text/html; charset=utf-8\" http-equiv=\"Content-Type\">\n\
<title>This is a test</title>\n\
</head>\n\
<body>\n\
<div align=center height=\"500px\" >\n\
<br/><br/><br/>\n\
<h2>大家好，欢迎使用EpollFrame</h2><br/><br/>\n\
<form action=\"commit\" method=\"post\">\n\
尊姓大名: <input type=\"text\" name=\"name\" />\n\
<br/>芳龄几何: <input type=\"password\" name=\"age\" />\n\
<br/><br/><br/><input type=\"submit\" value=\"提交\" />\n\
<input type=\"reset\" value=\"重置\" />\n\
</form>\n\
</div>\n\
</body>\n\
</html>";

    char sendbuffer[4096];
    char content_len[64];

    strcpy(sendbuffer, main_header);
    snprintf(content_len, 64, "Content-Length: %d\r\n\r\n", (int)strlen(welcome_content));
    strcat(sendbuffer, content_len);
    strcat(sendbuffer, welcome_content);
    if (debug)
        printf("send reply to client \n%s", sendbuffer);

    write(stat->fd, sendbuffer, strlen(sendbuffer));

    stat->_ev.events = EPOLLIN;
    // stat->_ev.data.ptr = stat;
    epoll_ctl(epfd, EPOLL_CTL_MOD, stat->fd, &stat->_ev);
}

void commit_respone_handler(ConnectStat* stat)
{
    const char* commit_content = "\
<html lang=\"zh-CN\">\n\
<head>\n\
<meta content=\"text/html; charset=utf-8\" http-equiv=\"Content-Type\">\n\
<title>This is a test</title>\n\
</head>\n\
<body>\n\
<div align=center height=\"500px\" >\n\
<br/><br/><br/>\n\
<h2>欢迎学霸同学&nbsp;%s &nbsp;,你的芳龄是&nbsp;%s！</h2><br/><br/>\n\
</div>\n\
</body>\n\
</html>\n";

    char sendbuffer[4096];
    char content[4096];
    char content_len[64];
    int len = 0;

    len = snprintf(content, 4096, commit_content, stat->name, stat->age);
    strcpy(sendbuffer, main_header);
    snprintf(content_len, 64, "Content-Length: %d\r\n\r\n", len);
    strcat(sendbuffer, content_len);
    strcat(sendbuffer, content);
    if (debug)
        printf("send reply to client \n%s", sendbuffer);

    write(stat->fd, sendbuffer, strlen(sendbuffer));

    stat->_ev.events = EPOLLIN;
    epoll_ctl(epfd, EPOLL_CTL_MOD, stat->fd, &stat->_ev);
}

int main(int argc, char* argv[])
{
    Network::TCPSocket sock("127.0.0.1", 80);
    if (!sock.setAsServer())
    {
        return -1;
    }
    int listen_sock = sock.m_fd;
    ConnectStat* stat = stat_init(listen_sock);

    // 1.创建epoll
    epfd = epoll_create(256); //可处理的最大句柄数256个
    if (epfd < 0)
    {
        perror("epoll_create");
        exit(5);
    }

    struct epoll_event ev; // epoll结构填充
    ev.events = EPOLLIN; //监听读事件
    ev.data.ptr = stat;

    // 2.托管
    epoll_ctl(epfd, EPOLL_CTL_ADD, listen_sock, &ev); //将listen sock添加到epfd中

    struct epoll_event revs[64]; //事件组

    int timeout = -1;
    int num = 0;

    while (true)
    {
        // epoll_wait()相当于在检测事件
        switch ((num = epoll_wait(epfd, revs, 64, timeout))) //返回需要处理的事件数目  64表示 事件有多大
        {
        case 0: //返回0 ，表示监听超时
            printf("timeout\n");
            break;
        case -1: //出错
            perror("epoll_wait");
            break;
        default: //大于零 即就是返回了需要处理事件的数目
        {
            struct sockaddr_in peer;
            socklen_t len = sizeof(peer);

            for (int i = 0; i < num; i++)
            {
                ConnectStat* stat = (ConnectStat*)revs[i].data.ptr;

                int rsock = stat->fd;                                    //准确获取哪个事件的描述符
                if (rsock == listen_sock && (revs[i].events) && EPOLLIN) //如果是初始的 就接受，建立链接
                {
                    int new_fd = accept(listen_sock, (struct sockaddr*)&peer, &len);

                    if (new_fd > 0)
                    {
                        printf("get a new client:%s:%d\n", inet_ntoa(peer.sin_addr), ntohs(peer.sin_port));
                        // sleep(1000);
                        connect_handle(new_fd);
                    }
                }
                else // 接下来对num - 1 个事件处理
                {
                    if (revs[i].events & EPOLLIN)
                    {
                        do_http_request(stat);
                    }
                    else if (revs[i].events & EPOLLOUT)
                    {
                        stat->handler(stat);
                    }
                }
            }
            printf("------------\n");
        }
        break;
        } // end switch
    }     // end while
    return 0;
}