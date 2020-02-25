/*
 * UdpServer.cpp
 *
 *  Created on: 2020. 2. 25.
 *      Author: csm
 */

#include <src/UdpServer.h>

UdpServer::UdpServer() {
	serverSocket = createSocket(PF_INET, 0);
	initAddr(serverAddr, RELAY_PORT);
}

UdpServer::~UdpServer() {
	// TODO Auto-generated destructor stub
}
