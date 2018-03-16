#include <sys/timer.h>
#include <sys/kprintf.h>
#include <sys/pic.h>
#include <sys/process.h>

__volatile__ uint32_t tick;
//uint64_t sleep_time = 0;

void
timer_callback(
			  )
{
	PIC_sendEOI(0);
	tick++;
	if ((tick % 160) == 0 && boot_done && process_count > 1)
	{
		usr_schedule();	
	}
	uint32_t tmp_tick = tick % 100;
	if (tmp_tick == 0)
	{
//		uint64_t seconds = tick / 100;
//		uint64_t hours = seconds / 3600;
//		uint64_t minutes = (seconds % (3600)) / 60;
//		uint64_t secs = (seconds % (3600)) % 60;
//		kprintf("%d:%d:%d ", hours, minutes, secs);
//		kprintf("%d\t", tick);
//		sleep_time++;
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
	outb(I86_PIT_REG_COMMAND, 0x36);

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
//	uint64_t sleep_time = tick;
	uint64_t tmp_tick = tick % 100; 
	if(tmp_tick == 0)
	{
		while (tmp_tick <= secs)
		{
			tmp_tick++;
		}
//		if((tick % 1000) == 0)
//		{
//			tmp_tick++;
//		}
	}
	kprintf("\n");
}
