#include "task.h"

#include <base/elf.h>
#include <cstring>

Task::Task(
	const Genode::Xml_node& node,
	std::unordered_map<std::string, Genode::Attached_ram_dataspace>& binaries,
	Launchpad& launchpad,
	Genode::Signal_receiver& sigRec) :
		_config(Genode::env()->ram_session(), node.sub_node("config").size()),
		_binaries(binaries),
		_launchpad(launchpad),
		_sigRec(sigRec),
		_startDispatcher(_sigRec, *this, &Task::_start)
{
	const Genode::Xml_node& configNode = node.sub_node("config");

	_getNodeValue(node, "id", &_id);
	_getNodeValue(node, "executiontime", &_executionTime);
	_getNodeValue(node, "criticaltime", &_criticalTime);
	_getNodeValue(node, "priority", &_priority);
	_getNodeValue(node, "period", &_period);
	_getNodeValue(node, "offset", &_offset);
	_getNodeValue(node, "quota", &_quota);
	_getNodeValue(node, "pkg", _binaryName, 16);
	std::strncpy(_config.local_addr<char>(), configNode.addr(), configNode.size());
	_makeName();

	// Register timeout handler.
	_timer.sigh(_startDispatcher);
}

Task::~Task()
{
}

void Task::run()
{
	_start(0);
}

std::string Task::name() const
{
	return _name;
}

void Task::_makeName()
{
	char name[20];
	snprintf(name, sizeof(name), "%.2d.%s", _id, _binaryName);
	_name = name;
}

void Task::_start(unsigned)
{
	Genode::Attached_ram_dataspace& ds = _binaries.at(_binaryName);

	_makeName();
	PINF("Starting %s linked task \"%s\".", _checkDynamicElf(ds) ? "dynamically" : "statically", _name.c_str());
	_child = _launchpad.start_child(_name.c_str(), _quota, _config.cap(), ds.cap());

	// Launchpad might give the child a different name if a task with the same name already exists.
	// This will be announced to the console by Launchpad.
	_name = _child->name();
}

bool Task::_checkDynamicElf(Genode::Attached_ram_dataspace& ds)
{
	/* read program header */
	Genode::Elf_binary elf((Genode::addr_t)ds.local_addr<char>());
	return elf.is_dynamically_linked();
}

bool Task::_getNodeValue(const Genode::Xml_node& configNode, const char* type, char* dst, size_t maxLen)
{
	if (configNode.has_sub_node(type))
	{
		configNode.sub_node(type).value(dst, maxLen);
		return true;
	}
	return false;
}
