#include "dom0_server.h"

#include <cstring>

#include <base/printf.h>
#include <lwip/genode.h>

#include "config.h"
#include "communication_magic_numbers.h"

Dom0Server::Dom0Server() :
	TcpSocket(),
	_listenSocket(0),
	in_addr{0},
	target_addr{0},
	_taskMngr()
{
	lwip_tcpip_init();

	const Config& config = Config::get();

	in_addr.sin_family = AF_INET;
	in_addr.sin_port = htons(config.port);
	if (std::strcmp(config.dhcp, "yes") == 0)
	{
		if (lwip_nic_init(0, 0, 0, config.bufSize, config.bufSize)) {
			PERR("We got no IP address!\n");
			return;
		}
		in_addr.sin_addr.s_addr = INADDR_ANY;
	}
	else
	{
		if (lwip_nic_init(inet_addr(config.listenAddress), inet_addr(config.networkMask), inet_addr(config.networkGateway), config.bufSize, config.bufSize)) {
			PERR("We got no IP address!\n");
			return;
		}
		in_addr.sin_addr.s_addr = inet_addr(config.listenAddress);
	}

	if ((_listenSocket = lwip_socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		PERR("no socket available!");
		return;
	}
	if (lwip_bind(_listenSocket, (struct sockaddr*)&in_addr, sizeof(in_addr)))
	{
		PERR("bind failed!");
		return;
	}
	if (lwip_listen(_listenSocket, 5))
	{
		PERR("listen failed!");
		return;
	}
	PINF("Listening...\n");
}

Dom0Server::~Dom0Server()
{
	disconnect();
}

int Dom0Server::connect()
{
	socklen_t len = sizeof(target_addr);
	targetSocket = lwip_accept(_listenSocket, &target_addr, &len);
	if (targetSocket < 0)
	{
		PWRN("Invalid socket from accept!");
		return targetSocket;
	}
	connected = true;
	sockaddr_in* target_in_addr = (sockaddr_in*)&target_addr;
	PINF("Got connection from %s\n", inet_ntoa(target_in_addr));
	return targetSocket;
}

void Dom0Server::serve()
{
	int message = 0;
	while (true)
	{
		NETCHECK_LOOP(receiveInt32_t(message));
		if (message == TASK_DESC)
		{
			PINF("Ready to receive task description.\n");

			// Get JSON size.
			int jsonSize;
			NETCHECK_LOOP(receiveInt32_t(jsonSize));
			std::vector<char> json(jsonSize);
			PINF("Ready to receive json of size %d.\n", jsonSize);

			// Get JSON file.
			NETCHECK_LOOP(receiveData(json.data(), json.size()));
			PINF("Received JSON %s", json.data());
			_taskMngr.clearTasks();
			_taskMngr.addTasks(json.data());
		}
		else if (message == SEND_BINARIES)
		{
			PINF("Ready to receive binaries.\n");

			// Get number of binaries to receive.
			int numBinaries = 0;
			NETCHECK_LOOP(receiveInt32_t(numBinaries));
			PINF("%d binar%s to be sent.\n", numBinaries, numBinaries == 1 ? "y" : "ies");

			// Receive binaries.
			for (int i = 0; i < numBinaries; i++)
			{
				// Get binary name.
				PINF("Waiting for binary %d\n", i);
				char name[9];
				NETCHECK_LOOP(receiveData(name, 8));
				name[8]='\0';
				std::string binaryName(name);

				// Get binary size.
				int32_t binarySize = 0;
				NETCHECK_LOOP(receiveInt32_t(binarySize));

				// Get binary data.
				NETCHECK_LOOP(receiveData(_taskMngr.getBinarySpace(binaryName, binarySize), binarySize));
				PINF("Got binary '%s' of size %d.\n", name, binarySize);

				// Client is waiting for ready signal.
				NETCHECK_LOOP(sendInt32_t(GO_SEND));
			}
		}
		else if (message == START)
		{
			_taskMngr.start();
		}
		else
		{
			PWRN("Unknown message: %d\n", message);
		}
	}
}

void Dom0Server::disconnect()
{
	lwip_close(targetSocket);
	PERR("Target socket closed.\n");
	lwip_close(_listenSocket);
	PERR("Server socket closed.\n");
	connected = false;
}
