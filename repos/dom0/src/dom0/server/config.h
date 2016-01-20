#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <util/xml_node.h>

// Get XML node value (not attribute) if it exists.
template <typename T>
bool getNodeValue(const Genode::Xml_node& configNode, const char* type, T* out)
{
	if (configNode.has_sub_node(type))
	{
		configNode.sub_node(type).value<T>(out);
		return true;
	}
	return false;
}

// Get XML node string value (not attribute) if it exists.
bool getNodeValue(const Genode::Xml_node& configNode, const char* type, char* dst, size_t maxLen);

struct Config
{
	unsigned int bufSize;
	char dhcp[4];
	char listenAddress[16];
	char networkMask[16];
	char networkGateway[16];
	unsigned int port;
	Genode::Number_of_bytes launchpadQuota;

	static const Config& get();
};

#endif