#include "task.h"

#include <base/elf.h>
#include <cstring>
#include <vector>

Task::Child_policy::Child_policy(
	const std::string& name,
	Genode::Service_registry& parent_services,
	Genode::Dataspace_capability config_ds,
	Genode::Dataspace_capability binary_ds,
	Genode::Rpc_entrypoint& parent_entrypoint) :
		_parent_services(&parent_services),
		_parent_entrypoint(&parent_entrypoint),
		_config_policy("config", config_ds, _parent_entrypoint),
		_binary_policy("binary", binary_ds, _parent_entrypoint),
		_active(true)
{
	std::strncpy(_name, name.c_str(), 32);
}

void Task::Child_policy::exit(int exit_value)
{
	_active = false;
	PDBG("child %s exited with exit value %d", name(), exit_value);
}

const char* Task::Child_policy::name() const
{
	return _name;
}

bool Task::Child_policy::active() const
{
	return _active;
}

Genode::Service* Task::Child_policy::resolve_session_request(const char* service_name, const char* args)
{
	Genode::Service* service = nullptr;

	// check for config file request
	if ((service = _config_policy.resolve_session_request(service_name, args)))
	{
		return service;
	}

	// check for binary file request
	if ((service = _binary_policy.resolve_session_request(service_name, args)))
	{
		return service;
	}

	// check parent services
	if ((service = _parent_services->find(service_name)))
	{
		return service;
	}

	PERR("Service %s requested by %s not found.", service_name, _name);
	return nullptr;
}

Task::Meta::Meta(const std::string& name, size_t quota, unsigned int priority) :
	ram{},
	// Scale priority.
	cpu{name.c_str(), (long int)priority * Genode::config()->xml_node().attribute_value<long int>("prio_levels", 0)},
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
	unsigned int priority,
	Genode::Service_registry& parent_services,
	Genode::Dataspace_capability config_ds,
	Genode::Dataspace_capability binary_ds,
	Genode::Rpc_entrypoint& parent_entrypoint) :
		Meta{name, quota, priority},
		policy{name, parent_services, config_ds, binary_ds, parent_entrypoint},
		child{binary_ds, pd.cap(), ram.cap(), cpu.cap(), rm.cap(), &parent_entrypoint, &policy}
{
}

Task::Task(
	Server::Entrypoint& ep,
	std::unordered_map<std::string, Genode::Attached_ram_dataspace>& binaries,
	Genode::Sliced_heap& heap,
	Genode::Cap_session& cap,
	Genode::Service_registry& parent_services,
	const Genode::Xml_node& node) :
		_binaries{binaries},
		_heap{heap},
		_parent_services{parent_services},
		_id           {_get_node_value<unsigned int>(node, "id")},
		_execution_time{_get_node_value<unsigned int>(node, "executiontime")},
		_critical_time {_get_node_value<unsigned int>(node, "criticaltime")},
		_priority     {_get_node_value<unsigned int>(node, "priority")},
		_period       {_get_node_value<unsigned int>(node, "period")},
		_offset       {_get_node_value<unsigned int>(node, "offset")},
		_quota        {_get_node_value<Genode::Number_of_bytes>(node, "quota")},
		_binary_name   {_get_node_value(node, "pkg", 32, "")},
		_config{Genode::env()->ram_session(), node.sub_node("config").size()},
		_name{_make_name()},
		_start_timer{},
		_kill_timer{},
		_start_dispatcher{ep, *this, &Task::_start},
		_kill_dispatcher{ep, *this, &Task::_kill_crit},
		_idle_dispatcher{ep, *this, &Task::_idle},
		_child_ep{&cap, 12 * 1024, _name.c_str(), false},
		_meta(nullptr)
{
	const Genode::Xml_node& config_node = node.sub_node("config");
	std::strncpy(_config.local_addr<char>(), config_node.addr(), config_node.size());
}

Task::~Task()
{
	stop();
}

void Task::run()
{
	// (Re-)Register timeout handlers.
	_start_timer.sigh(_start_dispatcher);
	_kill_timer.sigh(_kill_dispatcher);

	if (_period > 0)
	{
		_start_timer.trigger_periodic(_period * 1000);
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
	_start_timer.sigh(_idle_dispatcher);
	_kill_timer.sigh(_idle_dispatcher);
	_start_timer.trigger_once(0);
	_kill_timer.trigger_once(0);
}

std::string Task::name() const
{
	return _name;
}

Task::Meta_ex* const Task::meta()
{
	return _meta;
}

std::string Task::_make_name() const
{
	char id[4];
	snprintf(id, sizeof(id), "%.2d.", _id);
	return std::string(id) + _binary_name;
}

void Task::_start(unsigned)
{
	if (_meta)
	{
		PINF("Trying to start %s but previous instance still running. Killing first.\n", _name.c_str());
		_kill();
	}

	// Check if binary has already been received.
	auto bin_it = _binaries.find(_binary_name);
	if (bin_it == _binaries.end())
	{
		PERR("Binary %s for task %s not found, possibly not yet received by dom0.", _binary_name.c_str(), _name.c_str());
		return;
	}

	Genode::Attached_ram_dataspace& ds = bin_it->second;

	PINF("Starting %s linked task %s with quota %u and priority %u", _check_dynamic_elf(ds) ? "dynamically" : "statically", _name.c_str(), (size_t)_quota, _priority);

	// Dispatch kill timer after critical time.
	_kill_timer.trigger_once(_critical_time * 1000);

	// Abort if RAM quota insufficient. Alternatively, we could give all remaining quota to the child.
	if (_quota > Genode::env()->ram_session()->avail()) {
		PERR("Not enough RAM quota for task %s, requested: %u, available: %u", _name.c_str(), (size_t)_quota, Genode::env()->ram_session()->avail());
		return;
	}

	try
	{
		PINF("Allocating child meta data on heap.");
		_meta = new (&_heap) Meta_ex(_name, _quota, _priority, _parent_services, _config.cap(), ds.cap(), _child_ep);
		_child_ep.activate();
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

void Task::_kill_crit(unsigned)
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

bool Task::_check_dynamic_elf(Genode::Attached_ram_dataspace& ds)
{
	// Read program header.
	Genode::Elf_binary elf((Genode::addr_t)ds.local_addr<char>());
	return elf.is_dynamically_linked();
}

std::string Task::_get_node_value(const Genode::Xml_node& config_node, const char* type, size_t max_len, const std::string& default_val)
{
	if (config_node.has_sub_node(type))
	{
		std::vector<char> out(max_len);
		config_node.sub_node(type).value(out.data(), max_len);
		return out.data();
	}
	return default_val;
}
