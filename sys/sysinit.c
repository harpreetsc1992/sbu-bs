#include <sys/init.h>
#include <sys/swtch_rings.h>
#include <sys/gdt.h>
#include <sys/process.h>

char curr_file[NAMEMAX];

void
enter_usermode(
			   uint64_t user_entry,
			   uint64_t stack
			  )
{
	clear_screen();
    jmp_usrmode(user_entry, stack);
}

void
init_entry(
		  )
{
	kmemcpy((char *)curr_file, (void *)"bin/sbush", kstrlen("bin/sbush"));
	struct task_struct *pcb = (struct task_struct *)create_usr_pcb(curr_file);
	*((uint64_t *)((uint64_t)pcb->stack - 0x8)) = 0;
	*((uint64_t *)((uint64_t)pcb->stack - 0x10)) = 0;
	*((uint64_t *)((uint64_t)pcb->stack - 0x18)) = 0;
    uint64_t user_entry = pcb->entry;
	set_tss_rsp((uint64_t *)processes->c_t.rsp);
    enter_usermode(user_entry, (uint64_t)pcb->stack);
}

void
idle(
	)
{
	while(1)
	{
		yield();
	}
}
