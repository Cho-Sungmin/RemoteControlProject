//============================================================================
// Name        : EpollUdpServer.cpp
// Author      : Cho-Sungmin
// Version     :
// Copyright   : Your copyright notice
// Description :
//============================================================================

#include <src/EpollUdpServer.h>

EpollUdpServer::EpollUdpServer()
{
	/* base class do
	create socket */
}
EpollUdpServer::~EpollUdpServer()
{
}
EpollUdpServer::EpollUdpServer(const EpollUdpServer& lvalue)
{
	this->fd_epoll = lvalue.fd_epoll;
	this->events = lvalue.events;
}
int EpollUdpServer::setSocketNonBlocking()
{
/* make the socket non-blocking */
	int flags = fcntl(serverSocket, F_GETFL);
	flags |= O_NONBLOCK;

	if(fcntl(serverSocket, F_SETFL, flags) == -1)
	{
		cout<<"setSocketNonBlocking() error"<<endl;
		return -1;
	}
	return 0;
}

int EpollUdpServer::createEpoll(int size)
{
	/* set epoll */
	fd_epoll = epoll_create(EPOLL_SIZE);

	if(fd_epoll == -1)
	{
		cout<<"epoll_create() error"<<endl;
		close(serverSocket);
		return -1;
	}
	return 0;
}

int EpollUdpServer::setEpoll(uint32_t flags)
{
	events.events = flags;
	events.data.fd = serverSocket;

	if(epoll_ctl(fd_epoll, EPOLL_CTL_ADD, serverSocket, &events) == -1)
	{
		cout<<"epoll_ctl() error"<<endl;
		close(serverSocket);
		close(fd_epoll);
		return -1;
	}

	return 1;
}
int EpollUdpServer::waitEvent()
{
	cout<<"epoll_wait"<<endl;
	return epoll_wait(fd_epoll, epollEvents, EPOLL_SIZE, -1);
}
int EpollUdpServer::bind()
{
	return this->UdpServer::bind(serverSocket, serverAddr);
}




