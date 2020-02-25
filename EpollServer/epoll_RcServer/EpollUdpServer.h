#include <src/UdpServer.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <queue>
#include <map>


#define EPOLL_SIZE 100
#define CORE 4

using namespace std;

class EpollUdpServer : UdpServer
{
public:
	EpollUdpServer();
	virtual ~EpollUdpServer();
	EpollUdpServer(const EpollUdpServer& lvalue);
	int setSocketNonBlocking();
	int createEpoll(int size);
	int setEpoll(uint32_t flags);
	int waitEvent();
	int bind();

	struct epoll_event epollEvents[EPOLL_SIZE];
private:
	int fd_epoll;
	struct epoll_event events;
};
