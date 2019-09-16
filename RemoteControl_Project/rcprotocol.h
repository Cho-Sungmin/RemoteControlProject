#pragma once
#pragma comment(lib, "ws2_32.lib")

#include <iostream>
#include "DataQueue.h"
#include "winsock2.h"
#define DESIRED_WINSOCK_VERSION        0x0101
#define MINIMUM_WINSOCK_VERSION        0x0001

#define SERVER_IP "192.168.0.13"		////로컬
#define RELAY_PORT 3333
#define UDP_PORT 4444
#define itoa(x) #x

#define ON_MOUSEHOOK 9000
#define ON_KBHOOK 9001

using std::cout;
using std::endl;

class Log {
private :
	int mode;						//// P2P & Relay
	char id[10];					
	char pw[10];
	SOCKADDR_IN private_addr;		////내부 ip
	SOCKADDR_IN public_addr;		////공인 ip
public :
	Log()
	{
		mode = 0;
		ZeroMemory(id, 10);
		ZeroMemory(pw, 10);
		ZeroMemory(&private_addr, sizeof(SOCKADDR_IN));
		ZeroMemory(&public_addr, sizeof(SOCKADDR_IN)); 
	}
	int getMode()
	{
		return mode;
	}
	void setMode(int n)
	{
		mode = n;
	}
	void setID(char* ID) {
		strcpy(id, ID);
	}
	char* getID() {
		return id;
	}
	void setPW(char* PW) {
		strcpy(pw, PW);
	}
	char* getPW() {
		return pw;
	}
	SOCKADDR_IN getPrvAddr() {
		return private_addr;
	}
	SOCKADDR_IN getPubAddr() {
		return public_addr;
	}
	void setPubAddr(SOCKADDR_IN addr) {
		public_addr = addr;
	}
	void setPrvAddr(SOCKADDR_IN addr) {
		private_addr = addr;
	}
	/*Log& operator=(const Log& log)
	{
		strcpy(id, log.id);
		strcpy(pw, log.pw);
		private_addr = log.private_addr;
		public_addr = log.public_addr;

		return *this;
	}*/

	bool operator==(const Log& log)
	{
		if (strncmp(id, log.id, strlen(id)) == 0)
			if(strncmp(pw, log.pw, strlen(pw)) == 0)
				return true;
			else
				return false;
		else
			return false;
	}
	bool operator!=(const Log& log)
	{
		if (strncmp(id, log.id, strlen(id)) == 0 && strncmp(pw, log.pw, strlen(pw)) == 0)
			return false;
		else
			return true;
	}

};

/////parameter for threads/////
typedef struct {
	SOCKADDR_IN addr;
	SOCKET sock;
}RC_Param;

#define DISCONNECT -1

#define IO_TYPE_SEND 0
#define IO_TYPE_RECV 1

#define PACKET_TYPE_CON_CUST 1
#define PACKET_TYPE_CON_HOST 2
#define PACKET_TYPE_SEND_IMG 3
#define PACKET_TYPE_SEND_MP 4
#define PACKET_TYPE_SEND_KB 5
#define CON_FAILED 0
#define CON_SUCCESS 1
#define MODE_P2P 1
#define MODE_RELAY 0
#define MAX_USER 3

/////Infomation about users//////
class Customer{
private:
	Log cust;
	Log* host;

public:
	Customer() {
		ZeroMemory(&cust, sizeof(cust));
		host = NULL;
	}
	Customer(Log user) {
		cust = user;
		host = NULL;
	}

	void addCust(Log user) {
		memcpy(&cust, &user, sizeof(cust));
	}
	Log getCust()
	{
		return cust;
	}
	void setHost(Log* user)
	{
		host = user;
	}
	Log* getHost() {
		return host;
	}
	bool checkAuthority(Log log)
	{
		if (cust == log)
				return true;
		else
			return false;
	}
};

/////packet header/////
typedef struct {
	int uId;
	int type;
}PACKET_INFO;

/////iocp/////
typedef struct {
	SOCKET hSocket;
	SOCKADDR_IN hAddr;
}SOCKET_CONTEXT;

typedef struct {
	WSAOVERLAPPED overlapped;
	BOOL type;
	PACKET_INFO head;
	Image_Packet buf;
	WSABUF wsaBuf[2];
}IO_CONTEXT;

class IOCP_IO{
public:

	void IOCP_RecvFrom(SOCKET_CONTEXT* pSocket)
	{
		int nError;
		int len= sizeof(SOCKADDR);

		DWORD dwRecv, dwFlags = 0;

		IO_CONTEXT* pIO = new IO_CONTEXT;
		ZeroMemory(pIO, sizeof(IO_CONTEXT));

		pIO->wsaBuf[0].buf = (char*)&pIO->head;
		pIO->wsaBuf[0].len = sizeof(pIO->head);
		pIO->wsaBuf[1].buf = (char*)&pIO->buf;
		pIO->wsaBuf[1].len = sizeof(pIO->buf);

		pIO->type = IO_TYPE_RECV;

		nError = WSARecvFrom(pSocket->hSocket, pIO->wsaBuf, 2, &dwRecv, &dwFlags, (SOCKADDR*)&pSocket->hAddr, &len, &pIO->overlapped, NULL);
		if (nError == 0)
		{
			//// Sent all
		}
		else if (nError == SOCKET_ERROR && WSAGetLastError() == WSA_IO_PENDING)
		{
			//// request succeed
		}
		else
			delete pIO;
	}
	void IOCP_SendTo(SOCKET_CONTEXT* pSocket, WSABUF* wsaBuf)
	{
		int nError;
		int len = sizeof(SOCKADDR_IN);

		DWORD dwSend, dwFlags = 0;

		IO_CONTEXT* pIO = new IO_CONTEXT;
		ZeroMemory(pIO, sizeof(IO_CONTEXT));
		pIO->type = IO_TYPE_SEND;

		memcpy(&pIO->wsaBuf[0], &wsaBuf[0], sizeof(wsaBuf[0]));
		memcpy(&pIO->wsaBuf[1], &wsaBuf[1], sizeof(wsaBuf[1]));

		nError = WSASendTo(pSocket->hSocket, pIO->wsaBuf, 2, &dwSend, dwFlags, (SOCKADDR*)&pSocket->hAddr, len, &pIO->overlapped, NULL);
		if (nError == 0)
		{
			//// Sent all
		}
		else if (nError == SOCKET_ERROR && WSAGetLastError() == WSA_IO_PENDING)
		{
			//// request succeed 
		}
		else
		{
			delete pIO;
		}

	}
};

///// Get ip address, not loopback /////
static IN_ADDR GetInIpAddress()
{
	WSADATA wsadata;
	IN_ADDR ip;


	if (!WSAStartup(DESIRED_WINSOCK_VERSION, &wsadata))
	{
		if (wsadata.wVersion >= MINIMUM_WINSOCK_VERSION)
		{
			HOSTENT *p_host_info;
			IN_ADDR in;
			char host_name[128] = { 0, };

			gethostname(host_name, 128);
			p_host_info = gethostbyname(host_name);

			if (p_host_info != NULL)
			{
				for (int i = 0; p_host_info->h_addr_list[i]; i++)
				{
					memcpy(&in, p_host_info->h_addr_list[i], sizeof(in));
					ip = in;
				}
			}
		}
		WSACleanup();
	}

	return ip;
}


static IN_ADDR GetIpAddress()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	char name[255];
	HOSTENT* p_host_info;
	IN_ADDR ip;
	wVersionRequested = MAKEWORD(2, 0);

	if (WSAStartup(wVersionRequested, &wsaData) == 0)
	{
		if (gethostname(name, sizeof(name)) == 0)
		{
			if ((p_host_info = gethostbyname(name)) != NULL)
				memcpy(&ip, p_host_info->h_addr_list, sizeof(ip));
			
		}
		WSACleanup();
	}
	return ip;
}