#include <sys/timer.h>
#include <sys/kprintf.h>

uint32_t tick;
uint64_t sleep_time;

void
timer_callback(
			  )
{
	tick++;
	uint32_t tmp_tick = tick % 100;
	if (tmp_tick == 0)
	{
		uint64_t seconds = tick / 100;
		uint64_t hours = seconds / 3600;
		uint64_t minutes = (seconds % (3600)) / 60;
		uint64_t secs = (seconds % (3600)) % 60;
		print_timer("Time Elapsed Since Boot: %d:%d:%d ", hours, minutes, secs);
	}
}

void
init_timer(
		  )
{
	int frequency = 100;
	uint64_t mh_freq = MH_FREQ;
	uint16_t divisor = mh_freq/frequency;
	// Send the command byte.
	outb(0x43, 0x36);

	// Divisor has to be sent byte-wise, so split here into upper/lower bytes.
	uint8_t l = (uint8_t)(divisor & 0xFF);
	uint8_t h = (uint8_t)(divisor >> 8);

	// Send the frequency divisor.
	outb(0x40, l);
	outb(0x40, h);
}

void
sleep_shell(
			int secs
		   )
{
	sleep_time = secs * 100;
	while(sleep_time != 0)
	{
		sleep_time--;
	}
}
