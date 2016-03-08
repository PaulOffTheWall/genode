#include <base/printf.h>
#include <os/attached_rom_dataspace.h>
#include <timer_session/connection.h>
#include <ram_session/connection.h>
#include <trace_session/connection.h>
#include <cstdlib>
#include <cstring>
#include <string>

struct ProcessInfo
{
	std::string session;
	std::string thread;
	std::string state;
	unsigned policyId;
	unsigned long long executionTime;
	size_t traceBufferSize;
};

int main(int argc, char* argv[])
{
	PDBG("cpumon: Hello!\n");
	using namespace Fiasco;

	Timer::Connection timer;
	ProcessInfo pInfo;
	Genode::Trace::Connection trace(1024*1024, 64*1024, 0);

	while (true)
	{
		Genode::Trace::Subject_id subjects[32];
		size_t numSubjects = trace.subjects(subjects, 32);
		PINF("numSubjects: %d\n", numSubjects);

		unsigned long long totalTime = 0LL;
		for (size_t i = 0; i < numSubjects; ++i)
		{
			Genode::Trace::Subject_info info = trace.subject_info(subjects[i]);

			pInfo.session = info.session_label().string();
			pInfo.thread = info.thread_name().string();
			pInfo.state = Genode::Trace::Subject_info::state_name(info.state());
			pInfo.policyId = info.policy_id().id;
			pInfo.executionTime = info.execution_time().value;
			pInfo.traceBufferSize = 0;

			Genode::Dataspace_capability dsCap = trace.buffer(subjects[i]);
			if (info.state() == Genode::Trace::Subject_info::TRACED)
			{
				Genode::Dataspace_client ds(dsCap);
				pInfo.traceBufferSize = ds.size();
			}

			PINF("Tracing subject: %s, thread: %s, state: %s, policy: %u, execution time: %llu\n, trace buffer size: %d",
				pInfo.session.c_str(),
				pInfo.thread.c_str(),
				pInfo.state.c_str(),
				pInfo.policyId,
				pInfo.executionTime,
				pInfo.traceBufferSize);
			totalTime += pInfo.executionTime;
		}
		PINF("Total execution time of all threads: %llu\n", totalTime);
		PINF("Time on timer session (ms): %lu\n", timer.elapsed_ms());
		timer.msleep(5000);
	}

	return 0;
}
