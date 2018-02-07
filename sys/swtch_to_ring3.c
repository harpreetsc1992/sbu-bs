#include <sys/swtch_rings.h>
#include <sys/process.h>

void 
jmp_usrmode(
		   )
{
   __asm__ __volatile__(
	"cli\t\n"
	"mov $0x23, %rax\t\n"
    "mov %rax, %ds\t\n" 
   	"mov %rax, %es\t\n" 
   	"mov %rax, %ds\t\n" 
   	"mov %rax, %gs\t\n" 

    "mov %rsp, %rax\t\n" 
    "push $0x23\t\n" 
    "push %rax\t\n" 
    "pushf\t\n" 
    "push $0x2B\t\n" 
    "push $tst_usr_fn\t\n"
    "iretq\t\n"); 
}

void
initiate_jmp(
			)
{
	set_user_pages();
	create_usr_pcb("bin/sbush");
	
	jmp_usrmode();
	return;
}

void
tst_usr_fn(
		  )
{
	kprintf("I shouldn't be able to print this BS!!!\n");
	return;
}

