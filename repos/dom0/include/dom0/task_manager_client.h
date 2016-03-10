#pragma once

#include <dom0/task_manager_session.h>
#include <base/rpc_client.h>
#include <base/printf.h>

struct TaskManagerSessionClient : Genode::Rpc_client<TaskManagerSession>
{
	TaskManagerSessionClient(Genode::Capability<TaskManagerSession> cap)
	: Genode::Rpc_client<TaskManagerSession>(cap) { }

	void addTasks(Ram_dataspace_capability xmlDs)
	{
		call<Rpc_add_tasks>(xmlDs);
	}
};
