#ifndef _TIMER_H
#define _TIMER_H

#include <sys/isr.h>
#include <sys/io_ports.h>

#define MH_FREQ 1193180

#define		I86_PIT_OCW_MASK_BINCOUNT			1	//00000001
#define		I86_PIT_OCW_MASK_MODE				0xE	//00001110
#define		I86_PIT_OCW_MASK_RL				0x30	//00110000
#define		I86_PIT_OCW_MASK_COUNTER			0xC0	//11000000

#define		I86_PIT_OCW_BINCOUNT_BINARY	0	//0		//! Use when setting I86_PIT_OCW_MASK_BINCOUNT
#define		I86_PIT_OCW_BINCOUNT_BCD	1	//1
#define		I86_PIT_OCW_MODE_TERMINALCOUNT	0	//0000		//! Use when setting I86_PIT_OCW_MASK_MODE
#define		I86_PIT_OCW_MODE_ONESHOT	0x2	//0010
#define		I86_PIT_OCW_MODE_RATEGEN	0x4	//0100
#define		I86_PIT_OCW_MODE_SQUAREWAVEGEN	0x6	//0110
#define		I86_PIT_OCW_MODE_SOFTWARETRIG	0x8	//1000
#define		I86_PIT_OCW_MODE_HARDWARETRIG	0xA	//1010
#define		I86_PIT_OCW_RL_LATCH		0	//000000	//! Use when setting I86_PIT_OCW_MASK_RL
#define		I86_PIT_OCW_RL_LSBONLY		0x10	//010000
#define		I86_PIT_OCW_RL_MSBONLY		0x20	//100000
#define		I86_PIT_OCW_RL_DATA		0x30	//110000
#define		I86_PIT_OCW_COUNTER_0		0	//00000000	//! Use when setting I86_PIT_OCW_MASK_COUNTER
#define		I86_PIT_OCW_COUNTER_1		0x40	//01000000
#define		I86_PIT_OCW_COUNTER_2		0x80	//10000000

#define		I86_PIT_REG_COUNTER0		0x40
#define		I86_PIT_REG_COUNTER1		0x41
#define		I86_PIT_REG_COUNTER2		0x42
#define		I86_PIT_REG_COMMAND		0x43

extern __volatile__ uint32_t tick;
extern uint64_t sleep_time;

void
timer_callback(
              );

void
init_timer(
          );

void
sleep_shell(
            uint64_t secs
           );
#endif
