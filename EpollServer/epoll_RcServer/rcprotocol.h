
#pragma once

#include <iostream>
#include <arpa/inet.h>
#include <string.h>
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

#define UINT unsigned int
#define SOCKET int
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
		memset(data, 0, sizeof(data));
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
	struct sockaddr_in private_addr;		// private ip
	struct sockaddr_in public_addr;		// public ip
public :
	Log()
	{
		mode = -1;
		memset(id, 0, ID_LENGTH);
		memset(pw, 0, PW_LENGTH);
		memset(&private_addr, 0, sizeof(struct sockaddr_in));
		memset(&public_addr, 0, sizeof(struct sockaddr_in));
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
	const struct sockaddr_in getPrvAddr() const {
		return private_addr;
	}
	const struct sockaddr_in getPubAddr() const{
		return public_addr;
	}
	void setPubAddr(const struct sockaddr_in& addr) {
		public_addr = addr;
	}
	void setPrvAddr(const struct sockaddr_in& addr) {
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
	struct sockaddr_in addr;
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
