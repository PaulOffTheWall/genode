#pragma once

#include "tcp_socket.h"
#include "task_manager.h"

class Dom0Server : public TcpSocket
{
public:
	Dom0Server();

	~Dom0Server();

	int connect();

	void serve();

	void disconnect();

private:
	int _listenSocket;
	struct sockaddr_in in_addr;
	sockaddr target_addr;
	TaskManager _taskMngr;
};
