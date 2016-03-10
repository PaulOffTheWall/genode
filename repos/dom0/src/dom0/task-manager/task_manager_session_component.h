#pragma once

#include <list>
#include <unordered_map>
#include <os/attached_ram_dataspace.h>
#include <launchpad/launchpad.h>
#include <base/signal.h>
#include <util/string.h>
#include <root/component.h>
#include <dom0/task_manager_session.h>
#include "task.h"

class TaskManagerSessionComponent : public Genode::Rpc_object<TaskManagerSession>
{
public:
	TaskManagerSessionComponent();
	virtual ~TaskManagerSessionComponent();

	void addTasks(Genode::Ram_dataspace_capability xmlDsCap);
	Genode::Ram_dataspace_capability binaryDs(Genode::Ram_dataspace_capability nameDsCap, size_t size);
	void start();

protected:
	std::unordered_map<std::string, Genode::Attached_ram_dataspace> _binaries;

	// List instead of vector because reallocation would invalidate dataspaces.
	std::list<Task> _tasks;
	Launchpad _launchpad;
	Genode::Signal_receiver _sigRec;

	static Genode::Number_of_bytes _launchpadQuota();
};

class TaskManagerRootComponent : public Genode::Root_component<TaskManagerSessionComponent>
{
public:
	TaskManagerRootComponent(Genode::Rpc_entrypoint *ep, Genode::Allocator *allocator) :
		Genode::Root_component<TaskManagerSessionComponent>(ep, allocator)
	{
		PDBG("Creating root component.");
	}

protected:
	TaskManagerSessionComponent* _create_session(const char *args)
	{
		PDBG("Creating Task Manager session.");
		return new (md_alloc()) TaskManagerSessionComponent();
	}
};
