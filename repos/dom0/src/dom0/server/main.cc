#include <base/printf.h>
#include <base/exception.h>
#include <os/config.h>

#include <string>

#include <cstring>
#include <cstdint>

#include "tcp_server_socket.h"
#include "communication_magic_numbers.h"

#include <nic/packet_allocator.h>

#include <lwip/genode.h>

struct Config
{
	char listenAddress[16];
	unsigned int port;
};

void serve(TcpServerSocket& myServer)
{
}

Config loadConfig()
{
	Config config;

	const Genode::Xml_node& configNode = Genode::config()->xml_node();
	std::strcpy(config.listenAddress, "0.0.0.0");
	try 
	{
		configNode.sub_node("listen-address").value(config.listenAddress, 16);
		configNode.sub_node("port").value(&config.port);
	}
	catch (const Genode::Exception& err)
	{
	}
	PINF("Config readouts:\n");
	PINF("\tListening address: %s\n", config.listenAddress);
	PINF("\tPort: %d\n", config.port);

	return config;
}

int runServer(const Config& config)
{
	const int BUF_SIZE = Nic::Packet_allocator::DEFAULT_PACKET_SIZE * 128;
	lwip_tcpip_init();

	/* Initialize network stack and do DHCP */
	if (lwip_nic_init(0, 0, 0, BUF_SIZE, BUF_SIZE)) {
		PERR("We got no IP address!");
		return 1;
	}
	ip_addr_t addr = {0};
	inet_aton(config.listenAddress, &addr);

	// PDBG("%d.%d.%d.%d\n", ip4_addr1(&addr.addr), ip4_addr2(&addr.addr), ip4_addr3(&addr.addr), ip4_addr4(&addr.addr));
	TcpServerSocket server(addr, config.port);

	while (true)
	{
		server.connect();
	}

	return 0;
}

int main(int argc, char* argv[])
{
	PDBG("dom0: Hello!\n");

	Config config = loadConfig();

	return runServer(config);
}
