#include "task.h"

#include <base/elf.h>
#include <cstring>

Task::Task(
	const Genode::Xml_node& node,
	std::unordered_map<std::string, Genode::Attached_ram_dataspace>& binaries,
	Launchpad& launchpad,
	Server::Entrypoint& ep) :
		_config(Genode::env()->ram_session(), node.sub_node("config").size()),
		_name(),
		_binaries(binaries),
		_launchpad(launchpad),
		_child(nullptr),
		_startDispatcher(ep, *this, &Task::_start),
		_killDispatcher(ep, *this, &Task::_kill),
		_idleDispatcher(ep, *this, &Task::_idle)
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
}

Task::~Task()
{
	stop();
}

void Task::run()
{
	// Register timeout handlers.
	_startTimer.sigh(_startDispatcher);
	_killTimer.sigh(_killDispatcher);

	if (_period > 0)
	{
		_startTimer.trigger_periodic(_period * 1000);
	}
	else
	{
		_start(0);
	}
}

void Task::stop()
{
	PINF("Stopping task %s\n", _name.c_str());
	if (_child)
	{
		_launchpad.exit_child(_child);
		_child = nullptr;
	}

	// "Stop" timers.
	_startTimer.sigh(_idleDispatcher);
	_killTimer.sigh(_idleDispatcher);
	_startTimer.trigger_once(0);
	_killTimer.trigger_once(0);
}

std::string Task::name() const
{
	return _name;
}

Launchpad_child* Task::child() const
{
	return _child;
}

void Task::_makeName()
{
	char name[20];
	snprintf(name, sizeof(name), "%.2d.%s", _id, _binaryName);
	_name = name;
}

void Task::_start(unsigned)
{
	if (_child)
	{
		PINF("Trying to start %s but previous instance still running. Killing first.\n", _name.c_str());
		_launchpad.exit_child(_child);
		_child = nullptr;
	}
	Genode::Attached_ram_dataspace& ds = _binaries.at(_binaryName);

	_makeName();
	PINF("Starting %s linked task %s", _checkDynamicElf(ds) ? "dynamically" : "statically", _name.c_str());
	_child = _launchpad.start_child(_name.c_str(), _quota, _config.cap(), ds.cap());

	// Launchpad might give the child a different name if a task with the same name already exists.
	// This will be announced to the console by Launchpad.
	_name = _child->name();

	// Dispatch kill timer after critical time.
	_killTimer.trigger_once(_criticalTime * 1000);
}

void Task::_kill(unsigned)
{
	if (_child)
	{
		PINF("Critical time reached. Killing %s\n", _name.c_str());
		_launchpad.exit_child(_child);
		_child = nullptr;
	}
}

void Task::_idle(unsigned)
{
	// Do nothing.
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
