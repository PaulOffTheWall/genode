#include <base/printf.h>
#include <os/attached_rom_dataspace.h>
#include <timer_session/connection.h>
#include <trace_session/connection.h>
#include <cstdlib>
#include <cstring>

int main(int argc, char* argv[])
{
	PDBG("cpumon: Hello!\n");

	Timer::Connection timer;

	Genode::Trace::Connection trace(1024*1024, 64*1024, 0);
	Genode::Trace::Subject_id subjects[32];
	size_t numSubjects = trace.subjects(subjects, 32);
	PINF("numSubjects: %d\n", numSubjects);

	for (size_t i = 0; i < numSubjects; ++i)
	{
		Genode::Trace::Subject_info info = trace.subject_info(subjects[i]);
		PINF("Tracing subject: %s, thread: %s, state: %s, execution time: %llu\n", info.session_label().string(), info.thread_name().string(), Genode::Trace::Subject_info::state_name(info.state()), info.execution_time().value);
	}

	return 0;
}
