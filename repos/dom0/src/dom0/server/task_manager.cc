#include "task_manager.h"

#include <cstring>
#include <base/printf.h>
#include "json_util.h"

TaskManager::TaskManager() :
	_binaries(),
	_tasks()
{
}

TaskManager::~TaskManager()
{
}

void TaskManager::addTask(const TaskDescription& td)
{
	_tasks.push_back(td);
}

void TaskManager::addTasks(const char* const json)
{
	clearTasks();

	PINF("Parsing json file.");
	jsmn_parser p;
	jsmntok_t t[128];

	int res, pos;

	jsmn_init(&p);
	res = jsmn_parse(&p, json, strlen(json), t, sizeof(t)/sizeof(t[0]));

	if (res < 0)
	{
		PERR("Failed to parse.\n");
		return;
	}

	if (res<1 || t[0].type !=JSMN_ARRAY)
	{
		PERR("Array expected.\n");
		return;
	}

	pos = 1;

	for (int j=0; j < t[0].size; j++)
	{
		if (t[pos].type != JSMN_OBJECT)
		{
			pos++; j--; continue;
		}

		pos++;
		TaskDescription td;
		jsonreadint(json, t, &pos, "id", &td.id);
		jsonreadint(json, t, &pos, "executiontime", &td.executionTime);
		jsonreadint(json, t, &pos, "criticaltime", &td.criticalTime);
		jsonreadint(json, t, &pos, "priority", &td.priority);
		jsonreadint(json, t, &pos, "period", &td.period);
		jsonreadint(json, t, &pos, "offset", &td.offset);
		jsonreadint(json, t, &pos, "matrixSize", &td.matrixSize);
		jsonreadstring(json, t, &pos, "pkg", &td.binaryName);

		addTask(td);
	}

	for (const TaskDescription& task : _tasks)
	{
		PINF("Task ID: %d, Matrix Size: %d, Binary: %s\n", task.id, task.matrixSize, task.binaryName);
	}
}

void TaskManager::clearTasks()
{
	_tasks.clear();
}

char* const TaskManager::getBinarySpace(const std::string& name, size_t size)
{
	PINF("Reserving %d bytes for binary '%s'.\n", size, name.c_str());
	_binaries[name].resize(size);
	return _binaries[name].data();
}

void TaskManager::clearBinaries()
{
	_binaries.clear();
}

void TaskManager::start()
{
	PINF("Starting tasks.\n");
	while (!_tasks.empty())
	{
		_tasks.pop_back();
	}
}