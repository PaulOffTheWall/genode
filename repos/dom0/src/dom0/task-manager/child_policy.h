#pragma once

#include <init/child.h>

class ChildPolicy : public Genode::Child_policy, Genode::Client
{
public:
	ChildPolicy();
	virtual ~ChildPolicy();

	Genode::Service* resolve_session_request(const char* service_name, const char* args);
	void filter_session_args(const char* service, char* args, Genode::size_t args_len);
	bool announce_service(const char* service_name, Genode::Root_capability root, Genode::Allocator* alloc, Genode::Server*);
	void unregister_services();

protected:
	Init::Child_policy_enforce_labeling _labeling_policy;
};
