/*
 * UdpServer.h
 *
 *  Created on: 2020. 2. 25.
 *      Author: csm
 */

#ifndef UDPSERVER_H_
#define UDPSERVER_H_

#include <src/UdpConnection.h>

class UdpServer : protected UdpConnection{
public:
	UdpServer();
	virtual ~UdpServer();

	int serverSocket;
	struct sockaddr_in serverAddr;

};

#endif /* UDPSERVER_H_ */
