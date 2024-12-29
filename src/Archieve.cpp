// #ifndef _SERVER_H_
// #define _SERVER_H_
// #include "EpollManager.h"
// #ifndef _COMM_EPOLL_H_
// #define _COMM_EPOLL_H_

// #include <sys/epoll.h>
// #include "global.h"

// // namespace EpollManager{
// // 	fde* m_fde_table;

// // };

// /// <summary>
// /// epoll封装接口
// /// </summary>
// #ifndef _COMM_H
// #define _COMM_H
// #include "global.h"
// #include "EpollManager.h"

// // namespace Network
// // {
// // 	int server_fd;
	
// // }

// class Network
// {
// public:
// 	Network(int max_fd);
// 	~Network();

// 	void comm_init();


// 	time_t commSetTimeout(int fd, int timeout, PF* handler, void* data);

// 	void comm_close(int fd);

// 	void checkTimeouts(void);


// 	int comm_select(int msec);


// 	void comm_select_shutdown(void);


// 	void commUpdateWriteHandler(int fd, PF* handler, void* data);
	

// 	void commUpdateReadHandler(int fd, PF* handler, void* data);

// private:
// 	int Biggest_FD = 1024;  /* 默认的最大文件描述符数量 1024 */
// 	int MAX_POLL_TIME = 1000;	// epoll最大获取事件的间隔
// 	EpollManager* comm_epoll;//调用epoll的接口

// public:
// 	fde* fd_table = nullptr;//句柄表，允许外部变量修改这个值，不适用桥接模式来更新
// };

// #endif // !_COMM_H

// #include "Network.h"

// Network::Network(int max_fd)
// {
//     if (max_fd > 0) Biggest_FD = max_fd;
// }

// Network::~Network()
// {
//     Biggest_FD = 1024;
//     safe_free(fd_table);
// }

// void Network::comm_init()
// {
//     this->fd_table = (fde*)calloc(Biggest_FD, sizeof(fde));

//     if (!fd_table)
//         fprintf(stderr, "calloc fd table fail.");

//     if (Debug) fprintf(stdout, "%p has create %d connection\n", this->fd_table, Biggest_FD);
//     this->comm_epoll = new EpollManager(this->fd_table);

//     comm_epoll->do_epoll_init(Biggest_FD);
// }

// time_t Network::commSetTimeout(int fd, int timeout, PF* handler, void* data)
// {
//     fde* F;
//     DEBUG(5) ("commSetTimeout: FD %d timeout %d\n", fd, timeout);
//     assert(fd >= 0);
//     assert(fd < Biggest_FD);
//     F = &fd_table[fd];

//     if (timeout < 0) {
//         F->timeout_handler = NULL;
//         F->timeout_data = NULL;
//         return F->timeout = (time_t)0;
//     }//如果设置的超时小于零返回空fde
//     assert(handler || F->timeout_handler);
//     if (handler || data) {
//         F->timeout_handler = handler;
//         F->timeout_data = data;
//     }
//     return F->timeout = sys_curtime + (time_t)timeout;
// }

// void Network::comm_close(int fd)
// {
//     assert(fd > 0);
//     fde* F = &fd_table[fd];
//     if (F) memset((void*)F, '\0', sizeof(fde));
//     comm_epoll->epollSetEvents(fd, 0, 0);//清空这个socket上的事件
//     close(fd);
// }

// void Network::checkTimeouts(void)
// {
//     int fd;
//     fde* F = nullptr;
//     PF* callback;
//     DEBUG(3)("checktimeouts\n");

//     for (fd = 0; fd <= Biggest_FD; fd++) {
//         F = &fd_table[fd];

//         if (F->timeout == 0)
//             continue;
//         if (F->timeout > sys_curtime)
//             continue;//找下一个超时的
//         DEBUG(5) ("checkTimeouts: FD %d Expired\n", fd);

//         if (F->timeout_handler) {
//             DEBUG(5) ("checkTimeouts: FD %d: Call timeout handler\n", fd);
//             callback = F->timeout_handler;
//             F->timeout_handler = nullptr;
//             callback(fd, F->timeout_data);
//         }
//         else {
//             DEBUG(5) ("checkTimeouts: FD %d: Forcing comm_close()\n", fd);
//             comm_close(fd);
//         }
//     }
// }

// int Network::comm_select(int msec)
// {
//     static double last_timeout = 0.0;//静态函数，上一次运行时间
//     int rc;

//     DEBUG(3)("comm_select: timeout %d\n", msec);

//     msec = msec > MAX_POLL_TIME ? MAX_POLL_TIME : msec;

//     /* 每一秒钟检查一下超时 */
//     if (last_timeout + 0.999 < current_dtime) {
//         last_timeout = current_dtime;
//         checkTimeouts();
//     }//如果距离上一个时间过去了一秒，检查超时事件，并设置为当前时间
//     else {
//         double max_timeout = (last_timeout + 1.0 - current_dtime) * 1000;
//         if (max_timeout < msec)
//             msec = (int)max_timeout;
//     }//否则求出还有多少时间到下一秒，并且将这个时间设置为等待时间

//     rc = comm_epoll->do_epoll_select(msec);

//     getCurrentTime();//更新current_dtime

//     if (rc == COMM_TIMEOUT)
//         DEBUG(3) ("comm_select: time out\n");

//     return rc;
// }

// void Network::commUpdateWriteHandler(int fd, PF* handler, void* data)
// {
//     if (fd_table == nullptr || fd < 0)
//         return;
//     fd_table[fd].read_handler = handler;
//     fd_table[fd].read_data = data;

//     this->comm_epoll->epollSetEvents(fd, 0, 1);
//     if (Debug) fprintf(stdout, "%d has update read event,has read handler%p,has read_data%p\n", fd, fd_table[fd].read_handler, fd_table[fd].read_data);
// }

// void Network::commUpdateReadHandler(int fd, PF* handler, void* data)
// {
//     if (fd_table == nullptr || fd < 0)
//         return;
//     fd_table[fd].write_handler = handler;
//     fd_table[fd].write_data = data;
//     if (Debug) fprintf(stdout, "%d has update read event,has read handler%p,has read_data%p\n", fd, fd_table[fd].read_handler, fd_table[fd].read_data);

//     comm_epoll->epollSetEvents(fd, 1, 0);
// }

// void Network::comm_select_shutdown(void)
// {
//     comm_epoll->do_epoll_shutdown();
//     if (fd_table) free(fd_table);
// }
// class EpollManager
// {
// public:
// 	EpollManager(fde*& fd_table);
// 	~EpollManager();

// 	static const char* epolltype_atoi(int x);


// 	void comm_call_handlers(int fd, int read_event, int write_event);


// 	void do_epoll_shutdown();


// 	int do_epoll_select(int msec);


// 	void epollSetEvents(int fd, int need_read, int need_write);
// private:
// 	fde* m_fde_table;//comm的fde表，减少耦合不使用comm对象
// 	struct epoll_event events[MAX_EVENTS];//存放epoll事件的数组
// 	int epoll_instance;//epoll实例句柄
// 	int epoll_fds;//epoll事件的总和
// 	unsigned int* epoll_state; // 保存每个epoll 的事件状态 
// };

// #endif // !_COMM_EPOLL_H_

// // EpollManager::EpollManager(fde*& fd_table)
// {
// 	m_fde_table = fd_table;
// 	epoll_fds = 0;
// 	//printf("comm_epoll, fd_table%p init\n",m_fde_table);//为了防止多次构造，可以使用单例模式
// }

// // EpollManager::EpollManager()
// // {
// // }

// EpollManager::~EpollManager()
// {
// 	do_epoll_shutdown();//忘记关闭在析构关闭
// }

// const char* EpollManager::epolltype_atoi(int x)
// {
// 	switch (x)
// 	{
// 	case EPOLL_CTL_ADD:
// 		return "EPOLL_CTL_ADD";

// 	case EPOLL_CTL_DEL:
// 		return "EPOLL_CTL_DEL";

// 	case EPOLL_CTL_MOD:
// 		return "EPOLL_CTL_MOD";

// 	default:
// 		return "UNKNOWN_EPOLLCTL_OP";
// 	}
// }

// void EpollManager::comm_call_handlers(int fd, int read_event, int write_event)
// {
// 	if (fd < 0)
// 	{
// 		fprintf(stderr, "file is error");
// 		return;
// 	}

// 	fde F = m_fde_table[fd];
// 	if (F.read_handler && read_event) {
// 		PF* hdl = F.read_handler;
// 		DEBUG(5) ("comm_call_handlers(): Calling read handler on fd=%d\n", fd);
// 		hdl(fd, F.read_data);
// 	}
// 	else
// 	{
// 		DEBUG(3) ("\nread event is null or not set read_handler %p\n", m_fde_table[fd].read_handler);
// 	}

// 	if (F.write_handler && write_event) {

// 		PF* hdl = F.write_handler;
// 		hdl(fd, F.write_data);
// 	}
// 	else
// 	{
// 		DEBUG(3) ("\nwrite event is null or not set write_handler %p\n", m_fde_table[fd].read_handler);
// 	}
// }

// void EpollManager::do_epoll_shutdown()
// {
// 	close(epoll_instance);
// 	epoll_instance = -1;
// 	safe_free(epoll_state);
// }

// int EpollManager::do_epoll_select(int msec)
// {
// 	DEBUG(3) ("do_epoll_select...\n");

// 	int num = epoll_wait(epoll_instance, events, MAX_EVENTS, msec);
// 	if (num < 0)
// 	{
// 		getCurrentTime();
// 		if (ignoreErrno(errno))
// 			return COMM_OK;

// 		DEBUG(5)("comm_select: epoll failure: %s\n", xstrerror());
// 		return COMM_ERROR;
// 	}

// 	if (num == 0)
// 	{
// 		DEBUG(3)("select time out or no connect\n");
// 		return COMM_TIMEOUT;
// 	}

// 	int i;
// 	int fd;
// 	struct epoll_event* tmp_events;
// 	for (i = 0, tmp_events = events; i < num; i++, tmp_events++)
// 	{
// 		fd = tmp_events->data.fd;
// 		comm_call_handlers(fd, tmp_events->events & EPOLLOUT, tmp_events->events & EPOLLIN);
// 	}
// 	return COMM_OK;
// }

// void EpollManager::epollSetEvents(int fd, int need_read, int need_write)
// {
// 	int epoll_ctl_type = 0;
// 	struct epoll_event ev;

// 	assert(fd >= 0);
// 	DEBUG(5)("commSetEvents(fd=%d)\n", fd);

// 	memset(&ev, 0, sizeof(ev));
// 	ev.events = 0;
// 	ev.data.fd = fd;

// 	if (need_read)
// 		ev.events |= EPOLLIN;

// 	if (need_write)
// 		ev.events |= EPOLLOUT;

// 	if (ev.events)
// 		ev.events |= EPOLLHUP | EPOLLERR;

// 	if (ev.events != epoll_state[fd])
// 	{
// 		/* 如果epoll的事件已经保存了状态 */
// 		if (!ev.events)
// 		{
// 			epoll_ctl_type = EPOLL_CTL_DEL;
// 		}
// 		else if (epoll_state[fd])
// 		{
// 			epoll_ctl_type = EPOLL_CTL_MOD;
// 		}//如果相对原来的有改变
// 		else
// 		{
// 			epoll_ctl_type = EPOLL_CTL_ADD;
// 		}

// 		epoll_state[fd] = ev.events;//更新事件

// 		if (epoll_ctl(epoll_instance, epoll_ctl_type, fd, &ev) < 0)
// 		{
// 			DEBUG(5)("commSetEvents: epoll_ctl(%s): failed on fd=%d: %s\n",
// 				epolltype_atoi(epoll_ctl_type), fd, xstrerror());
// 			return;
// 		}	
// 		switch (epoll_ctl_type)
// 		{
// 		case EPOLL_CTL_ADD:
// 			epoll_fds++;
// 			break;
// 		case EPOLL_CTL_DEL:
// 			epoll_fds--;
// 			break;
// 		default:
// 			break;
// 		}
// 		DEBUG(5)("commSetEvents: epoll_ctl%d has %d event \n", epoll_instance, epoll_fds);
// 	}
// }

// #include "global.h"
// #include "Network.h"

// struct ConnectStat
// {
// 	int fd;
// 	char send_buf[BUFLEN];
// 	PF* handler; //不同页面的处理函数

// 	ConnectStat(int fd)
// 	{
// 		this->fd = fd;
// 	}
// 	void set_nonblock()
// 	{
// 		int fl = fcntl(this->fd, F_GETFL);
// 		fcntl(this->fd, F_SETFL, fl | O_NONBLOCK);
// 	}
// };

// /// <summary>
// /// 流程概述
// /// 1、先等待回调accept_connection
// /// 2、执行do_welcome_handler
// /// 3、执行do_echo_handler
// /// 4、执行do_echo_response
// /// 循环执行3，4
// /// </summary>
// class Server
// {
// public:
// 	Server( char* _ip, int _port);
// 	~Server();


// 	static void set_comm(Network*& comm);


// 	static void accept_connection(int fd, void* data);


// 	static void do_echo_handler(int fd, void* data);


// 	static void do_welcome_handler(int fd, void* data);


// 	static void do_echo_response(int fd, void* data);

// 	static void do_echo_timeout(int fd, void* data);
// private:
// 	char* m_ip;//服务器IP
// 	int m_port;//服务器端口
// 	static Network* m_comm;//通用连接类
// 	static int m_timeout;//默认超时处理
// };

// #endif // !_SERVER_H_

// Network* Server::m_comm = nullptr;
// int Server::m_timeout = 30;

// Server::~Server()
// {
// 	safe_free(m_ip);
// }

// void Server::set_comm(Network*& comm)
// {
// 	m_comm = comm;
// }

// void Server::accept_connection(int fd, void* data)
// {
// 	printf("accept\n");
// 	if (m_comm == nullptr)
// 	{
// 		fprintf(stderr, "---comm is error---\n");
// 		return;
// 	}
// 	struct sockaddr_in peer;
// 	socklen_t len = sizeof(peer);

// 	int new_fd = accept(fd, (struct sockaddr*)&peer, &len);

// 	if (new_fd > 0)
// 	{
// 		ConnectStat* stat = new ConnectStat(new_fd);
// 		stat->set_nonblock();

// 		printf("new client: %s:%d\n", inet_ntoa(peer.sin_addr), ntohs(peer.sin_port));
// 		m_comm->commUpdateWriteHandler(new_fd, do_welcome_handler, (void*)stat);
// 		m_comm->commSetTimeout(new_fd, m_timeout, do_echo_timeout, (void*)stat);
// 	}
// 	else
// 	{
// 		DEBUG(3)("accept error!");
// 	}
// }

// void Server::do_echo_handler(int fd, void* data)
// {
// 	ConnectStat* stat = (ConnectStat*)(data);
// 	char* p = NULL;

// 	assert(stat != NULL);

// 	p = stat->send_buf;
// 	*p++ = '-';
// 	*p++ = '>';
// 	ssize_t _s = read(fd, p, BUFLEN - (p - stat->send_buf) - 1); // 2字节"->" +字符结束符.
// 	if (_s > 0)
// 	{

// 		*(p + _s) = '\0';
// 		printf("receive from client: %s\n", p);
// 		//_s--;
// 		// while( _s>=0 && ( stat->send_buf[_s]=='\r' || stat->send_buf[_s]=='\n' ) ) stat->send_buf[_s]='\0';

// 		if (!strncasecmp(p, "quit", 4))
// 		{ //退出.
// 			m_comm->comm_close(fd);
// 			free(stat);
// 			return;
// 		}
// 		m_comm->commUpdateWriteHandler(fd, do_echo_response, (void*)stat);
// 		m_comm->commSetTimeout(fd, m_timeout, do_echo_timeout, (void*)stat);
// 	}
// 	else if (_s == 0) // client:close
// 	{
// 		fprintf(stderr, "Remote connection[fd: %d] has been closed\n", fd);
// 		m_comm->comm_close(fd);
// 		free(stat);
// 	}
// 	else // err occurred.
// 	{
// 		fprintf(stderr, "read faield[fd: %ld], reason:%s [%d]\n", fd, strerror(errno), _s);
// 	}
// }

// void Server::do_welcome_handler(int fd, void* data)
// {
// 	const char* WELCOME = "Welcome.\n";
// 	size_t wlen = strlen(WELCOME);
// 	size_t n;
// 	ConnectStat* stat = (ConnectStat*)(data);

// 	if ((n = write(fd, "Welcome.\n", wlen)) != wlen)
// 	{

// 		if (n <= 0)
// 			fprintf(stderr, "write failed[len:%d], reason: %s\n", n, strerror(errno));
// 		else
// 			fprintf(stderr, "send %d bytes only ,need to send %d bytes.\n", n, wlen);
// 	}
// 	else
// 	{
// 		m_comm->commUpdateReadHandler(fd, do_echo_handler, (void*)stat);
// 		m_comm->commSetTimeout(fd, m_timeout, do_echo_timeout, (void*)stat);
// 	}

// }

// void Server::do_echo_response(int fd, void* data)
// {
// 	ConnectStat* stat = (ConnectStat*)(data);
// 	size_t len = strlen(stat->send_buf);
// 	size_t _s = write(fd, stat->send_buf, len);

// 	if (_s > 0)
// 	{
// 		m_comm->commSetTimeout(fd, 10, do_echo_timeout, (void*)stat);
// 		m_comm->commUpdateReadHandler(fd, do_echo_handler, (void*)stat);
// 	}
// 	else if (_s == 0)
// 	{
// 		fprintf(stderr, "---Remote connection[fd: %d] has been closed---\n", fd);
// 		m_comm->comm_close(fd);
// 		free(stat);
// 	}
// 	else
// 	{
// 		fprintf(stderr, "read faield[fd: %d], reason:%s [%d]\n", fd, _s, strerror(errno));
// 	}
// }

// void Server::do_echo_timeout(int fd, void* data)
// {
// 	fprintf(stdout, "---------timeout[fd:%d]----------\n", fd);
// 	m_comm->comm_close(fd);
// 	free(data);
// }