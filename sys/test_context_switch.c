#include <sys/process.h>
#include <sys/swtch_rings.h>
#include <sys/gdt.h>

/*
void
thread3(
       )
{
    kprintf("Inside thread 3\n");
    return;
}
*/
void
thread2(
       )
{
    kprintf("Inside thread 2\n");
	set_tss_rsp((uint64_t *)processes->next->c_t.rsp);
	initiate_jmp();
	while (1)	;
    return;
}

void
thread1(
       )
{
    kprintf("Inside thread 1\n");
//  dispatch(&(processes->c_t), &(processes->next->c_t)/*processes->kstack[sz - 1], processes->next->kstack[sz - 1]*/);
	return;
}

int
fn_call(
       )
{
    create_pcb(thread1);
    create_pcb(thread2);
//  create_pcb(thread3);
    yield();
    return 0;
}
