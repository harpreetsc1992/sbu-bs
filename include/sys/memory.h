#include <sys/defs.h>
#include <sys/page_alloc.h>

void
*memset(
        void *,
        int,
        int
       );

void 
*kmalloc(
		 uint64_t sz
		);
