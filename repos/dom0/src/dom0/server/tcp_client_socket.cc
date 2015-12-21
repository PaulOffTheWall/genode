#include "tcp_client_socket.h"

#include <base/printf.h>


TcpClientSocket::TcpClientSocket(ip_addr_t address, int port)
{
	connected = false;
	targetSockaddr_in.sin_family = AF_INET;
	targetSockaddr_in.sin_port = htons(port);
	targetSockaddr_in.sin_addr.s_addr = address.addr;
}

TcpClientSocket::~TcpClientSocket()
{
	disconnect();
}

int TcpClientSocket::connect()
{
	targetSocket = lwip_socket(PF_INET, SOCK_STREAM, 0);
	PINF("socket created: %d\n", targetSocket);
	if (lwip_connect(targetSocket, (struct sockaddr*) &targetSockaddr_in,
			sizeof(targetSockaddr_in)) == 0)
	{
		PINF("connected\n");
		PINF("%s\n", strerror(errno));
		connected = true;
		return 1;
	}
	else
		return -errno;
}

int TcpClientSocket::disconnect()
{
	lwip_close(targetSocket);
	connected = false;
	return 1;
}
