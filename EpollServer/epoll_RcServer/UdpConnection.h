/*
 * UdpConnection.h
 *
 *  Created on: 2020. 2. 25.
 *      Author: csm
 */

#ifndef UDPCONNECTION_H_
#define UDPCONNECTION_H_

#include <src/Connection.h>

class UdpConnection : public Connection{
public:
	UdpConnection();
	virtual ~UdpConnection();
	int createSocket(const int domain, const int protocol);
	void initAddr(struct sockaddr_in& addr, const uint16_t port);
};

#endif /* UDPCONNECTION_H_ */
