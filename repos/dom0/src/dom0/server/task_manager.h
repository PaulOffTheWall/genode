#pragma once

#include <list>
#include <unordered_map>
#include <os/attached_ram_dataspace.h>
#include <launchpad/launchpad.h>
#include <base/signal.h>
#include "task.h"

class TaskManager
{
public:
	TaskManager();
	virtual ~TaskManager();

	void addTasks(const char* const xml);
	void clearTasks();
	char* const getBinarySpace(const std::string& name, size_t size);
	void clearBinaries();
	void start();
	void stop();

protected:
	std::unordered_map<std::string, Genode::Attached_ram_dataspace> _binaries;

	// List instead of vector because reallocation would invalidate dataspaces.
	std::list<Task> _tasks;
	Launchpad _launchpad;
	Genode::Signal_receiver _sigRec;
};
