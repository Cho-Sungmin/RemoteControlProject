/*
 * Connection.h
 *
 *  Created on: 2020. 2. 25.
 *      Author: csm
 */
#ifndef CONNECTION_H_
#define CONNECTION_H_

#include <sys/socket.h>
#include <iostream>
#include <src/rcprotocol.h>
#include <sys/unistd.h>

class Connection {
public:
	Connection();
	virtual ~Connection();
	int bind(int socket, const struct sockaddr_in addr);
	void initAddr(struct sockaddr_in& addr, const uint16_t port, const uint32_t ip);
protected:
	int createSocket(const int domain, const int type, const int protocol);
};

#endif /* CONNECTION_H_ */
