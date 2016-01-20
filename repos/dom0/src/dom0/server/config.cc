#include "config.h"

#include <nic/packet_allocator.h>
#include <cstring>

bool getNodeValue(const Genode::Xml_node& configNode, const char* type, char* dst, size_t maxLen)
{
	if (configNode.has_sub_node(type))
	{
		configNode.sub_node(type).value(dst, maxLen);
		return true;
	}
	return false;
}


// Set defaults and overwrite if XML entries are found in the run config.
Config loadConfig()
{
	Config config;

	// Defaults
	config.bufSize = Nic::Packet_allocator::DEFAULT_PACKET_SIZE * 128;
	std::strcpy(config.dhcp, "no");
	std::strcpy(config.listenAddress, "0.0.0.0");
	std::strcpy(config.networkMask, "255.255.255.0");
	std::strcpy(config.networkGateway, "192.168.0.254");
	config.port = 3001;
	config.launchpadQuota = 16*1024*1024;

	const Genode::Xml_node& configNode = Genode::config()->xml_node();
	getNodeValue(configNode, "buf-size", &config.bufSize);
	getNodeValue(configNode, "dhcp", config.dhcp, 4);
	getNodeValue(configNode, "listen-address", config.listenAddress, 16);
	getNodeValue(configNode, "network-mask", config.networkMask, 16);
	getNodeValue(configNode, "network-gateway", config.networkGateway, 16);
	getNodeValue(configNode, "port", &config.port);
	getNodeValue(configNode, "launchpad-quota", &config.launchpadQuota);

	// Print config
	PINF("Config readouts:\n");
	PINF("\tBuffer size: %d\n", config.bufSize);
	PINF("\tUse DHCP: %s\n", config.dhcp);
	PINF("\tListening address: %s\n", config.listenAddress);
	PINF("\tNetwork mask: %s\n", config.networkMask);
	PINF("\tNetwork gateway: %s\n", config.networkGateway);
	PINF("\tPort: %d\n", config.port);
	PINF("\tLaunchpad Quota: %d\n", config.launchpadQuota);

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