#pragma once

#include <dom0/task_manager_client.h>
#include <base/connection.h>

struct TaskManagerConnection : Genode::Connection<TaskManagerSession>, TaskManagerSessionClient
{
	TaskManagerConnection() :
		/* create session */
		Genode::Connection<TaskManagerSession>(session("foo, ram_quota=4K")),

		/* initialize RPC interface */
		TaskManagerSessionClient(cap())
	{
	}
};
