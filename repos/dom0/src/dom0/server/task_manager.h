#ifndef _TASK_MANAGER_H_
#define _TASK_MANAGER_H_

#include <list>
#include <unordered_map>
#include <os/attached_ram_dataspace.h>
#include <launchpad/launchpad.h>

class TaskManager
{
protected:
	struct TaskDescription
	{
		TaskDescription(size_t configSize) :
			id(0),
			executionTime(0),
			criticalTime(0),
			priority(0),
			period(0),
			offset(0),
			config(Genode::env()->ram_session(), configSize)
		{
		}
		// We don't want no invalidated dataspace, no we don't.
		TaskDescription(const TaskDescription& td) = delete;
		TaskDescription& operator=(const TaskDescription& td) = delete;

		unsigned int id;
		unsigned int executionTime;
		unsigned int criticalTime;
		unsigned int priority;
		unsigned int period;
		unsigned int offset;
		Genode::Number_of_bytes quota;
		char binaryName[16];
		Genode::Attached_ram_dataspace config;
	};

public:
	TaskManager();
	virtual ~TaskManager();

	void addTasks(const char* const xml);
	void clearTasks();
	char* const getBinarySpace(const std::string& name, size_t size);
	void clearBinaries();
	void start();
	void stop();
	void startTask(const TaskDescription& td);

protected:
	std::unordered_map<std::string, Genode::Attached_ram_dataspace> _binaries;

	// List instead of vector because reallocation would invalidate dataspaces.
	std::list<TaskDescription> _tasks;
	Launchpad _launchpad;

	static bool _checkDynamicElf(Genode::Attached_ram_dataspace& ds);
};

#endif
