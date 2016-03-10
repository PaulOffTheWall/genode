#include "task_manager_session_component.h"

#include <base/env.h>
#include <base/printf.h>
#include <base/process.h>
#include <util/xml_node.h>

TaskManagerSessionComponent::TaskManagerSessionComponent() :
	_binaries(),
	_tasks(),
	_launchpad(_launchpadQuota())
{
	// Load dynamic linker for dynamically linked binaries.
	static Genode::Rom_connection ldso_rom("ld.lib.so");
	Genode::Process::dynamic_linker(ldso_rom.dataspace());
}

TaskManagerSessionComponent::~TaskManagerSessionComponent()
{
}

void TaskManagerSessionComponent::addTasks(Genode::Ram_dataspace_capability xmlDs)
{
	const char* xml = (const char*)Genode::env()->rm_session()->attach(xmlDs);
	PINF("Parsing XML file.");
	Genode::Xml_node root(xml);

	const auto fn = [this] (const Genode::Xml_node& node)
	{
		_tasks.emplace_back(node, _binaries, _launchpad, _sigRec);
	};
	root.for_each_sub_node("periodictask", fn);
}

void TaskManagerSessionComponent::clearTasks()
{
	_tasks.clear();
}

char* const TaskManagerSessionComponent::getBinarySpace(const std::string& name, size_t size)
{
	PINF("Reserving %d bytes for binary '%s'.\n", size, name.c_str());
	Genode::Ram_session* ram = Genode::env()->ram_session();

	// Hoorray for C++ syntax. This basically forwards ctor arguments, constructing the dataspace in-place so there is no copy or dtor call involved which may invalidate the attached pointer.
	// Also, emplace returns a <iterator, bool> pair indicating insertion success, so we need .first to get the map iterator and ->second to get the actual dataspace.
	Genode::Attached_ram_dataspace& ds = _binaries.emplace(std::piecewise_construct, std::make_tuple(name), std::make_tuple(ram, size)).first->second;
	return ds.local_addr<char>();
}

void TaskManagerSessionComponent::clearBinaries()
{
	_binaries.clear();
}

void TaskManagerSessionComponent::start()
{
	PINF("Starting tasks.\n");
	for (Task& task : _tasks)
	{
		task.run();
	}
}

Genode::Number_of_bytes TaskManagerSessionComponent::_launchpadQuota()
{
	Genode::Xml_node launchpadNode = Genode::config()->xml_node().sub_node("launchpad");
	return launchpadNode.attribute_value<Genode::Number_of_bytes>("quota", 10 * 1024 * 1024);
}
