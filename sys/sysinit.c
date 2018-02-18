#include <sys/init.h>
#include <sys/swtch_rings.h>
#include <sys/gdt.h>
#include <sys/process.h>

char curr_file[NAMEMAX];

void
enter_usermode(
			   uint64_t user_entry
			  )
{
    jmp_usrmode(user_entry);
}

void
init_entry(
		  )
{
	kmemcpy((char *)curr_file, (void *)"bin/init", kstrlen("bin/init"));
	struct task_struct *pcb = (struct task_struct *)create_usr_pcb(curr_file);
    uint64_t user_entry = pcb->entry;
	set_tss_rsp((uint64_t *)processes->c_t.rsp);
    enter_usermode(user_entry);
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
