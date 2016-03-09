#include "task_manager.h"

#include <base/env.h>
#include <base/printf.h>
#include <base/process.h>
#include <util/xml_node.h>
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

void TaskManager::addTasks(const char* const xml)
{
	PINF("Parsing XML file.");
	Genode::Xml_node root(xml);

	const auto fn = [this] (const Genode::Xml_node& node)
	{
		_tasks.emplace_back(node, _binaries, _launchpad, _sigRec);
	};
	root.for_each_sub_node("periodictask", fn);
}

void TaskManager::clearTasks()
{
	_tasks.clear();
}

char* const TaskManager::getBinarySpace(const std::string& name, size_t size)
{
	PINF("Reserving %d bytes for binary '%s'.\n", size, name.c_str());
	Genode::Ram_session* ram = Genode::env()->ram_session();

	// Hoorray for C++ syntax. This basically forwards ctor arguments, constructing the dataspace in-place so there is no copy or dtor call involved which may invalidate the attached pointer.
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
	PINF("Starting tasks.\n");
	for (Task& task : _tasks)
	{
		task.run();
	}
}
