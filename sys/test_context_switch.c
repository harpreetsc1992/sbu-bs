#include <sys/process.h>
#include <sys/swtch_rings.h>
#include <sys/gdt.h>
/*
void
thread2(
       )
{
    kprintf("Inside thread 2\n");
	initiate_jmp();
	while (1)	;
    return;
}

void
thread1(
       )
{
    kprintf("Inside thread 1\n");
	return;
}

int
fn_call(
       )
{
    create_pcb(thread1);
    create_pcb(thread2);
    yield();
    return 0;
}
*/
