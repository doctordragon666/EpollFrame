/*************************************************
Date: 2024年12月1日10点59分
Description: 使用network的接口打开一个服务器，
然后调用epoll进行监听，并处理事件
*************************************************/

#include "Network.h"
#include <memory>

const char* IP = "127.0.0.1";


int main() {
    std::shared_ptr< Network::TCPSocket> tcp_socket(new Network::TCPSocket(IP, 8080));
    if (!tcp_socket->setAsServer()) {
        puts("Failed to set as server");
        return -1;
    }

    if(!EM->init()) {
        ERROR("please restart program");
        return -1;
    }
    
    EM->add_server(tcp_socket.get()->m_fd,Network::accept);
    int res = EPOLL_OK;
    while (res == EPOLL_OK)
    {
        res = EM->select();
    }
    
    return 0;
}