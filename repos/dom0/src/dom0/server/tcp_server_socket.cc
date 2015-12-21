#include "tcp_server_socket.h"

#include <base/printf.h>


TcpServerSocket::TcpServerSocket(ip_addr_t address, int port)
{
	ownSocket = 0;
	targetSocket = 0;
	connected = false;
	targetSockaddr_inSize = sizeof(targetSockaddr_in);
	ownSockaddr_in.sin_family = AF_INET;
	ownSockaddr_in.sin_port = htons(port);
	ownSockaddr_in.sin_addr.s_addr = address.addr;
	int error;
	ownSocket = lwip_socket(PF_INET, SOCK_STREAM, 0);
	error = lwip_bind(ownSocket, (struct sockaddr*) (&ownSockaddr_in),
			sizeof(ownSockaddr_in));
	PINF("Server: bound to addr: %d\n", error);
	error = lwip_listen(ownSocket, 10);
	PINF("Server: listen(): %d\n", error);
}

TcpServerSocket::~TcpServerSocket()
{
	disconnect();
}

int TcpServerSocket::connect()
{
	targetSocket = lwip_accept(ownSocket, (struct sockaddr*) (&targetSockaddr_in), &targetSockaddr_inSize);
	PINF("Got connection from %d.%d.%d.%d\n", ip4_addr1(&targetSockaddr_in.sin_addr), ip4_addr2(&targetSockaddr_in.sin_addr), ip4_addr3(&targetSockaddr_in.sin_addr), ip4_addr4(&targetSockaddr_in.sin_addr));
	connected = true;
	return targetSocket == -1 ? -errno : targetSocket;
}

int TcpServerSocket::disconnect()
{
	lwip_close(targetSocket);
	PERR("Target socket closed.\n");
	lwip_close(ownSocket);
	PERR("Server socket closed.\n");
	connected = false;
	return 1;
}
