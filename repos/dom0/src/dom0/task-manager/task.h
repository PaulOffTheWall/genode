#pragma once

#include <util/noncopyable.h>
#include <util/xml_node.h>
#include <os/attached_ram_dataspace.h>
#include <launchpad/launchpad.h>
#include <timer_session/connection.h>
#include <base/signal.h>
#include <unordered_map>

// Noncopyable because dataspaces might get invalidated.
class Task : Genode::Noncopyable
{
public:
	Task(
		const Genode::Xml_node& node,
		std::unordered_map<std::string, Genode::Attached_ram_dataspace>& binaries,
		Launchpad& launchpad,
		Genode::Signal_receiver& sigRec);
	virtual ~Task();

	void run();
	void stop();
	std::string name() const;

protected:
	unsigned int _id;
	unsigned int _executionTime;
	unsigned int _criticalTime;
	unsigned int _priority;
	unsigned int _period;
	unsigned int _offset;
	Genode::Number_of_bytes _quota;
	char _binaryName[16];
	Genode::Attached_ram_dataspace _config;
	std::string _name;

	std::unordered_map<std::string, Genode::Attached_ram_dataspace>& _binaries;

	Launchpad& _launchpad;
	Launchpad_child* _child;

	Timer::Connection _timer;

	Genode::Signal_receiver& _sigRec;
	Genode::Signal_dispatcher<Task> _startDispatcher;

	void _makeName();
	void _start(unsigned);

	static bool _checkDynamicElf(Genode::Attached_ram_dataspace& ds);

	// Get XML node value (not attribute) if it exists.
	template <typename T>
	bool _getNodeValue(const Genode::Xml_node& configNode, const char* type, T* out)
	{
		if (configNode.has_sub_node(type))
		{
			configNode.sub_node(type).value<T>(out);
			return true;
		}
		return false;
	}

	// Get XML node string value (not attribute) if it exists.
	bool _getNodeValue(const Genode::Xml_node& configNode, const char* type, char* dst, size_t maxLen);
};
