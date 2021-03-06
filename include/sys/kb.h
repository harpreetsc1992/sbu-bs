#ifndef _KB_H
#define _KB_H

#include <sys/defs.h>
#include <sys/idt.h>
#include <sys/putcharacter.h>
#include <sys/io_ports.h>
#include <sys/kern_ops.h>
#include <sys/page_alloc.h>

extern unsigned char kbdus[128];
extern unsigned char kbdshift[128];
extern unsigned char kbdcaps[128];
extern char *kbd_buf;
extern int flag;
extern uint64_t* gets_at;
extern uint16_t counter;

extern void irq1();

typedef enum
{
	NO,
	CTRL,
	SHIFT,
	ALT,
	CAPS
}last_key_pressed;

extern last_key_pressed lkp;

struct interrupt_frame
{
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
};

void
init_kbd(
		);

void
keyboard_handler(
                 regs_t r
                );

void
keyboard_install(
                );

#endif
