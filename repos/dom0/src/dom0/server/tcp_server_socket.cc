#include "tcp_server_socket.h"

#include <base/printf.h>


TcpServerSocket::TcpServerSocket(const char* const address, int port)
{
	ownSocket = 0;
	targetSocket = 0;
	connected = false;
	in_addr.sin_family = AF_INET;
	in_addr.sin_port = htons(port);
	in_addr.sin_addr.s_addr = INADDR_ANY;
	if (address && *address)
	{
		in_addr.sin_addr.s_addr = inet_addr(address);
	}
	if ((ownSocket = lwip_socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		PERR("no socket available!");
		return;
	}
	if (lwip_bind(ownSocket, (struct sockaddr*)&in_addr, sizeof(in_addr)))
	{
		PERR("bind failed!");
		return;
	}
	if (lwip_listen(ownSocket, 5))
	{
		PERR("listen failed!");
		return;
	}
	PINF("Listening...\n");
}

TcpServerSocket::~TcpServerSocket()
{
	disconnect();
}

int TcpServerSocket::connect()
{
	socklen_t len = sizeof(target_addr);
	targetSocket = lwip_accept(ownSocket, &target_addr, &len);
	if (targetSocket < 0)
	{
		PWRN("Invalid socket from accept!");
		return targetSocket;
	}
	sockaddr_in* target_in_addr = (sockaddr_in*)&target_addr;
	PINF("Got connection from %s\n", inet_ntoa(target_in_addr));
	connected = true;
	return targetSocket;
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
