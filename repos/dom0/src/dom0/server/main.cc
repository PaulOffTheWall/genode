#include <base/printf.h>
#include <timer_session/connection.h>
#include <base/signal.h>
#include "dom0_server.h"

int main(int argc, char* argv[])
{
	PDBG("dom0: Hello!\n");

	Dom0Server server;

	while (true)
	{
		server.connect();
		server.serve();
	}
}
