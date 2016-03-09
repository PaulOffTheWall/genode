#include "task.h"

#include <base/elf.h>
#include <cstring>
#include "config.h"

Task::Task(const Genode::Xml_node& node, std::unordered_map<std::string, Genode::Attached_ram_dataspace>& binaries) :
	_config(Genode::env()->ram_session(), node.sub_node("config").size()),
	_binaries(binaries)
{
	const Genode::Xml_node& configNode = node.sub_node("config");

	getNodeValue(node, "td", &_id);
	getNodeValue(node, "executiontime", &_executionTime);
	getNodeValue(node, "criticaltime", &_criticalTime);
	getNodeValue(node, "priority", &_priority);
	getNodeValue(node, "period", &_period);
	getNodeValue(node, "offset", &_offset);
	getNodeValue(node, "quota", &_quota);
	getNodeValue(node, "pkg", _binaryName, 16);
	std::strncpy(_config.local_addr<char>(), configNode.addr(), configNode.size());
}

Task::~Task()
{
}

void Task::start(Launchpad& launchpad)
{
	Genode::Attached_ram_dataspace& ds = _binaries.at(_binaryName);
	PINF("Starting %s linked task \"%s\".", _checkDynamicElf(ds) ? "dynamically" : "statically", _binaryName);
	launchpad.start_child(_binaryName, _quota, _config.cap(), ds.cap());
}

bool Task::_checkDynamicElf(Genode::Attached_ram_dataspace& ds)
{
	/* read program header */
	Genode::Elf_binary elf((Genode::addr_t)ds.local_addr<char>());
	return elf.is_dynamically_linked();
}
