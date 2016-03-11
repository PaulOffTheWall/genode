#pragma once

#include <base/printf.h>
#include <base/rpc_client.h>
#include <dom0/task_manager_session.h>

struct TaskManagerSessionClient : Genode::Rpc_client<TaskManagerSession>
{
	TaskManagerSessionClient(Genode::Capability<TaskManagerSession> cap) :
		Genode::Rpc_client<TaskManagerSession>(cap) { }

	void addTasks(Genode::Ram_dataspace_capability xmlDsCap)
	{
		call<Rpc_add_tasks>(xmlDsCap);
	}

	void clearTasks()
	{
		call<Rpc_clear_tasks>();
	}

	Genode::Ram_dataspace_capability binaryDs(Genode::Ram_dataspace_capability nameDsCap, size_t size)
	{
		return call<Rpc_binary_ds>(nameDsCap, size);
	}

	void start()
	{
		call<Rpc_start>();
	}

	void stop()
	{
		call<Rpc_stop>();
	}

	Genode::Ram_dataspace_capability profileData()
	{
		return call<Rpc_profile_data>();
	}
};
