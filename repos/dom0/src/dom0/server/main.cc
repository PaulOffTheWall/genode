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
	char listenAddress[16];
	unsigned int port;
};

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

bool getNodeValue(const Genode::Xml_node& configNode, const char* type, char* dst, size_t max_len)
{
	if (configNode.has_sub_node(type))
	{
		configNode.sub_node(type).value(dst, max_len);
		return true;
	}
	return false;
}

Config loadConfig()
{
	Config config;

	// Defaults
	config.bufSize = Nic::Packet_allocator::DEFAULT_PACKET_SIZE * 128;
	std::strcpy(config.listenAddress, "0.0.0.0");
	config.port = 3001;

	const Genode::Xml_node& configNode = Genode::config()->xml_node();
	getNodeValue(configNode, "buf-size", &config.bufSize);
	getNodeValue(configNode, "listen-address", config.listenAddress, 16);
	getNodeValue(configNode, "port", &config.port);

	// Print config
	PINF("Config readouts:\n");
	PINF("\tBuffer size: %d\n", config.bufSize);
	PINF("\tListening address: %s\n", config.listenAddress);
	PINF("\tPort: %d\n", config.port);

	return config;
}

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
	Launchpad launchpad(1024*1024);

	lwip_tcpip_init();

	/* Initialize network stack and do DHCP */
	if (lwip_nic_init(0, 0, 0, config.bufSize, config.bufSize)) {
		PERR("We got no IP address!");
		return 1;
	}
	ip_addr_t addr = {0};
	inet_aton(config.listenAddress, &addr);

	TcpServerSocket server(addr, config.port);
	int32_t message = 0;

	std::unordered_map<std::string, std::vector<char>> binaries;
	std::vector<taskDescription> tasks;

	/* Check if dataspace was falsely freed */
	while (true)
	{
		// launchpad.start_child("hello", 512*1024, cap);
		server.connect();

		while (true)
		{
			//The first 4 Byte of a new message always
			//tell us what the message contains,
			//either a LUA or a binary (control) command
			// NETCHECK_LOOP(server.receiveInt32_t(message));
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
			break;
		}
		break;
	}

	return 0;
}

int main(int argc, char* argv[])
{
	PDBG("dom0: Hello!\n");

	Config config = loadConfig();

	return runServer(config);
}
