#pragma once

#include <dom0/client.h>
#include <base/connection.h>

struct TaskManagerConnection : Genode::Connection<TaskManagerSession>, TaskManagerSessionClient
{
	TaskManagerConnection()
	:
		/* create session */
		Genode::TaskManagerConnection<TaskManagerSession>(session("foo, ram_quota=4K")),

		/* initialize RPC interface */
		TaskManagerSessionClient(cap())
	{
	}
};
