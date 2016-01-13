#ifndef _CONFIG_H_
#define _CONFIG_H_

struct Config
{
	unsigned int bufSize;
	char dhcp[4];
	char listenAddress[16];
	char networkMask[16];
	char networkGateway[16];
	unsigned int port;
	unsigned int launchpadQuota;
	unsigned int taskQuota;

	static const Config& get();
};

#endif