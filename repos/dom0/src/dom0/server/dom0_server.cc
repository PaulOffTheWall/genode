#include "dom0_server.h"

#include <cstring>
#include <vector>
#include <string>

#include <base/printf.h>
#include <lwip/genode.h>
#include <os/attached_ram_dataspace.h>

#include "config.h"
#include "communication_magic_numbers.h"

Dom0_server::Dom0_server() :
	_listenSocket(0),
	_inAddr{0},
	_targetAddr{0},
	_taskManager{}
{
	lwip_tcpip_init();

	const Config& config = Config::get();

	_inAddr.sin_family = AF_INET;
	_inAddr.sin_port = htons(config.port);
	if (std::strcmp(config.dhcp, "yes") == 0)
	{
		if (lwip_nic_init(0, 0, 0, config.bufSize, config.bufSize)) {
			PERR("We got no IP address!");
			return;
		}
		_inAddr.sin_addr.s_addr = INADDR_ANY;
	}
	else
	{
		if (lwip_nic_init(inet_addr(config.listenAddress), inet_addr(config.networkMask), inet_addr(config.networkGateway), config.bufSize, config.bufSize)) {
			PERR("We got no IP address!");
			return;
		}
		_inAddr.sin_addr.s_addr = inet_addr(config.listenAddress);
	}

	if ((_listenSocket = lwip_socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		PERR("No socket available!");
		return;
	}
	if (lwip_bind(_listenSocket, (struct sockaddr*)&_inAddr, sizeof(_inAddr)))
	{
		PERR("Bind failed!");
		return;
	}
	if (lwip_listen(_listenSocket, 5))
	{
		PERR("Listen failed!");
		return;
	}
	PINF("Listening...\n");
}

Dom0_server::~Dom0_server()
{
	disconnect();
}

int Dom0_server::connect()
{
	socklen_t len = sizeof(_targetAddr);
	_targetSocket = lwip_accept(_listenSocket, &_targetAddr, &len);
	if (_targetSocket < 0)
	{
		PWRN("Invalid socket from accept!");
		return _targetSocket;
	}
	sockaddr_in* target_in_addr = (sockaddr_in*)&_targetAddr;
	PINF("Got connection from %s", inet_ntoa(target_in_addr));
	return _targetSocket;
}

void Dom0_server::serve()
{
	int message = 0;
	while (true)
	{
		NETCHECK_LOOP(receiveInt32_t(message));
		if (message == SEND_DESCS)
		{
			PDBG("Ready to receive task description.");

			// Get XML size.
			int xmlSize;
			NETCHECK_LOOP(receiveInt32_t(xmlSize));
			Genode::Attached_ram_dataspace xmlDs(Genode::env()->ram_session(), xmlSize);
			PINF("Ready to receive XML of size %d.", xmlSize);

			// Get XML file.
			NETCHECK_LOOP(receiveData(xmlDs.local_addr<char>(), xmlSize));
			PDBG("Received XML. Initializing tasks.");
			_taskManager.addTasks(xmlDs.cap());
			PDBG("Done.");
		}
		else if (message == CLEAR)
		{
			PDBG("Clearing tasks.");
			_taskManager.clearTasks();
			PDBG("Done.");
		}
		else if (message == SEND_BINARIES)
		{
			PDBG("Ready to receive binaries.");

			// Get number of binaries to receive.
			int numBinaries = 0;
			NETCHECK_LOOP(receiveInt32_t(numBinaries));
			PINF("%d binar%s to be sent.", numBinaries, numBinaries == 1 ? "y" : "ies");

			// Receive binaries.
			for (int i = 0; i < numBinaries; i++)
			{
				// Client is waiting for ready signal.
				NETCHECK_LOOP(sendInt32_t(GO_SEND));

				// Get binary name.
				Genode::Attached_ram_dataspace nameDs(Genode::env()->ram_session(), 16);
				NETCHECK_LOOP(receiveData(nameDs.local_addr<char>(), 16));

				// Get binary size.
				int32_t binarySize = 0;
				NETCHECK_LOOP(receiveInt32_t(binarySize));

				// Get binary data.
				Genode::Dataspace_capability binDsCap = _taskManager.binaryDs(nameDs.cap(), binarySize);
				Genode::Rm_session* rm = Genode::env()->rm_session();
				char* bin = (char*)rm->attach(binDsCap);
				NETCHECK_LOOP(receiveData(bin, binarySize));

				PINF("Got binary '%s' of size %d.", nameDs.local_addr<char>(), binarySize);
				rm->detach(bin);
			}
			PDBG("Done.");
		}
		else if (message == START)
		{
			PDBG("Starting tasks.");
			_taskManager.start();
			PDBG("Done.");
		}
		else if (message == STOP)
		{
			PDBG("Stopping tasks.");
			_taskManager.stop();
			PDBG("Done.");
		}
		else if (message == GET_PROFILE)
		{
			Genode::Dataspace_capability xmlDsCap = _taskManager.profileData();
			Genode::Rm_session* rm = Genode::env()->rm_session();
			char* xml = (char*)rm->attach(xmlDsCap);

			size_t size = std::strlen(xml) + 1;
			PINF("Sending profile data of size %d", size);
			NETCHECK_LOOP(sendInt32_t(size));
			NETCHECK_LOOP(sendData(xml, size));

			rm->detach(xml);
			PDBG("Done.");
		}
		else
		{
			PWRN("Unknown message: %d", message);
		}
	}
}

void Dom0_server::disconnect()
{
	lwip_close(_targetSocket);
	PERR("Target socket closed.");
	lwip_close(_listenSocket);
	PERR("Server socket closed.");
}
