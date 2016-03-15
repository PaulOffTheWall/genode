#include "task.h"

#include <base/elf.h>
#include <cstring>
#include <vector>

Task::Child_policy::Child_policy(
	const std::string& name,
	Genode::Service_registry& parentServices,
	Genode::Dataspace_capability configDs,
	Genode::Dataspace_capability binaryDs,
	Genode::Rpc_entrypoint& parentEntrypoint) :
		_parentServices(&parentServices),
		_parentEntrypoint(&parentEntrypoint),
		_configPolicy("config", configDs, _parentEntrypoint),
		_binaryPolicy("binary", binaryDs, _parentEntrypoint),
		_active(true)
{
	std::strncpy(_name, name.c_str(), 32);
}

void Task::Child_policy::exit(int exitValue)
{
	_active = false;
	PDBG("child %s exited with exit value %d", name(), exitValue);
}

const char* Task::Child_policy::name() const
{
	return _name;
}

bool Task::Child_policy::active() const
{
	return _active;
}

Genode::Service* Task::Child_policy::resolve_session_request(const char* serviceName, const char* args)
{
	Genode::Service* service = nullptr;

	// check for config file request
	if ((service = _configPolicy.resolve_session_request(serviceName, args)))
	{
		return service;
	}

	// check for binary file request
	if ((service = _binaryPolicy.resolve_session_request(serviceName, args)))
	{
		return service;
	}

	// check parent services
	if ((service = _parentServices->find(serviceName)))
	{
		return service;
	}

	PERR("Service %s requested by %s not found.", serviceName, _name);
	return nullptr;
}

Task::Meta::Meta(const std::string& name, size_t quota) :
	ram{},
	cpu{name.c_str()},
	rm{},
	pd{}
{
	ram.ref_account(Genode::env()->ram_session_cap());
	if (Genode::env()->ram_session()->transfer_quota(ram.cap(), quota) != 0)
	{
		PWRN("Failed to transfer RAM quota to child %s", name.c_str());
	}
}

Task::Meta_ex::Meta_ex(
	const std::string& name,
	size_t quota,
	Genode::Service_registry& parentServices,
	Genode::Dataspace_capability configDs,
	Genode::Dataspace_capability binaryDs,
	Genode::Rpc_entrypoint& parentEntrypoint) :
		Meta{name, quota},
		policy{name, parentServices, configDs, binaryDs, parentEntrypoint},
		child{binaryDs, pd.cap(), ram.cap(), cpu.cap(), rm.cap(), &parentEntrypoint, &policy}
{
}

Task::Task(
	Server::Entrypoint& ep,
	std::unordered_map<std::string, Genode::Attached_ram_dataspace>& binaries,
	Genode::Sliced_heap& heap,
	Genode::Cap_session& cap,
	Genode::Service_registry& parentServices,
	const Genode::Xml_node& node) :
		_binaries{binaries},
		_heap{heap},
		_parentServices{parentServices},
		_id           {_getNodeValue<unsigned int>(node, "id")},
		_executionTime{_getNodeValue<unsigned int>(node, "executiontime")},
		_criticalTime {_getNodeValue<unsigned int>(node, "criticaltime")},
		_priority     {_getNodeValue<unsigned int>(node, "priority")},
		_period       {_getNodeValue<unsigned int>(node, "period")},
		_offset       {_getNodeValue<unsigned int>(node, "offset")},
		_quota        {_getNodeValue<Genode::Number_of_bytes>(node, "quota")},
		_binaryName   {_getNodeValue(node, "pkg", 32, "")},
		_config{Genode::env()->ram_session(), node.sub_node("config").size()},
		_name{_makeName()},
		_startTimer{},
		_killTimer{},
		_startDispatcher{ep, *this, &Task::_start},
		_killDispatcher{ep, *this, &Task::_killCrit},
		_idleDispatcher{ep, *this, &Task::_idle},
		_childEp{&cap, 12 * 1024, _name.c_str(), false},
		_meta(nullptr)
{
	const Genode::Xml_node& configNode = node.sub_node("config");
	std::strncpy(_config.local_addr<char>(), configNode.addr(), configNode.size());
}

Task::~Task()
{
	stop();
}

void Task::run()
{
	// (Re-)Register timeout handlers.
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
	_kill();

	// "Stop" timers. Apparently there is no way to stop a running timer, so instead we let it trigger an idle method.
	_startTimer.sigh(_idleDispatcher);
	_killTimer.sigh(_idleDispatcher);
	_startTimer.trigger_once(0);
	_killTimer.trigger_once(0);
}

std::string Task::name() const
{
	return _name;
}

Task::Meta_ex* const Task::meta()
{
	return _meta;
}

std::string Task::_makeName() const
{
	char id[4];
	snprintf(id, sizeof(id), "%.2d.", _id);
	return std::string(id) + _binaryName;
}

void Task::_start(unsigned)
{
	if (_meta)
	{
		PINF("Trying to start %s but previous instance still running. Killing first.\n", _name.c_str());
		_kill();
	}

	// Check if binary has already been received.
	auto binIt = _binaries.find(_binaryName);
	if (binIt == _binaries.end())
	{
		PERR("Binary %s for task %s not found, possibly not yet received by dom0.", _binaryName.c_str(), _name.c_str());
		return;
	}

	Genode::Attached_ram_dataspace& ds = binIt->second;

	PINF("Starting %s linked task %s with quota %u", _checkDynamicElf(ds) ? "dynamically" : "statically", _name.c_str(), (size_t)_quota);

	// Dispatch kill timer after critical time.
	_killTimer.trigger_once(_criticalTime * 1000);

	// Abort if RAM quota insufficient. Alternatively, we could give all remaining quota to the child.
	if (_quota > Genode::env()->ram_session()->avail()) {
		PERR("Not enough RAM quota for task %s, requested: %u, available: %u", _name.c_str(), (size_t)_quota, Genode::env()->ram_session()->avail());
		return;
	}

	try
	{
		PINF("Allocating child meta data on heap.");
		_meta = new (&_heap) Meta_ex(_name, _quota, _parentServices, _config.cap(), ds.cap(), _childEp);
		_childEp.activate();
	}
	catch (Genode::Cpu_session::Thread_creation_failed)
	{
		PWRN("Failed to create child - Cpu_session::Thread_creation_failed");
	}
	catch (...)
	{
		PWRN("Failed to create child - unknown reason");
	}
}

void Task::_killCrit(unsigned)
{
	PINF("Critical time reached for %s", _name.c_str());
	_kill();
}

void Task::_kill()
{
	if (_meta)
	{
		if (_meta->policy.active())
		{
			PINF("Killing %s", _name.c_str());
			_meta->child.exit(-1);
		}
		if (_meta)
		{
			Genode::destroy(_heap, _meta);
			_meta = nullptr;
		}
	}
}

void Task::_idle(unsigned)
{
	// Do nothing.
}

bool Task::_checkDynamicElf(Genode::Attached_ram_dataspace& ds)
{
	// Read program header.
	Genode::Elf_binary elf((Genode::addr_t)ds.local_addr<char>());
	return elf.is_dynamically_linked();
}

std::string Task::_getNodeValue(const Genode::Xml_node& configNode, const char* type, size_t maxLen, const std::string& defaultVal)
{
	if (configNode.has_sub_node(type))
	{
		std::vector<char> out(maxLen);
		configNode.sub_node(type).value(out.data(), maxLen);
		return out.data();
	}
	return defaultVal;
}
