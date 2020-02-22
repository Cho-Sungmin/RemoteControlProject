#pragma once
#include "atltypes.h"
#define QSIZE 500

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
	bool enqueue(const data& val);
	bool dequeue(data *data);
	const data& peek();
	bool isFull();
	bool isEmpty();

};

template <typename data>
DataQueue<data>::DataQueue()
{
	rear = front = 0;
}

template <typename data>
bool DataQueue<data>::enqueue(const data& val)
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
const data& DataQueue<data>::peek()
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
