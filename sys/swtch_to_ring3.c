#include <sys/swtch_rings.h>
#include <sys/process.h>
#include <sys/init.h>

void 
jmp_usrmode(
			uint64_t function
		   )
{
   __asm__ volatile (
	"cli\t\n"
	"mov $0x23, %%rax\t\n"
    "mov %%rax, %%ds\t\n" 
   	"mov %%rax, %%es\t\n" 
   	"mov %%rax, %%ds\t\n" 
   	"mov %%rax, %%gs\t\n" 

    "mov %%rsp, %%rax\t\n" 
    "push $0x23\t\n" 
    "push %%rax\t\n" 
    "pushf\t\n" 
    "push $0x2B\t\n"
    "push %0\t\n"
	"sti\t\n"
    "iretq\t\n"
	:: "r"(function)
	); 
}

void
initiate_jmp(
			)
{
	create_usr_pcb(curr_file);
	yield();

	return;
}

void
tst_usr_fn(
		  )
{
	kprintf("I shouldn't be able to print this BS!!!\n");
	return;
}

