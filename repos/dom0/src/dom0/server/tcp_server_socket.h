#pragma once

#include "tcp_socket.h"


class TcpServerSocket: public TcpSocket
{
public:
	TcpServerSocket(const char* const address, int port);

	~TcpServerSocket();

	int connect();

	int disconnect();

private:
	int ownSocket;
	struct sockaddr_in in_addr;
	sockaddr target_addr;
};
