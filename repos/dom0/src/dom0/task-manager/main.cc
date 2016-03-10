#include <base/printf.h>
#include <cap_session/connection.h>
#include <base/rpc_server.h>
#include <base/sleep.h>
#include "task_manager_session_component.h"

int main(int argc, char* argv[])
{
	PDBG("task-manager: Hello!\n");

	// Server setup. See hello_tutorial for documentation.
	Genode::Cap_connection cap;
	static Genode::Sliced_heap slicedHeap(Genode::env()->ram_session(), Genode::env()->rm_session());

	enum { STACK_SIZE = 4096 };
	static Genode::Rpc_entrypoint ep(&cap, STACK_SIZE, "task-manager-ep");

	static TaskManagerRootComponent tmRoot(&ep, &slicedHeap);
	Genode::env()->parent()->announce(ep.manage(&tmRoot));

	Genode::sleep_forever();

	return 0;
}
