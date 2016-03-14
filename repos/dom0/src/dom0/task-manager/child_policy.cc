#include "child_policy.h"

ChildPolicy::ChildPolicy()
{
}

Genode::Service* ChildPolicy::resolve_session_request(const char* service_name, const char* args)
{
}

void ChildPolicy::filter_session_args(const char* service, char* args, Genode::size_t args_len)
{
}

bool ChildPolicy::announce_service(const char* service_name, Genode::Root_capability root, Genode::Allocator* alloc, Genode::Server*)
{
}

void ChildPolicy::unregister_services()
{
}
