#include "task_manager.h"

#include <cstring>
#include <base/env.h>
#include <base/elf.h>
#include <base/printf.h>
#include <base/process.h>
#include "json_util.h"
#include "config.h"

TaskManager::TaskManager() :
	_binaries(),
	_tasks(),
	_launchpad(Config::get().launchpadQuota)
{
	// Load dynamic linker for dynamically linked binaries.
	static Genode::Rom_connection ldso_rom("ld.lib.so");
	Genode::Process::dynamic_linker(ldso_rom.dataspace());
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
	Genode::Ram_session* ram = Genode::env()->ram_session();

	// Hoorray for C++ syntax. This basically forwards ctor arguments so there is no copy or dtor call involved which may invalidate the attached pointer.
	// Also, emplace returns a <iterator, bool> pair indicating insertion success, so we need .first to get the map iterator and ->second to get the actual dataspace.
	Genode::Attached_ram_dataspace& ds = _binaries.emplace(std::piecewise_construct, std::make_tuple(name), std::make_tuple(ram, size)).first->second;
	return ds.local_addr<char>();
}

void TaskManager::clearBinaries()
{
	_binaries.clear();
}

void TaskManager::start()
{
	stop();
	PINF("Starting tasks.\n");
	while (!_tasks.empty())
	{
		startTask(_tasks.back());
		_tasks.pop_back();
	}
}

void TaskManager::stop()
{
}

void TaskManager::startTask(const TaskDescription& td)
{
	Genode::Attached_ram_dataspace& ds = _binaries.at(td.binaryName);
	PINF("Starting %s linked task \"%s\".", _checkDynamicElf(ds) ? "dynamically" : "statically", td.binaryName);
	_launchpad.start_child(td.binaryName, Config::get().taskQuota, Genode::Dataspace_capability(), ds.cap());
}


bool TaskManager::_checkDynamicElf(Genode::Attached_ram_dataspace& ds)
{
	/* read program header */
	Genode::Elf_binary elf((Genode::addr_t)ds.local_addr<char>());
	return elf.is_dynamically_linked();
}
