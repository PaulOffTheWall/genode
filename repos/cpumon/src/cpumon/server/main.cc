#include <base/printf.h>
#include <timer_session/connection.h>

int main(int argc, char* argv[])
{
	PDBG("cpumon: Hello!\n");

	Timer::Connection timer;

	while (true)
	{
		PINF("Fake monitoring data: %d\n", 5);
		timer.msleep(2000);
	}

	return 0;
}
