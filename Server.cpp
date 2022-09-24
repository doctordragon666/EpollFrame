#include "Server.h"

Comm* Server::m_comm = nullptr;
int Server::m_timeout = 30;

Server::Server(char* _ip, int _port) :m_port(_port)
{
	size_t len = strlen(_ip) + 1;
	m_ip = (char*)malloc(len * sizeof(char));
	strcat(m_ip, _ip);
}

Server::~Server()
{
	safe_free(m_ip);
}

void Server::set_comm(Comm*& comm)
{
	m_comm = comm;
}

void Server::accept_connection(int fd, void* data)
{
	printf("accept\n");
	if (m_comm == nullptr)
	{
		fprintf(stderr, "---comm is error---\n");
		return;
	}
	struct sockaddr_in peer;
	socklen_t len = sizeof(peer);

	int new_fd = accept(fd, (struct sockaddr*)&peer, &len);

	if (new_fd > 0)
	{
		ConnectStat* stat = new ConnectStat(new_fd);
		stat->set_nonblock();

		printf("new client: %s:%d\n", inet_ntoa(peer.sin_addr), ntohs(peer.sin_port));
		m_comm->commUpdateWriteHandler(new_fd, do_welcome_handler, (void*)stat);
		m_comm->commSetTimeout(new_fd, m_timeout, do_echo_timeout, (void*)stat);
	}
	else
	{
		DEBUG(3)("accept error!");
	}
}

void Server::do_echo_handler(int fd, void* data)
{
	ConnectStat* stat = (ConnectStat*)(data);
	char* p = NULL;

	assert(stat != NULL);

	p = stat->send_buf;
	*p++ = '-';
	*p++ = '>';
	ssize_t _s = read(fd, p, BUFLEN - (p - stat->send_buf) - 1); // 2字节"->" +字符结束符.
	if (_s > 0)
	{

		*(p + _s) = '\0';
		printf("receive from client: %s\n", p);
		//_s--;
		// while( _s>=0 && ( stat->send_buf[_s]=='\r' || stat->send_buf[_s]=='\n' ) ) stat->send_buf[_s]='\0';

		if (!strncasecmp(p, "quit", 4))
		{ //退出.
			m_comm->comm_close(fd);
			free(stat);
			return;
		}
		m_comm->commUpdateWriteHandler(fd, do_echo_response, (void*)stat);
		m_comm->commSetTimeout(fd, m_timeout, do_echo_timeout, (void*)stat);
	}
	else if (_s == 0) // client:close
	{
		fprintf(stderr, "Remote connection[fd: %d] has been closed\n", fd);
		m_comm->comm_close(fd);
		free(stat);
	}
	else // err occurred.
	{
		fprintf(stderr, "read faield[fd: %d], reason:%s [%d]\n", fd, strerror(errno), _s);
	}
}

void Server::do_welcome_handler(int fd, void* data)
{
	const char* WELCOME = "Welcome.\n";
	size_t wlen = strlen(WELCOME);
	size_t n;
	ConnectStat* stat = (ConnectStat*)(data);

	if ((n = write(fd, "Welcome.\n", wlen)) != wlen)
	{

		if (n <= 0)
			fprintf(stderr, "write failed[len:%d], reason: %s\n", n, strerror(errno));
		else
			fprintf(stderr, "send %d bytes only ,need to send %d bytes.\n", n, wlen);
	}
	else
	{
		m_comm->commUpdateReadHandler(fd, do_echo_handler, (void*)stat);
		m_comm->commSetTimeout(fd, m_timeout, do_echo_timeout, (void*)stat);
	}

}

int Server::startup()
{
	// 1.创建套接字
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
	{
		perror("sock");
		exit(2);
	}

	int opt = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));//设置非阻塞

	// 2.填充本地 sockaddr_in 结构体（设置本地的IP地址和端口）
	struct sockaddr_in local;
	local.sin_port = htons(m_port);
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = inet_addr(m_ip);

	// 3.bind（）绑定
	if (bind(sock, (struct sockaddr*)&local, sizeof(local)) < 0)
	{
		perror("bind");
		exit(3);
	}
	// 4.listen（）监听 检测服务器
	if (listen(sock, 5) < 0)
	{
		perror("listen");
		exit(4);
	}

	if(Debug) printf("start up success,fd=%d\n", sock);
	return sock;
}

void Server::do_echo_response(int fd, void* data)
{
	ConnectStat* stat = (ConnectStat*)(data);
	size_t len = strlen(stat->send_buf);
	size_t _s = write(fd, stat->send_buf, len);

	if (_s > 0)
	{
		m_comm->commSetTimeout(fd, 10, do_echo_timeout, (void*)stat);
		m_comm->commUpdateReadHandler(fd, do_echo_handler, (void*)stat);
	}
	else if (_s == 0)
	{
		fprintf(stderr, "---Remote connection[fd: %d] has been closed---\n", fd);
		m_comm->comm_close(fd);
		free(stat);
	}
	else
	{
		fprintf(stderr, "read faield[fd: %d], reason:%s [%d]\n", fd, _s, strerror(errno));
	}
}

void Server::do_echo_timeout(int fd, void* data)
{
	fprintf(stdout, "---------timeout[fd:%d]----------\n", fd);
	m_comm->comm_close(fd);
	free(data);
}