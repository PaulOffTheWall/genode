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
	struct ChildPolicy : Genode::Child_policy
	{
	public:
		ChildPolicy(
			const std::string& name,
			Genode::Service_registry& parentServices,
			Genode::Dataspace_capability configDs,
			Genode::Dataspace_capability binaryDs,
			Genode::Rpc_entrypoint& parentEntrypoint);

		virtual void exit(int exitValue) override;
		virtual const char *name() const override;
		Genode::Service *resolve_session_request(const char *serviceName, const char *args);

		virtual bool active() const;

	protected:
		char _name[32];
		Genode::Service_registry* _parentServices;
		Genode::Rpc_entrypoint* _parentEntrypoint;
		Init::Child_policy_provide_rom_file  _configPolicy;
		Init::Child_policy_provide_rom_file  _binaryPolicy;
		bool _active;
	};

	Task(
		Server::Entrypoint& ep,
		std::unordered_map<std::string, Genode::Attached_ram_dataspace>& binaries,
		Genode::Sliced_heap& heap,
		Genode::Cap_session& cap,
		Genode::Service_registry& parentServices,
		const Genode::Xml_node& node);
	virtual ~Task();

	void run();
	void stop();
	std::string name() const;

protected:
	// All binaries loaded by the task manager.
	std::unordered_map<std::string, Genode::Attached_ram_dataspace>& _binaries;

	// Heap on which to create the init child.
	Genode::Sliced_heap& _heap;
	Genode::Service_registry& _parentServices;

	// XML task description.
	unsigned int _id;
	unsigned int _executionTime;
	unsigned int _criticalTime;
	unsigned int _priority;
	unsigned int _period;
	unsigned int _offset;
	Genode::Number_of_bytes _quota;
	std::string _binaryName;

	Genode::Attached_ram_dataspace _config;
	std::string _name;

	// Periodic timers.
	Timer::Connection _startTimer;
	Timer::Connection _killTimer;

	// Timer dispatchers registering callbacks.
	Genode::Signal_rpc_member<Task> _startDispatcher;
	Genode::Signal_rpc_member<Task> _killDispatcher;
	Genode::Signal_rpc_member<Task> _idleDispatcher;

	// Child process entry point.
	Genode::Rpc_entrypoint _childEp;

	// Service connections required for Genode child initialization.
	Genode::Ram_connection _ram;
	Genode::Cpu_connection _cpu;
	Genode::Rm_connection _rm;
	Genode::Pd_connection _pd;

	ChildPolicy* _childPolicy;
	// Actual init child process.
	Genode::Child* _child;

	// Combine ID and binary name into a unique name, e.g. 01.namaste
	std::string _makeName() const;

	// Start task once.
	void _start(unsigned);
	void _killCrit(unsigned);
	void _kill();
	void _idle(unsigned);

	// Check if the provided ELF is dynamic by reading the header.
	static bool _checkDynamicElf(Genode::Attached_ram_dataspace& ds);

	// Get XML node value (not attribute) if it exists.
	template <typename T>
	static T _getNodeValue(const Genode::Xml_node& configNode, const char* type, T defaultVal = T())
	{
		if (configNode.has_sub_node(type))
		{
			T out{};
			configNode.sub_node(type).value<T>(&out);
			return out;
		}
		return defaultVal;
	}

	// Get XML node string value (not attribute) if it exists.
	static std::string _getNodeValue(const Genode::Xml_node& configNode, const char* type, size_t maxLen, const std::string& defaultVal = "");
};
