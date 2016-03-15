#pragma once

#include <list>
#include <unordered_map>

#include <base/signal.h>
#include <dom0/task_manager_session.h>
#include <os/attached_ram_dataspace.h>
#include <os/server.h>
#include <root/component.h>
#include <util/string.h>
#include <trace_session/connection.h>

#include "task.h"

struct Task_manager_session_component : Genode::Rpc_object<Task_manager_session>
{
public:
	Task_manager_session_component(Server::Entrypoint& ep);
	virtual ~Task_manager_session_component();

	// Create tasks in idle state from XML description.
	void add_tasks(Genode::Ram_dataspace_capability xml_ds_cap);

	// Allocate and return a capability of a new dataspace to be used for a task binary.
	Genode::Ram_dataspace_capability binary_ds(Genode::Ram_dataspace_capability name_ds_cap, size_t size);

	// Destruct all tasks.
	void clear_tasks();

	// Start idle tasks.
	void start();

	// Stop all tasks.
	void stop();

	// Get profiling data as an XML file.
	Genode::Ram_dataspace_capability profile_data();

protected:
	Server::Entrypoint& _ep;
	std::unordered_map<std::string, Genode::Attached_ram_dataspace> _binaries;
	Genode::Sliced_heap _heap;
	Genode::Cap_connection _cap;
	Genode::Service_registry _parent_services;

	// List instead of vector because reallocation would invalidate dataspaces.
	std::list<Task> _tasks;

	Genode::Trace::Connection _trace;
	Genode::Attached_ram_dataspace _profile_data;

	void _update_profile_data();
	Task* _task_by_name(const std::string& name);

	static Genode::Number_of_bytes _launchpad_quota();
	static Genode::Number_of_bytes _trace_quota();
	static Genode::Number_of_bytes _trace_buf_size();
	static Genode::Number_of_bytes _profile_ds_size();
};

struct Task_manager_root_component : Genode::Root_component<Task_manager_session_component>
{
public:
	Task_manager_root_component(Server::Entrypoint* ep, Genode::Allocator *allocator) :
		Genode::Root_component<Task_manager_session_component>(&ep->rpc_ep(), allocator),
		_ep(*ep)
	{
		PDBG("Creating root component.");
	}

protected:
	Server::Entrypoint& _ep;

	Task_manager_session_component* _create_session(const char *args)
	{
		PDBG("Creating Task Manager session.");
		return new (md_alloc()) Task_manager_session_component(_ep);
	}
};
