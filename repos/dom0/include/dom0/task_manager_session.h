#pragma once

#include <session/session.h>
#include <base/rpc.h>
#include <ram_session/ram_session.h>
#include <string>

struct TaskManagerSession : Genode::Session
{
	static const char *service_name() { return "task-manager"; }

	virtual void addTasks(Genode::Ram_dataspace_capability xmlDsCap) = 0;
	virtual Genode::Ram_dataspace_capability binaryDs(Genode::Ram_dataspace_capability nameDsCap, size_t size) = 0;
	virtual void start() = 0;

	/*******************
	 ** RPC interface **
	 *******************/
	GENODE_RPC(Rpc_add_tasks, void, addTasks, Genode::Ram_dataspace_capability);
	GENODE_RPC(Rpc_binary_ds, Genode::Ram_dataspace_capability, binaryDs, Genode::Ram_dataspace_capability, size_t);
	GENODE_RPC(Rpc_start, void, start);

	GENODE_RPC_INTERFACE(Rpc_add_tasks, Rpc_binary_ds, Rpc_start);
};
