#pragma once

#include <util/xml_node.h>

// Get XML node attribute if it exists and copy default if not.
bool attributeValue(const Genode::Xml_node& configNode, const char* type, char* dst, const char* defaultVal, size_t maxLen);

struct Config
{
	unsigned int bufSize;
	char dhcp[4];
	char listenAddress[16];
	char networkMask[16];
	char networkGateway[16];
	unsigned int port;

	static const Config& get();
};
