#include <sys/process.h>

int sz = STACK_SIZE;
struct PCB*
init_pcb(
		 void (*t)()
		)
{
	struct PCB *pcb;
	pcb = (struct PCB *)page_alloc();

	pcb->kstack[sz - 1] = (uint64_t)t;
	pcb->c_t.rip = pcb->kstack[sz - 1];
	pcb->pid = p_id++;
	pcb->c_t.rsp = (uint64_t)&pcb->kstack[sz - 2];
	pcb->proc_state = READY;
	pcb->exit_status = 0;

	return pcb;
}

int
create_pcb(
		   void (*t)()
		  )
{
	struct PCB *pcb = NULL;

	pcb = init_pcb((void *)t);
	if(pcb == NULL)
	{
		return 0;
	}
	ready_procs ++;
	add_to_ready_list(pcb);

	return 1;
}
