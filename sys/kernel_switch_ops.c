#include <sys/kern.h>
#include <sys/process.h>
#include <sys/page_alloc.h>

void 
load_krsp(
		 )
{
	__asm__(
            "movq %0, %%rsp\t\n"
			"movq %1, %%cr3\t\n"
            ::"r"((struct PCB *)processes->c_t.rsp), "b"(global_pml4 - VIRT_BASE)
           );
}


