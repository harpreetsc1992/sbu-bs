#ifndef _INIT_H
#define _INIT_H

#include <sys/defs.h>
#include <sys/page_alloc.h>

extern struct task_struct *shell_pcb;
extern uint64_t shell_entry;

void
enter_usermode(
               uint64_t user_entry,
			   uint64_t stack
              );

void
idle(
	);

void
init(
	);

void
init_entry(
		  );

#endif
