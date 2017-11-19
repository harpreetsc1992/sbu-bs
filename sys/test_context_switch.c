#include <sys/process.h>

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
    return;}

void
thread1(
       )
{
    kprintf("Inside thread 1\n");
    dispatch(&(processes->c_t), &(processes->next->c_t)/*processes->kstack[sz - 1], processes->next->kstack[sz - 1]*/);
}

int
fn_call(
       )
{
    create_pcb(thread1);
    create_pcb(thread2);
//  create_pcb(thread3);
    thread1();
    return 0;
}
