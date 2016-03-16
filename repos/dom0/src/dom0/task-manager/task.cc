#include "task.h"

#include <cstring>
#include <vector>

#include <base/elf.h>
#include <base/lock.h>

Task::Child_policy::Child_policy(
	const std::string& name,
	Task& task,
	Genode::Service_registry& parent_services,
	Genode::Dataspace_capability config_ds,
	Genode::Dataspace_capability binary_ds,
	Genode::Rpc_entrypoint& parent_entrypoint) :
		_task{&task},
		_parent_services{&parent_services},
		_parent_entrypoint{&parent_entrypoint},
		_labeling_policy{name.c_str()},
		_config_policy{"config", config_ds, _parent_entrypoint},
		_binary_policy{"binary", binary_ds, _parent_entrypoint},
		_active{true}
{
	std::strncpy(_name, name.c_str(), 32);
}

void Task::Child_policy::exit(int exit_value)
{
	PDBG("child %s exited with exit value %d", name(), exit_value);

	_task->_stop_kill_timer();
	Task::Event::Type type;
	switch (exit_value)
	{
		case 0:
			type = Event::EXIT; break;
		case 17:
			type = Event::EXIT_CRITICAL; break;
		case 19:
			type = Event::EXIT_EXTERNAL; break;
		default:
			type = Event::EXIT_ERROR;
	}

	Task::log_profile_data(type, _task->_name, _task->_shared);
	Task::_child_destructor.submit_for_destruction(_task);
	_active = false;
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

void Task::Child_policy::filter_session_args(const char *service, char *args, Genode::size_t args_len)
{
	_labeling_policy.filter_session_args(service, args, args_len);
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
	Task& task,
	size_t quota,
	unsigned int priority,
	Genode::Service_registry& parent_services,
	Genode::Dataspace_capability config_ds,
	Genode::Dataspace_capability binary_ds,
	Genode::Rpc_entrypoint& parent_entrypoint) :
		Meta{name, quota, priority},
		policy{name, task, parent_services, config_ds, binary_ds, parent_entrypoint},
		child{binary_ds, pd.cap(), ram.cap(), cpu.cap(), rm.cap(), &parent_entrypoint, &policy}
{
}

const char* Task::Event::type_name(Type type)
{
	switch (type)
	{
		case START: return "START";
		case EXIT: return "EXIT";
		case EXIT_CRITICAL: return "EXIT_CRITICAL";
		case EXIT_ERROR: return "EXIT_ERROR";
		case EXIT_EXTERNAL: return "EXIT_EXTERNAL";
		case EXTERNAL: return "EXTERNAL";
		default: return "UNKNOWN";
	}
}

Task::Shared_data::Shared_data(size_t trace_quota, size_t trace_buf_size) :
	binaries{},
	heap{Genode::env()->ram_session(), Genode::env()->rm_session()},
	parent_services{},
	trace{trace_quota, trace_buf_size, 0}
{
}

Task::Task(Server::Entrypoint& ep, Genode::Cap_connection& cap, Shared_data& shared, const Genode::Xml_node& node) :
		_shared(shared),
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
		_paused{true},
		_start_timer{},
		_kill_timer{},
		_start_dispatcher{ep, *this, &Task::_start},
		_kill_dispatcher{ep, *this, &Task::_kill_crit},
		_idle_dispatcher{ep, *this, &Task::_idle},
		_child_ep{&cap, 12 * 1024, _name.c_str(), false},
		_meta{nullptr}
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
	_paused = false;

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
	_paused = true;
	_stop_timers();
	_kill(19);
}

std::string Task::name() const
{
	return _name;
}

bool Task::running() const
{
	return _meta != nullptr;
}

Task* Task::task_by_name(std::list<Task>& tasks, const std::string& name)
{
	for (Task& task : tasks)
	{
		if (task.name() == name)
		{
			return &task;
		}
	}
	return nullptr;
}

void Task::log_profile_data(Event::Type type, const std::string& task_name, Shared_data& shared)
{
	static const size_t MAX_NUM_SUBJECTS = 128;
	// Lock to avoid race conditions as this may be called by the child's thread.
	Genode::Lock::Guard guard(shared.log_lock);

	Genode::Trace::Subject_id subjects[MAX_NUM_SUBJECTS];
	const size_t num_subjects = shared.trace.subjects(subjects, MAX_NUM_SUBJECTS);
	Genode::Trace::Subject_info info;

	shared.event_log.emplace_back();
	Event& event = shared.event_log.back();

	event.type = type;
	event.task_name = task_name;
	event.time_stamp = shared.timer.elapsed_ms();

	for (Genode::Trace::Subject_id* subject = subjects; subject < subjects + num_subjects; ++subject)
	{
		info = shared.trace.subject_info(*subject);
		event.task_infos.emplace_back();
		Event::Task_info& task_info = event.task_infos.back();

		task_info.id = subject->id;
		task_info.session = info.session_label().string(),
		task_info.thread = info.thread_name().string(),
		task_info.state = info.state(),
		task_info.execution_time = info.execution_time().value;

		// Check if the session is started by this task manager (i.e., a managed task).
		size_t leaf_pos = task_info.session.rfind("task-manager -> ");
		Task* task = nullptr;
		if (leaf_pos < std::string::npos)
		{
			const std::string process = task_info.session.substr(leaf_pos + 16);
			if (process == task_info.thread)
			{
				task = task_by_name(shared.tasks, task_info.session.substr(leaf_pos + 16));
			}
		}
		if (task && task->running())
		{
			task_info.managed = true;
			task_info.managed_info.quota = task->_meta->ram.quota();
			task_info.managed_info.used = task->_meta->ram.used();
		}
		// Check if this is task-manager itself.
		else if (task_info.session.rfind("task-manager") == task_info.session.length() - 12 && task_info.thread == "task-manager")
		{
			task_info.managed = true;
			task_info.managed_info.quota = Genode::env()->ram_session()->quota();
			task_info.managed_info.used = Genode::env()->ram_session()->used();
		}
	}
}

std::string Task::_make_name() const
{
	char id[4];
	snprintf(id, sizeof(id), "%.2d.", _id);
	return std::string(id) + _binary_name;
}

void Task::_start(unsigned)
{
	if (_paused)
	{
		// This might happen if start timeout is triggered before a stop call but is handled after.
		return;
	}

	if (running())
	{
		PINF("Trying to start %s but previous instance still running or undestroyed. Abort.\n", _name.c_str());
		return;
	}

	// Check if binary has already been received.
	auto bin_it = _shared.binaries.find(_binary_name);
	if (bin_it == _shared.binaries.end())
	{
		PERR("Binary %s for task %s not found, possibly not yet received by dom0.", _binary_name.c_str(), _name.c_str());
		return;
	}

	Genode::Attached_ram_dataspace& ds = bin_it->second;

	PINF("Starting %s linked task %s with quota %u and priority %u", _check_dynamic_elf(ds) ? "dynamically" : "statically", _name.c_str(), (size_t)_quota, _priority);

	if ((size_t)_quota < 512 * 1024)
	{
		PWRN("Warning: RAM quota for %s might be too low to hold meta data.", _name.c_str());
	}

	// Dispatch kill timer after critical time.
	if (_critical_time > 0)
	{
		_kill_timer.trigger_once(_critical_time * 1000);
	}

	// Abort if RAM quota insufficient. Alternatively, we could give all remaining quota to the child.
	if (_quota > Genode::env()->ram_session()->avail()) {
		PERR("Not enough RAM quota for task %s, requested: %u, available: %u", _name.c_str(), (size_t)_quota, Genode::env()->ram_session()->avail());
		return;
	}

	try
	{
		_meta = new (&_shared.heap) Meta_ex(_name, *this, _quota, _priority, _shared.parent_services, _config.cap(), ds.cap(), _child_ep);
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

	log_profile_data(Event::START, _name, _shared);
}

Task::Child_destructor_thread::Child_destructor_thread() :
	Thread{"child_destructor"},
	_lock{},
	_queued{}
{
	start();
}

void Task::Child_destructor_thread::submit_for_destruction(Task* task)
{
	Genode::Lock::Guard guard(_lock);
	_queued.push_back(task);
}

void Task::Child_destructor_thread::entry()
{
	while (true)
	{
		_lock.lock();
		for (Task* task : _queued)
		{
			PDBG("Destroying task %s", task->_name.c_str());
			Genode::destroy(task->_shared.heap, task->_meta);
			task->_meta = nullptr;
		}
		_queued.clear();
		_lock.unlock();
		_timer.msleep(10);
	}
}

Task::Child_destructor_thread Task::_child_destructor;

void Task::_kill_crit(unsigned)
{
	// Check for paused status for the rare case where timer signals have been triggered before stopping but are handled after.
	if (!_paused)
	{
		PINF("Critical time reached for %s", _name.c_str());
		_kill(17);
	}
}

void Task::_kill(int exit_value)
{
	// Task might have a valid _meta and be inactive for the short time between submitting the task for destruction and the actual destruction. In that case we do nothing.
	if (_meta && _meta->policy.active())
	{
		PINF("Force-exiting %s", _name.c_str());
		_meta->child.exit(exit_value);
	}
}

void Task::_idle(unsigned)
{
	// Do nothing.
}

void Task::_stop_timers()
{
	// "Stop" timers. Apparently there is no way to stop a running timer, so instead we let it trigger an idle method.
	_stop_kill_timer();
	_stop_start_timer();
}

void Task::_stop_kill_timer()
{
	// "Stop" timers. Apparently there is no way to stop a running timer, so instead we let it trigger an idle method.
	_kill_timer.sigh(_idle_dispatcher);
	_kill_timer.trigger_once(0);
}

void Task::_stop_start_timer()
{
	_start_timer.sigh(_idle_dispatcher);
	_start_timer.trigger_once(0);
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
