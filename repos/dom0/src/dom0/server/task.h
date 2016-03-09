#pragma once

#include <util/noncopyable.h>
#include <util/xml_node.h>
#include <os/attached_ram_dataspace.h>
#include <launchpad/launchpad.h>
#include <unordered_map>

// Noncopyable because dataspaces might get invalidated.
class Task : Genode::Noncopyable
{
public:
	Task(const Genode::Xml_node& node, std::unordered_map<std::string, Genode::Attached_ram_dataspace>& binaries);
	virtual ~Task();

	void start(Launchpad& launchpad);

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
	std::unordered_map<std::string, Genode::Attached_ram_dataspace>& _binaries;

	static bool _checkDynamicElf(Genode::Attached_ram_dataspace& ds);
};
