#ifndef _INIT_H
#define _INIT_H

#include <sys/defs.h>
#include <sys/page_alloc.h>

void
enter_usermode(
               uint64_t user_entry
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
