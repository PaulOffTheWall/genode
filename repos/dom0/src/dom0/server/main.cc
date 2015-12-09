#include <base/printf.h>

#include <nic/packet_allocator.h>

extern "C"
{
#include <lwip/sockets.h>
#include <lwip/api.h>
}

#include <lwip/genode.h>

void serve(int client)
{

}

int main(int argc, char* argv[])
{
	PDBG("dom0: Hello!\n");

	const int BUF_SIZE = Nic::Packet_allocator::DEFAULT_PACKET_SIZE * 128;

	int socket;

	lwip_tcpip_init();

	/* Initialize network stack and do DHCP */
	if (lwip_nic_init(0, 0, 0, BUF_SIZE, BUF_SIZE)) {
		PERR("We got no IP address!");
		return -1;
	}

	PLOG("Create new socket ...");
	if ((socket = lwip_socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		PERR("No socket available!");
		return -1;
	}

	PLOG("Now, I will bind ...");
	struct sockaddr_in in_addr;
	in_addr.sin_family = AF_INET;
	in_addr.sin_port = htons(3001);
	in_addr.sin_addr.s_addr = INADDR_ANY;
	if (lwip_bind(socket, (struct sockaddr*)&in_addr, sizeof(in_addr))) {
		PERR("bind failed!");
		return -1;
	}

	PLOG("Now, I will listen ...");
	if (lwip_listen(socket, 5)) {
		PERR("listen failed!");
		return -1;
	}

	PLOG("Start the server loop ...");
	while (true) {
		struct sockaddr addr;
		socklen_t len = sizeof(addr);
		int client = lwip_accept(socket, &addr, &len);
		if (client < 0) {
			PWRN("Invalid socket from accept!");
			continue;
		}
		serve(client);
		lwip_close(client);
	}

	return 0;
}
