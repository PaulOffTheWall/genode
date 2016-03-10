#pragma once

#include <session/session.h>
#include <base/rpc.h>
#include <ram_session/ram_session.h>

struct TaskManagerSession : Genode::Session
{
	static const char *service_name() { return "TaskManager"; }

	virtual void addTasks(Genode::Ram_dataspace_capability xml) = 0;


	/*******************
	 ** RPC interface **
	 *******************/
	GENODE_RPC(Rpc_add_tasks, void, addTasks, Genode::Ram_dataspace_capability);

	GENODE_RPC_INTERFACE(Rpc_add_tasks);
};
