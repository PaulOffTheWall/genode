#include <base/printf.h>
#include <base/allocator.h>
#include <base/exception.h>
#include <os/config.h>
#include <nic/packet_allocator.h>
#include <lwip/genode.h>
#include <launchpad/launchpad.h>

#include <string>
#include <cstring>
#include <vector>
#include <unordered_map>

#include "tcp_server_socket.h"
#include "communication_magic_numbers.h"
#include "json_util.h"
#include "task_description.h"

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
};

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
bool getNodeValue(const Genode::Xml_node& configNode, const char* type, char* dst, size_t max_len)
{
	if (configNode.has_sub_node(type))
	{
		configNode.sub_node(type).value(dst, max_len);
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
	config.launchpadQuota = 4*1024*1024;
	config.taskQuota = 512*1024;

	const Genode::Xml_node& configNode = Genode::config()->xml_node();
	getNodeValue(configNode, "buf-size", &config.bufSize);
	getNodeValue(configNode, "dhcp", config.dhcp, 4);
	getNodeValue(configNode, "listen-address", config.listenAddress, 16);
	getNodeValue(configNode, "network-mask", config.networkMask, 16);
	getNodeValue(configNode, "network-gateway", config.networkGateway, 16);
	getNodeValue(configNode, "port", &config.port);
	getNodeValue(configNode, "launchpad-quota", &config.launchpadQuota);
	getNodeValue(configNode, "task-quota", &config.taskQuota);

	// Print config
	PINF("Config readouts:\n");
	PINF("\tBuffer size: %d\n", config.bufSize);
	PINF("\tUse DHCP: %s\n", config.dhcp);
	PINF("\tListening address: %s\n", config.listenAddress);
	PINF("\tNetwork mask: %s\n", config.networkMask);
	PINF("\tNetwork gateway: %s\n", config.networkGateway);
	PINF("\tPort: %d\n", config.port);
	PINF("\tLaunchpad Quota: %d\n", config.launchpadQuota);
	PINF("\tTask Quota: %d\n", config.taskQuota);

	return config;
}

// Parse json file for task descriptions and save in struct vector.
void parseTaskDescription(const char* json, std::vector<taskDescription>& tasks)
{
	PINF("Parsing the json file.");
	jsmn_parser p;
	jsmntok_t t[128];

	int res, pos;

	jsmn_init(&p);
	res = jsmn_parse(&p, json, strlen(json), t, sizeof(t)/sizeof(t[0]));

	if (res < 0)
	{
		PERR("Failed to parse.");
		return;
	}

	if (res<1 || t[0].type !=JSMN_ARRAY)
	{
		PERR("Array expected.");
		return;
	}

	pos = 1;

	for (int j=0; j < t[0].size; j++)
	{
		if (t[pos].type != JSMN_OBJECT)
		{
			pos++; j--; continue;
		}

		pos++;
		tasks.emplace_back();
		taskDescription& td = tasks.back();
		jsonreadint(json, t, &pos, "id", &td.id);
		jsonreadint(json, t, &pos, "executiontime", &td.executionTime);
		jsonreadint(json, t, &pos, "criticaltime", &td.criticalTime);
		jsonreadint(json, t, &pos, "priority", &td.priority);
		jsonreadint(json, t, &pos, "period", &td.period);
		jsonreadint(json, t, &pos, "offset", &td.offset);
		jsonreadint(json, t, &pos, "matrixSize", &td.matrixSize);
		jsonreadstring(json, t, &pos, "pkg", &td.binaryName);
	}

	for (const taskDescription& task : tasks)
	{
		PINF("Task ID: %d, Matrix Size: %d, Binary: %s\n", task.id, task.matrixSize, task.binaryName);
	}
}

int runServer(const Config& config)
{
	Launchpad launchpad(config.launchpadQuota);

	lwip_tcpip_init();

	char listenAddress[16];
	if (std::strcmp(config.dhcp, "yes") == 0)
	{
		*listenAddress = 0;
		if (lwip_nic_init(0, 0, 0, config.bufSize, config.bufSize)) {
			PERR("We got no IP address!");
			return 1;
		}
	}
	else
	{
		std::strcpy(listenAddress, config.listenAddress);
		if (lwip_nic_init(inet_addr(config.listenAddress), inet_addr(config.networkMask), inet_addr(config.networkGateway), config.bufSize, config.bufSize)) {
			PERR("We got no IP address!");
			return 1;
		}
	}
	TcpServerSocket server(listenAddress, config.port);
	int32_t message = 0;

	std::unordered_map<std::string, std::vector<char>> binaries;
	std::vector<taskDescription> tasks;

	while (true)
	{
		server.connect();

		while (true)
		{
			//The first 4 Byte of a new message always
			//tell us what the message contains,
			//either a LUA or a binary (control) command
			NETCHECK_LOOP(server.receiveInt32_t(message));
			message = CONTROL;
			if (message == LUA)
			{
				PERR("LUA Message received but not yet supported.\n");
			}
			if (message == CONTROL)
			{
				PINF("Control Message received");
				NETCHECK_LOOP(server.receiveInt32_t(message));
				if (message == SEND_BINARY)
				{
					//The communication partner wants to send us a binary.

					//Get the size of the binary.
					NETCHECK_LOOP(server.receiveInt32_t(message));
					PDBG("Binary size will be %i Bytes.\n", message);
					std::vector<char>& bin = binaries[""];
					bin.resize(message);

					//Our partner can now start sending.
					NETCHECK_LOOP(server.sendInt32_t(GO_SEND));

					//Get it...
					NETCHECK_LOOP(server.receiveData(bin.data(), bin.size()));
					PINF("Binary received.\n");
				}
			}
			else if (message == TASK_DESC)
			{
				PINF("Ready to receive task description\n");
				// get the length of the JSON file
				// receive the JSON
				// read the JSON
				// store it in an array of structs

				int json_size;

				// get the size of JSON
				NETCHECK_LOOP(server.receiveInt32_t(json_size));
				json_size = 512;

				PINF("Ready to receive json of size %d \n",json_size);
				std::vector<char> json(json_size);

				// get the JSON data (save in the same char* space for binary
				NETCHECK_LOOP(server.receiveData(json.data(), json.size()));

				PINF("received JSON %s", json.data());
				parseTaskDescription(json.data(), tasks);
			}
			else if (message == SEND_BINARIES)
			{
				// read the number of binaries
				// loop
				// read 8 byte binary name
				// read the binary name
				// read the binary length
				// save the binary

				PINF("Ready to receive binaries.\n");
				int numBinaries = 0;
				NETCHECK_LOOP(server.receiveInt32_t(numBinaries));

				PINF("%d binar%s to be sent.\n", numBinaries, numBinaries == 1 ? "y" : "ies");

				for (int i = 0; i < numBinaries; i++)
				{
					PINF("Waiting for Binary %d\n", i);
					char name[9];

					NETCHECK_LOOP(server.receiveData(name, 8));
					name[8]='\0';
					std::string binaryName(name);

					int binarySize;
					NETCHECK_LOOP(server.receiveInt32_t(binarySize));
					std::vector<char>& binary = binaries[binaryName];
					binary.resize(binarySize);

					NETCHECK_LOOP(server.receiveData(binary.data(), binarySize));

					PINF("Got binary %s of size %d.\n", name, binarySize);

					NETCHECK_LOOP(server.sendInt32_t(GO_SEND));
				}
			}
			else if (message == START)
			{
				PINF("Starting tasks.\n");
				while (!tasks.empty())
				{
					taskDescription& task = tasks.back();
					launchpad.start_child(task.binaryName, config.taskQuota, Genode::Dataspace_capability());
					tasks.pop_back();
				}
			}
		}
	}

	return 0;
}

int main(int argc, char* argv[])
{
	PDBG("dom0: Hello!\n");

	Config config = loadConfig();

	return runServer(config);
}
