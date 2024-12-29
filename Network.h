#ifndef NETWORK_H
#define NETWORK_H

/****************************************
 * Network.h头文件说明
 * 定义了TCPSocket类，用于封装TCP套接字操作
 * 提供了set_nonblock函数用于设置非阻塞
 * welcome、read、accept函数用于处理TCP连接
 ***************************************/

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "EpollManager.h"

/// @brief 设置非阻塞
/// @param fd 文件描述符
inline void set_nonblock(int& fd) {
    int flag = fcntl(fd, F_GETFL, 0);
    if (flag == -1) {
        perror("fcntl");
        return;
    }
    flag |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flag) == -1) {
        perror("fcntl");
        return;
    }
}

namespace Network {
    int queueSize = 10;//listen queue size
    bool ServerDebug = true;
    int reuseTime = 1;
    void welcome(int fd, void* arg);
    void read(int fd, void* arg);
    void accept(int fd, void* arg)
    {
        printf("--------accept-------\n");
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int clientfd = accept(fd, (struct sockaddr*)&client_addr, &client_addr_len);
        set_nonblock(clientfd);

        if (clientfd == -1) {
            perror("accept");
            return;
        }

        if (Network::ServerDebug) {
            printf("Client from: %s : % d(IP : port), "
                "clientfd = %d \n",
                inet_ntoa(client_addr.sin_addr),
                ntohs(client_addr.sin_port),
                clientfd);
        }

        // 设置客户端的读取，超时和写入事件
        MyepollEvent* event = new MyepollEvent();
        event->fd = clientfd;
        event->read_data = nullptr;
        event->read_cb = Network::read;
        event->write_data = nullptr;
        event->write_cb = Network::welcome;
        EM->set_event(clientfd, event);
    }
    struct TCPSocket {
        int m_fd;
        struct sockaddr_in m_addr;
        TCPSocket(char* ip, int port) {
            m_fd = socket(AF_INET, SOCK_STREAM, 0);
            m_addr.sin_family = AF_INET;
            m_addr.sin_port = htons(port);
            inet_pton(AF_INET, ip, &m_addr.sin_addr);
            if (setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, (void*)&reuseTime, sizeof(int)) == -1)
            {
                ERROR("setsockopt");
            }
        }

        TCPSocket(const char* ip, int port) {
            m_fd = socket(AF_INET, SOCK_STREAM, 0);
            m_addr.sin_family = AF_INET;
            m_addr.sin_port = htons(port);
            inet_pton(AF_INET, ip, &m_addr.sin_addr);
            if (setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, (void*)&reuseTime, sizeof(int)) == -1)
            {
                ERROR("setsockopt");
            }
        }

        ~TCPSocket() {
            close(m_fd);
        }

        bool setAsServer();
        bool setAsClient();
    };
}

bool Network::TCPSocket::setAsServer()
{
    if (bind(m_fd, (struct sockaddr*)&m_addr, sizeof(m_addr)) == -1) {
        perror("bind");
        return false;
    }
    if (listen(m_fd, Network::queueSize) == -1) {
        perror("listen");
        return false;
    }
    if (Network::ServerDebug) {
        printf("Server from: %s : % d(IP : port), "
            "Serverfd = %d \n",
            inet_ntoa(m_addr.sin_addr),
            ntohs(m_addr.sin_port),
            m_fd);
    }
    return true;
}

bool Network::TCPSocket::setAsClient()
{
    if (connect(m_fd, (struct sockaddr*)&m_addr, sizeof(m_addr)) == -1) {
        perror("connect");
        return false;
    }
    return true;
}


void Network::welcome(int fd, void* arg)
{
    printf("--------welcome-------\n");
    //读取welcome.html文件
    const char* html = "../welcome.html";
    int fp = open(html, O_RDONLY);
    if (fp == -1) {
        perror("open");
        return;
    }

    //读取文件所有内容并存放content数组中
    struct stat st;
    fstat(fp, &st);
    char* content = (char*)malloc(st.st_size + 1);
    int res = ::read(fp, content, st.st_size);
    if (res == -1) {
        ERROR("read");
        close(fp);
        EM->del_event(fd);
        return;
    }
    else {
        printf("read %s success \n", html);
    }
    content[st.st_size] = '\0';
    res = ::write(fd, content, st.st_size + 1);
    if (res == -1) {
        perror("write");
        close(fd);
        EM->del_event(fd);
        return;
    }
    else if (res == 0) {
        printf("client closed\n");
        close(fd);
        return;
    }
    else {
        printf("send %d bytes\n", res);
        if (res == st.st_size + 1)free(content);
    }
    EM->del_event(fd);
}

void Network::read(int fd, void* arg)
{
    printf("--------read-------\n");
    char buf[1024];
    int n = ::read(fd, buf, sizeof(buf));
    if (n == 0) {
        printf("client closed\n");
        close(fd);
        EM->del_event(fd);
        return;
    }
    else if (n == -1) {
        perror("read");
        close(fd);
        EM->del_event(fd);
        return;
    }
    printf("client say: %s\n", buf);
}

#endif // NETWORK_H