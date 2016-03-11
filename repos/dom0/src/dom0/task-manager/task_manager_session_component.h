#pragma once

#include <list>
#include <unordered_map>

#include <base/signal.h>
#include <dom0/task_manager_session.h>
#include <launchpad/launchpad.h>
#include <os/attached_ram_dataspace.h>
#include <os/server.h>
#include <root/component.h>
#include <util/string.h>
#include <trace_session/connection.h>

#include "task.h"

struct TaskManagerSessionComponent : Genode::Rpc_object<TaskManagerSession>
{
public:
	TaskManagerSessionComponent(Server::Entrypoint& ep);
	virtual ~TaskManagerSessionComponent();

	void addTasks(Genode::Ram_dataspace_capability xmlDsCap);
	Genode::Ram_dataspace_capability binaryDs(Genode::Ram_dataspace_capability nameDsCap, size_t size);
	void clearTasks();
	void start();
	void stop();
	Genode::Ram_dataspace_capability profileData();

protected:
	std::unordered_map<std::string, Genode::Attached_ram_dataspace> _binaries;

	// List instead of vector because reallocation would invalidate dataspaces.
	std::list<Task> _tasks;

	Launchpad _launchpad;
	Server::Entrypoint& _ep;
	Genode::Trace::Connection _trace;
	Genode::Attached_ram_dataspace _profileData;

	void _updateProfileData();

	static Genode::Number_of_bytes _launchpadQuota();
	static Genode::Number_of_bytes _traceQuota();
	static Genode::Number_of_bytes _traceBufSize();
	static Genode::Number_of_bytes _profileDsSize();
};

struct TaskManagerRootComponent : Genode::Root_component<TaskManagerSessionComponent>
{
public:
	TaskManagerRootComponent(Server::Entrypoint* ep, Genode::Allocator *allocator) :
		Genode::Root_component<TaskManagerSessionComponent>(&ep->rpc_ep(), allocator),
		_ep(*ep)
	{
		PDBG("Creating root component.");
	}

protected:
	Server::Entrypoint& _ep;

	TaskManagerSessionComponent* _create_session(const char *args)
	{
		PDBG("Creating Task Manager session.");
		return new (md_alloc()) TaskManagerSessionComponent(_ep);
	}
};
