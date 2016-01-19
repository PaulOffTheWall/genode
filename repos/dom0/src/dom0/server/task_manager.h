#ifndef _TASK_MANAGER_H_
#define _TASK_MANAGER_H_

#include <vector>
#include <unordered_map>
#include <os/attached_ram_dataspace.h>
#include <launchpad/launchpad.h>

#include "task_description.h"

class TaskManager
{
public:
	TaskManager();
	virtual ~TaskManager();

	void addTask(const TaskDescription& td);
	void addTasks(const char* const json);
	void clearTasks();
	char* const getBinarySpace(const std::string& name, size_t size);
	void clearBinaries();
	void start();
	void stop();
	void startTask(const TaskDescription& td);

protected:
	std::unordered_map<std::string, Genode::Attached_ram_dataspace> _binaries;

	std::vector<TaskDescription> _tasks;
	Launchpad _launchpad;

	static bool _checkDynamicElf(Genode::Attached_ram_dataspace& ds);
};

#endif