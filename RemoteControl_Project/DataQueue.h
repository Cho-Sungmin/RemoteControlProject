#pragma once
#include "atltypes.h"
#define QSIZE 300
#define MTU 576

typedef struct {
	double x;
	double y;
} MPoint;

class Mouse_Point {
public:
	MPoint point;
	UINT msg;

	Mouse_Point& operator=(Mouse_Point& mp)
	{
		point = mp.point;
		msg = mp.msg;

		return *this;
	}
};

class Image_Packet {
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
};

template <typename data>
class DataQueue
{
private:
	data queue[QSIZE];
	int front;
	int rear;
public:
	DataQueue();
	~DataQueue();
	bool enqueue(data val);
	bool dequeue(data *data);
	data peek();
	bool isFull();
	bool isEmpty();

};

template <typename data>
DataQueue<data>::DataQueue()
{
	rear = front = 0;
}

template <typename data>
bool DataQueue<data>::enqueue(data val)
{
	if (!isFull())
	{
		rear = (rear + 1) % QSIZE;
		queue[rear] = val;
		return true;
	}
	else return false;
}

template <typename data>
bool DataQueue<data>::dequeue(data *data)
{
	if (!isEmpty())
	{
		front = (front + 1) % QSIZE;
		*data = queue[front];
		return true;
	}
	else
		return false;
}

template <typename data>
data DataQueue<data>::peek()
{
	if (!isEmpty())
	{
		return queue[front + 1];
	}
	else return queue[0];
}

template <typename data>
bool DataQueue<data>::isFull()
{
	if (front == (rear + 1) % QSIZE)
		return true;
	else return false;
}

template <typename data>
bool DataQueue<data>::isEmpty()
{
	if (front == rear)
		return true;
	else return false;
}

template <typename data>
DataQueue<data>::~DataQueue()
{
}
