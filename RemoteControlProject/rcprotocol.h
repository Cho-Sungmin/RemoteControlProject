#pragma once
#pragma comment(lib, "ws2_32.lib")

#include <iostream>
#include "winsock2.h"
#define DESIRED_WINSOCK_VERSION        0x0101
#define MINIMUM_WINSOCK_VERSION        0x0001

#define ID_LENGTH 10
#define PW_LENGTH 10

#define SERVER_IP "192.168.0.13"		////use your server ip
#define RELAY_PORT 3333
#define UDP_PORT 4444
#define itoa(x) #x
#define MTU 1024
#define BIT_RATE 100

#define ON_DISPLAY_IMG WM_USER+2
#define ON_THREAD WM_USER + 3
#define ON_SCREEN WM_USER + 4
#define ON_SEND_ACK WM_USER + 5
#define ON_MOUSEHOOK 9000
#define ON_KBHOOK 9001

using std::cout;
using std::endl;

typedef struct MPoint
{
	double x;
	double y;

	MPoint() :x(0), y(0)
	{}

	MPoint(double _x, double _y)
	{
		x = _x;
		y = _y;
	}
} MPoint;

typedef struct Mouse_Point : MPoint
{
public:
	UINT msg;

	Mouse_Point() :msg(0)
	{}

	Mouse_Point(double _x, double _y, UINT _msg) : MPoint(_x, _y)
	{
		msg = _msg;
	}

	Mouse_Point& operator=(Mouse_Point& mp)
	{
		msg = mp.msg;

		return *this;
	}
}Mouse_Point;

typedef struct Image_Packet {

public:
	int seq;
	int flag;
	int size;
	char data[MTU];

	Image_Packet()
	{
		seq = 0;
		flag = true;
		size = 0;
		ZeroMemory(data, sizeof(data));
	}

	Image_Packet& operator=(const Image_Packet& p)
	{
		seq = p.seq;
		flag = p.flag;
		size = p.size;
		memcpy(data, p.data, sizeof(p.data));
		return *this;
	}
}Image_Packet;

class Log {
private :
	int mode;						// P2P or Relay
	char id[ID_LENGTH];
	char pw[PW_LENGTH];
	SOCKADDR_IN private_addr;		// private ip
	SOCKADDR_IN public_addr;		// public ip
public :
	Log()
	{
		mode = -1;
		ZeroMemory(id, ID_LENGTH);
		ZeroMemory(pw, PW_LENGTH);
		ZeroMemory(&private_addr, sizeof(SOCKADDR_IN));
		ZeroMemory(&public_addr, sizeof(SOCKADDR_IN)); 
	}
	const int getMode() const
	{
		return mode;
	}
	void setMode(const int& n)
	{
		mode = n;
	}
	void setID(const char* ID) {
		strcpy(id, ID);
	}
	const char* getID() const{
		return id;
	}
	void setPW(const char* PW) {
		strcpy(pw, PW);
	}
	const char* getPW() const{
		return pw;
	}
	const SOCKADDR_IN getPrvAddr() const {
		return private_addr;
	}
	const SOCKADDR_IN getPubAddr() const{
		return public_addr;
	}
	void setPubAddr(const SOCKADDR_IN& addr) {
		public_addr = addr;
	}
	void setPrvAddr(const SOCKADDR_IN& addr) {
		private_addr = addr;
	}
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
#define PACKET_TYPE_SEND_ACK 6	// for flow control
#define CON_FAILED 0
#define CON_SUCCESS 1
#define MODE_P2P 1
#define MODE_RELAY 0

/////packet header/////
typedef struct PACKET_INFO {
	int uId;
	int type;

	PACKET_INFO()
	{
		uId = -1;
		type = 0;
	}
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
	void IOCP_SendTo(const SOCKET_CONTEXT* pSocket, const WSABUF* wsaBuf)
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