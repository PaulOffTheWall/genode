#pragma once

#include "tcp_socket.h"
#include <dom0/task_manager_connection.h>

class Dom0_server : public Tcp_socket
{
public:
	Dom0_server();

	~Dom0_server();

	int connect();

	void serve();

	void disconnect();

private:
	int _listenSocket;
	struct sockaddr_in _inAddr;
	sockaddr _targetAddr;
	Task_manager_connection _taskManager;
};
