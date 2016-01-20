#include <base/printf.h>
#include <timer_session/connection.h>

int main(void)
{
	PDBG("namaste: Hello!\n");
	Timer::Connection timer;
	for (;;)
	{
		PINF("namaste\n");
		timer.msleep(200);
	}
}
