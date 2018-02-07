#ifndef _ISR_H
#define _ISR_H

#include <sys/defs.h>

struct stack_frame
{
	uint64_t rip;	
	uint64_t cs;
	uint64_t rflags;
	uint64_t rsp;
	uint64_t ss;
};

#endif
