#include <base/printf.h>
#include <timer_session/connection.h>

int main(void)
{
	PINF("hey: Hello!\n");
	Timer::Connection timer;
	for (;;)
	{
		PINF("hey\n");
		timer.msleep(200);
	}
}
