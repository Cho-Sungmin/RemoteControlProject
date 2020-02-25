#include <src/EpollUdpServer.h>
#include <thread>

static map<int, pair<Log,Log>> st_userTable;			// worker threads get tasks in the queue

typedef struct worker_procedure
{
	struct epoll_event task;
	PACKET_INFO head;
	Image_Packet data;
	struct iovec bufFromClnt[2];
	struct msghdr msg;
	struct sockaddr_in desAddr;
	EpollUdpServer serverObject;

	worker_procedure(EpollUdpServer& param) : serverObject(param)
	{
	}

	void operator()(){

		bufFromClnt[0].iov_base = &head;
		bufFromClnt[0].iov_len = sizeof(PACKET_INFO);
		bufFromClnt[1].iov_base = &data;
		bufFromClnt[1].iov_len = sizeof(Image_Packet);

		msg.msg_name = &desAddr;
		msg.msg_namelen = sizeof(struct sockaddr_in);
		msg.msg_iovlen = 2;
		msg.msg_control = nullptr;
		msg.msg_controllen = 0;
		msg.msg_flags = 0;

		int numOfEvents = 0;

		while(true)
		{
			msg.msg_iov = bufFromClnt;
			numOfEvents = serverObject.waitEvent();

			for(int i=0; i<numOfEvents; i++)
			{
				task = serverObject.epollEvents[i];

				if(task.events == EPOLLIN)
				{
					int size = recvmsg(task.data.fd,  &msg, 0);

					cout<<"recv message"<<endl;
					cout<<"type : "<<head.type<<endl;

					if(size <= 0)
					{
						cout<< "recvmsg() error" <<endl;
					}

					if(head.type == PACKET_TYPE_SEND_IMG)
					{
						map<int, pair<Log,Log>>::iterator iter = st_userTable.find(head.uId);
						if(iter != st_userTable.end())
						{
							desAddr = iter->second.second.getPubAddr();
							sendmsg(task.data.fd, &msg, 0);
						}
					}
					else if(head.type == PACKET_TYPE_SEND_MP || head.type == PACKET_TYPE_SEND_KB
							 || head.type == PACKET_TYPE_SEND_ACK  )
					{
						map<int, pair<Log,Log>>::iterator iter = st_userTable.find(head.uId);
						if(iter != st_userTable.end())
						{
							desAddr = iter->second.first.getPubAddr();
							sendmsg(task.data.fd, &msg, 0);
							if(((Mouse_Point*)&data)->msg == DISCONNECT)
							{
								st_userTable.erase(iter);
							}
						}

					}
					else if (head.type == PACKET_TYPE_CON_CUST)	//Connection Request
					{
						Log customer;
						memcpy(&customer, bufFromClnt[1].iov_base, sizeof(Log));
						customer.setPubAddr((const struct sockaddr_in)desAddr);
						/* get private addr */
						struct sockaddr_in tmp = customer.getPrvAddr();
						tmp.sin_port = customer.getPubAddr().sin_port;
						customer.setPrvAddr(tmp);
						int key = std::stoi(customer.getID());
						pair<Log, Log> value = make_pair(customer, customer);
						st_userTable.insert(make_pair(key, value));		//customer register
					}
					else if (head.type == PACKET_TYPE_CON_HOST)
					{
						Log hostInfo;
						struct iovec bufToCustomer[2];
						struct iovec bufToHost[2];

						memcpy(&hostInfo, bufFromClnt[1].iov_base, sizeof(Log));

						int key = std::stoi(hostInfo.getID());
						std::map<int, pair<Log, Log>>::iterator iter = st_userTable.find(key);

						if (iter != st_userTable.end() && iter->second.first == hostInfo)
						{
							head.type = CON_SUCCESS;
							head.uId = key;

							/* get private addr */
							hostInfo.setPubAddr((const struct sockaddr_in)desAddr);
							struct sockaddr_in tmp = hostInfo.getPrvAddr();
							tmp.sin_port = hostInfo.getPubAddr().sin_port;
							hostInfo.setPrvAddr(tmp);

							iter->second.second = hostInfo;
							const Log& customerInfo = iter->second.first;

							/* set packet for both of clients */
							bufToCustomer[0].iov_base = (char*)&head;
							bufToCustomer[0].iov_len = sizeof(PACKET_INFO);
							bufToHost[0].iov_base = (char*)&head;
							bufToHost[0].iov_len = sizeof(PACKET_INFO);
							bufToCustomer[1].iov_base = (char*)&hostInfo;
							bufToCustomer[1].iov_len = sizeof(Log);
							bufToHost[1].iov_base = (char*)&customerInfo;
							bufToHost[1].iov_len = sizeof(Log);

							/* send clients information */
							desAddr = hostInfo.getPubAddr();
							msg.msg_iov = bufToHost;
							sendmsg(task.data.fd, &msg, 0);
							desAddr = customerInfo.getPubAddr();
							msg.msg_iov = bufToCustomer;
							sendmsg(task.data.fd, &msg, 0);

							if (customerInfo.getMode() == MODE_P2P)
							{
								st_userTable.erase(key);	//clean the table
							}
						}
						else //found customer waiting for the host
						{
							head.type = CON_FAILED;
							msg.msg_iov = bufToHost;
							sendmsg(task.data.fd, &msg, 0);
						}
					}
				}
			}
		}
	}
}worker_procedure;

int main()
{
	EpollUdpServer serverObject;

	/* init epoll */
	if(serverObject.setSocketNonBlocking() == -1)
		return 0;
	if(serverObject.bind() == -1)
		return 0;
	if(serverObject.createEpoll(EPOLL_SIZE) == -1)
		return 0;
	if(serverObject.setEpoll(EPOLLIN) == -1)
		return 0;

	/* create worker threads */
	thread* threads[CORE*2];
	worker_procedure procedure(serverObject);

	for(int i=0; i<CORE*2; i++){
		threads[i] = new thread(procedure);
		threads[i]->join();
	}

	return 0;
}
