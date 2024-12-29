#include "Network.h"
#include <memory>

const char* IP = "127.0.0.1";


int main() {
    std::shared_ptr< Network::TCPSocket> tcp_socket(new Network::TCPSocket(IP, 8080));
    if (!tcp_socket->setAsServer()) {
        puts("Failed to set as server");
        return -1;
    }

    EM->init();
    EM->add_server(tcp_socket.get()->m_fd,Network::accept);
    int res = EPOLL_OK;
    while (res == EPOLL_OK)
    {
        res = EM->select();
    }
    
    return 0;
}