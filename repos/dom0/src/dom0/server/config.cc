#include "config.h"

#include <os/config.h>
#include <nic/packet_allocator.h>
#include <cstring>

bool attributeValue(const Genode::Xml_node& node, const char* type, char* dst, const char* defaultVal, size_t maxLen)
{
	if (node.has_attribute(type))
	{
		node.attribute(type).value(dst, maxLen);
		return true;
	}
	else
	{
		std::strcpy(dst, defaultVal);
	}
	return false;
}


// Set defaults and overwrite if XML entries are found in the run config.
Config loadConfig()
{
	Config config;

	const Genode::Xml_node& configNode = Genode::config()->xml_node();
	const Genode::Xml_node& serverNode = configNode.sub_node("server");

	config.bufSize = serverNode.attribute_value<unsigned int>("buf-size", Nic::Packet_allocator::DEFAULT_PACKET_SIZE * 128);
	attributeValue(serverNode, "dhcp", config.dhcp, "no", 4);
	attributeValue(serverNode, "listen-address", config.listenAddress, "0.0.0.0", 16);
	attributeValue(serverNode, "network-mask", config.networkMask, "255.255.255.0", 16);
	attributeValue(serverNode, "network-gateway", config.networkGateway, "192.168.0.254", 16);
	config.port = serverNode.attribute_value<unsigned int>("port", 3001);

	// Print config
	PINF("Config readouts:\n");
	PINF("\tBuffer size: %d\n", config.bufSize);
	PINF("\tUse DHCP: %s\n", config.dhcp);
	PINF("\tListening address: %s\n", config.listenAddress);
	PINF("\tNetwork mask: %s\n", config.networkMask);
	PINF("\tNetwork gateway: %s\n", config.networkGateway);
	PINF("\tPort: %d\n", config.port);

	return config;
}


// Get _the_ config.
const Config& Config::get()
{
	static Config config = {0};
	static bool loaded = false;

	if (!loaded)
	{
		config = loadConfig();
		loaded = true;
	}
	return config;
}
