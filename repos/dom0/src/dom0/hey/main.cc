#include <base/printf.h>
#include <timer_session/connection.h>

int main(void)
{
	PDBG("hey: Hello!\n");
	Timer::Connection timer;
	for (;;)
	{
		PINF("hey\n");
		timer.msleep(3000);
	}
}
