#include <sys/timer.h>
#include <sys/kprintf.h>
#include <sys/pic.h>

__volatile__ uint32_t tick;
uint64_t sleep_time;

void
timer_callback(
			  )
{
	tick++;
//	kprintf("%d ", tick);
	PIC_sendEOI(0);
	uint32_t tmp_tick = tick % 100;
	if (tmp_tick == 0)
	{
//		uint64_t seconds = tick / 100;
//		uint64_t hours = seconds / 3600;
//		uint64_t minutes = (seconds % (3600)) / 60;
//		uint64_t secs = (seconds % (3600)) % 60;
//		kprintf("%d:%d:%d ", hours, minutes, secs);
//		kprintf("%d\t", tick);
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
	outb(I86_PIT_REG_COUNTER0, l);
	outb(I86_PIT_REG_COUNTER0, h);
}

void
sleep_shell(
			uint64_t secs
		   )
{
	
	sleep_time = 0;
	while(sleep_time < secs)
	{
		if((tick % 400) == 0)
		{
			sleep_time++;
		}
	}
	kprintf("\n");
}
