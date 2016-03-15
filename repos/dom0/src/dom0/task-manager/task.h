#pragma once

#include <unordered_map>

#include <base/service.h>
#include <cap_session/connection.h>
#include <init/child.h>
#include <os/attached_ram_dataspace.h>
#include <os/server.h>
#include <os/signal_rpc_dispatcher.h>
#include <timer_session/connection.h>
#include <util/noncopyable.h>
#include <util/xml_node.h>

// Noncopyable because dataspaces might get invalidated.
class Task : Genode::Noncopyable
{
public:
	// Policy for handling binary and service requests.
	struct Child_policy : Genode::Child_policy
	{
	public:
		Child_policy(
			const std::string& name,
			Genode::Service_registry& parent_services,
			Genode::Dataspace_capability config_ds,
			Genode::Dataspace_capability binary_ds,
			Genode::Rpc_entrypoint& parent_entrypoint);

		virtual void exit(int exit_value) override;
		virtual const char *name() const override;
		Genode::Service *resolve_session_request(const char *service_name, const char *args);

		virtual bool active() const;

	protected:
		char _name[32];
		Genode::Service_registry* _parent_services;
		Genode::Rpc_entrypoint* _parent_entrypoint;
		Init::Child_policy_provide_rom_file  _config_policy;
		Init::Child_policy_provide_rom_file  _binary_policy;
		bool _active;
	};

	// Meta data that needs to be dynamically allocated on each start request.
	struct Meta
	{
	public:
		Meta(const std::string& name, size_t quota);

		Genode::Ram_connection ram;
		Genode::Cpu_connection cpu;
		Genode::Rm_connection rm;
		Genode::Pd_connection pd;
	};

	struct Meta_ex : Meta
	{
		Meta_ex(
			const std::string& name,
			size_t quota,
			Genode::Service_registry& parent_services,
			Genode::Dataspace_capability config_ds,
			Genode::Dataspace_capability binary_ds,
			Genode::Rpc_entrypoint& parent_entrypoint);

		Child_policy policy;
		Genode::Child child;
	};

	Task(
		Server::Entrypoint& ep,
		std::unordered_map<std::string, Genode::Attached_ram_dataspace>& binaries,
		Genode::Sliced_heap& heap,
		Genode::Cap_session& cap,
		Genode::Service_registry& parent_services,
		const Genode::Xml_node& node);
	virtual ~Task();

	void run();
	void stop();
	std::string name() const;
	Meta_ex* const meta();

protected:
	// All binaries loaded by the task manager.
	std::unordered_map<std::string, Genode::Attached_ram_dataspace>& _binaries;

	// Heap on which to create the init child.
	Genode::Sliced_heap& _heap;

	// Core services provided by the parent.
	Genode::Service_registry& _parent_services;

	// XML task description.
	unsigned int _id;
	unsigned int _execution_time;
	unsigned int _critical_time;
	unsigned int _priority;
	unsigned int _period;
	unsigned int _offset;
	Genode::Number_of_bytes _quota;
	std::string _binary_name;

	Genode::Attached_ram_dataspace _config;
	std::string _name;

	// Periodic timers.
	Timer::Connection _start_timer;
	Timer::Connection _kill_timer;

	// Timer dispatchers registering callbacks.
	Genode::Signal_rpc_member<Task> _start_dispatcher;
	Genode::Signal_rpc_member<Task> _kill_dispatcher;
	Genode::Signal_rpc_member<Task> _idle_dispatcher;

	// Child process entry point.
	Genode::Rpc_entrypoint _child_ep;

	// Child meta data.
	Meta_ex* _meta;

	// Combine ID and binary name into a unique name, e.g. 01.namaste
	std::string _make_name() const;

	// Start task once.
	void _start(unsigned);
	void _kill_crit(unsigned);
	void _kill();
	void _idle(unsigned);

	// Check if the provided ELF is dynamic by reading the header.
	static bool _check_dynamic_elf(Genode::Attached_ram_dataspace& ds);

	// Get XML node value (not attribute) if it exists.
	template <typename T>
	static T _get_node_value(const Genode::Xml_node& config_node, const char* type, T default_val = T())
	{
		if (config_node.has_sub_node(type))
		{
			T out{};
			config_node.sub_node(type).value<T>(&out);
			return out;
		}
		return default_val;
	}

	// Get XML node string value (not attribute) if it exists.
	static std::string _get_node_value(const Genode::Xml_node& config_node, const char* type, size_t max_len, const std::string& default_val = "");
};
