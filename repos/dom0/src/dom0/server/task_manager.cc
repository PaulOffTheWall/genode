#include "task_manager.h"

#include <cstring>
#include <base/env.h>
#include <base/elf.h>
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
		const Genode::Xml_node& configNode = node.sub_node("config");
		_tasks.emplace_back(configNode.size());

		TaskDescription& td = _tasks.back();

		getNodeValue(node, "td", &td.id);
		getNodeValue(node, "executiontime", &td.executionTime);
		getNodeValue(node, "criticaltime", &td.criticalTime);
		getNodeValue(node, "priority", &td.priority);
		getNodeValue(node, "period", &td.period);
		getNodeValue(node, "offset", &td.offset);
		getNodeValue(node, "quota", &td.quota);
		getNodeValue(node, "pkg", td.binaryName, 16);
		std::strncpy(td.config.local_addr<char>(), configNode.addr(), configNode.size());
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
	stop();
	PINF("Starting tasks.\n");
	for (const TaskDescription& td : _tasks)
	{
		startTask(td);
	}
}

void TaskManager::stop()
{
}

void TaskManager::startTask(const TaskDescription& td)
{
	Genode::Attached_ram_dataspace& ds = _binaries.at(td.binaryName);
	PINF("Starting %s linked task \"%s\".", _checkDynamicElf(ds) ? "dynamically" : "statically", td.binaryName);
	_launchpad.start_child(td.binaryName, td.quota, td.config.cap(), ds.cap());
}


bool TaskManager::_checkDynamicElf(Genode::Attached_ram_dataspace& ds)
{
	/* read program header */
	Genode::Elf_binary elf((Genode::addr_t)ds.local_addr<char>());
	return elf.is_dynamically_linked();
}
