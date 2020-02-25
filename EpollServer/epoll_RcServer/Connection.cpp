/*
 * Connection.cpp
 *
 *  Created on: 2020. 2. 25.
 *      Author: csm
 */

#include <src/Connection.h>

using namespace std;

Connection::Connection() {
	// TODO Auto-generated constructor stub

}

Connection::~Connection() {
	// TODO Auto-generated destructor stub
}

int Connection::createSocket(const int domain, const int type, const int protocol)
{
	int sock = socket(domain, type, protocol);
		if(sock == -1)
		{
			cout<<"socket() error"<<endl;
			return -1;
		}else
			return sock;
}
int Connection::bind(const int socket, const struct sockaddr_in addr)
{
	if(::bind(socket, (struct sockaddr*)&addr, sizeof(addr)) == -1)
	{
		cout<<"bind() error"<<endl;
		close(socket);
		return -1;
	}
	return 0;
}
void Connection::initAddr(struct sockaddr_in& addr, const uint16_t port, const uint32_t ip)
{
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(ip);
}

