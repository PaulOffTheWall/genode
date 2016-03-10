#pragma once

#include "tcp_socket.h"
#include <dom0/task_manager_connection.h>

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
	struct sockaddr_in _inAddr;
	sockaddr _targetAddr;
	TaskManagerConnection _taskManager;
};
