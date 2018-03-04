#ifndef _SWTCH_RINGS_H
#define _SWTCH_RINGS_H

#include <sys/defs.h>
#include <sys/kprintf.h>
#include <sys/page_alloc.h>

extern void jmp_usrmode();
extern unsigned short first_boot;

void
initiate_jmp(
			);

void
tst_usr_fn(
		  );

#endif
