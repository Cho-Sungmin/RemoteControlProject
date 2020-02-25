/*
 * UdpConnection.cpp
 *
 *  Created on: 2020. 2. 25.
 *      Author: csm
 */

#include <src/UdpConnection.h>

UdpConnection::UdpConnection() {
	// TODO Auto-generated constructor stub

}

UdpConnection::~UdpConnection() {
	// TODO Auto-generated destructor stub
}
void UdpConnection::initAddr(struct sockaddr_in& addr, const uint16_t port)
{
	Connection::initAddr(addr, port, INADDR_ANY);
}
int UdpConnection::createSocket(const int domain, const int protocol)
{
	return Connection::createSocket(domain, SOCK_DGRAM, protocol);
}
