#include "task_manager_session_component.h"

#include <base/env.h>
#include <base/printf.h>
#include <base/process.h>
#include <util/xml_node.h>
#include <util/xml_generator.h>

#include <string>

Task_manager_session_component::Task_manager_session_component(Server::Entrypoint& ep) :
	_ep{ep},
	_binaries{},
	_heap{Genode::env()->ram_session(), Genode::env()->rm_session()},
	_cap{},
	_parent_services{},
	_tasks{},
	_trace{_trace_quota(), _trace_buf_size(), 0},
	_profile_data{Genode::env()->ram_session(), _profile_ds_size()}
{
	// Load dynamic linker for dynamically linked binaries.
	static Genode::Rom_connection ldso_rom("ld.lib.so");
	Genode::Process::dynamic_linker(ldso_rom.dataspace());

	// Names of services provided by the parent.
	static const char* names[] =
	{
		// core services
		"CAP", "RAM", "RM", "PD", "CPU", "IO_MEM", "IO_PORT",
		"IRQ", "ROM", "LOG", "SIGNAL"
	};
	for (const char* name : names)
	{
		_parent_services.insert(new (Genode::env()->heap()) Genode::Parent_service(name));
	}
}

Task_manager_session_component::~Task_manager_session_component()
{
}

void Task_manager_session_component::add_tasks(Genode::Ram_dataspace_capability xml_ds_cap)
{
	Genode::Rm_session* rm = Genode::env()->rm_session();
	const char* xml = rm->attach(xml_ds_cap);
	PDBG("Parsing XML file:\n%s", xml);
	Genode::Xml_node root(xml);

	const auto fn = [this] (const Genode::Xml_node& node)
	{
		_tasks.emplace_back(_ep, _binaries, _heap, _cap, _parent_services, node);
	};
	root.for_each_sub_node("periodictask", fn);
	rm->detach(xml);
}

void Task_manager_session_component::clear_tasks()
{
	PDBG("Clearing %d task%s. Binaries still held.", _tasks.size(), _tasks.size() == 1 ? "" : "s");
	_tasks.clear();
}

Genode::Ram_dataspace_capability Task_manager_session_component::binary_ds(Genode::Ram_dataspace_capability name_ds_cap, size_t size)
{
	Genode::Rm_session* rm = Genode::env()->rm_session();
	const char* name = rm->attach(name_ds_cap);
	PDBG("Reserving %d bytes for binary %s", size, name);
	Genode::Ram_session* ram = Genode::env()->ram_session();

	// Hoorray for C++ syntax. This basically forwards ctor arguments, constructing the dataspace in-place so there is no copy or dtor call involved which may invalidate the attached pointer.
	// Also, emplace returns a <iterator, bool> pair indicating insertion success, so we need .first to get the map iterator and ->second to get the actual dataspace.
	Genode::Attached_ram_dataspace& ds = _binaries.emplace(std::piecewise_construct, std::make_tuple(name), std::make_tuple(ram, size)).first->second;
	rm->detach(name);
	return ds.cap();
}

void Task_manager_session_component::start()
{
	PINF("Starting %d task%s.", _tasks.size(), _tasks.size() == 1 ? "" : "s");
	for (Task& task : _tasks)
	{
		task.run();
	}
}

void Task_manager_session_component::stop()
{
	PINF("Stopping all tasks.");
	for (Task& task : _tasks)
	{
		task.stop();
	}
}

Genode::Ram_dataspace_capability Task_manager_session_component::profile_data()
{
	_update_profile_data();
	return _profile_data.cap();
}

void Task_manager_session_component::_update_profile_data()
{
	static const size_t MAX_NUM_SUBJECTS = 32;

	Genode::Trace::Subject_id subjects[MAX_NUM_SUBJECTS];
	size_t num_subjects = _trace.subjects(subjects, MAX_NUM_SUBJECTS);
	Genode::Trace::Subject_info info;
	std::string label;
	std::string thread;
	Genode::Trace::Subject_info::State state;

	// Xml_generator directly writes XML data into the buffer on construction, explaining the heavy recursion here.
	Genode::Xml_generator xml(_profile_data.local_addr<char>(), _profile_data.size(), "profile-data", [&]()
	{
		for (Genode::Trace::Subject_id* subject = subjects; subject < subjects + num_subjects; ++subject)
		{
			info = _trace.subject_info(*subject);
			label = info.session_label().string();
			thread = info.thread_name().string();
			state = info.state();
			xml.node("subject", [&]()
			{
				xml.attribute("id", std::to_string(subject->id).c_str());
				xml.node("session", [&]()
				{
					xml.append(label.c_str());
				});
				xml.node("thread", [&]()
				{
					xml.append(thread.c_str());
				});
				xml.node("state", [&]()
				{
					xml.append(Genode::Trace::Subject_info::state_name(state));
				});
				xml.node("execution-time", [&]()
				{
					xml.append(std::to_string(info.execution_time().value).c_str());
				});

				// Check if the session is started by this task manager.
				size_t leaf_pos = label.rfind("task-manager -> ");
				Task* task = nullptr;
				if (leaf_pos < std::string::npos)
				{
					std::string process = label.substr(leaf_pos + 16);
					if (process == thread)
					{
						task = _task_by_name(label.substr(leaf_pos + 16));
					}
				}
				if (task && task->meta())
				{
					// Add more detail to tasks started by us while their process still exists.
					xml.node("task-info", [&]()
					{
						xml.node("quota", [&]()
						{
							xml.append(std::to_string(task->meta()->ram.quota()).c_str());
						});
						xml.node("used", [&]()
						{
							xml.append(std::to_string(task->meta()->ram.used()).c_str());
						});
					});
				}
				// Check if this is task-manager itself.
				else if (label.rfind("task-manager") == label.length() - 12 && thread == "task-manager")
				{
					xml.node("task-info", [&]()
					{
						xml.node("quota", [&]()
						{
							xml.append(std::to_string(Genode::env()->ram_session()->quota()).c_str());
						});
						xml.node("used", [&]()
						{
							xml.append(std::to_string(Genode::env()->ram_session()->used()).c_str());
						});
					});
				}
			});
		}
	});
}

Task* Task_manager_session_component::_task_by_name(const std::string& name)
{
	for (Task& task : _tasks)
	{
		if (task.name() == name)
		{
			return &task;
		}
	}
	return nullptr;
}

Genode::Number_of_bytes Task_manager_session_component::_launchpad_quota()
{
	Genode::Xml_node launchpad_node = Genode::config()->xml_node().sub_node("launchpad");
	return launchpad_node.attribute_value<Genode::Number_of_bytes>("quota", 10 * 1024 * 1024);
}

Genode::Number_of_bytes Task_manager_session_component::_trace_quota()
{
	Genode::Xml_node launchpad_node = Genode::config()->xml_node().sub_node("trace");
	return launchpad_node.attribute_value<Genode::Number_of_bytes>("quota", 1024 * 1024);
}

Genode::Number_of_bytes Task_manager_session_component::_trace_buf_size()
{
	Genode::Xml_node launchpad_node = Genode::config()->xml_node().sub_node("trace");
	return launchpad_node.attribute_value<Genode::Number_of_bytes>("buf-size", 64 * 1024);
}

Genode::Number_of_bytes Task_manager_session_component::_profile_ds_size()
{
	Genode::Xml_node launchpad_node = Genode::config()->xml_node().sub_node("profile");
	return launchpad_node.attribute_value<Genode::Number_of_bytes>("ds-size", 128 * 1024);
}
