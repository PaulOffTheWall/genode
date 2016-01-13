#ifndef _TASK_MANAGER_H_
#define _TASK_MANAGER_H_

#include <vector>
#include <unordered_map>

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

protected:
	std::unordered_map<std::string, std::vector<char>> _binaries;
	std::vector<TaskDescription> _tasks;
};

#endif