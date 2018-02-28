#include <sys/process.h>
#include <sys/init.h>
#include <sys/swtch_rings.h>

int
schedule(
		 struct PCB *curr,
		 struct PCB *next
		)
{
	struct PCB *old = NULL;
	
	/*
	 * If next process is not ready, do not schedule it
	 */
	if (next->proc_state != READY)
	{
		return 0;
	}

	/*
	 * Don't schedule oneself
	 */
	if (next == curr)
	{
		return 0;
	}
	
	old = curr;
	next->proc_state = RUNNING;
	curr = next;

	old->proc_state = READY;

	dispatch(&old->c_t, &curr->c_t);
	return 1;
}

void
yield(
	 )
{
	if (ready_procs > 1)
	{
		if (!schedule(processes, processes->next))
		{
			tlb_flush(shell_pcb->cr3);
			pml4_shared = shell_pcb->pml4e;
			load_segment(shell_pcb, "bin/sbush", 0);
			memmap(shell_pcb->stack, PAGE_SIZE, USR_PERM_BITS, GROWS_DOWN);
			*((uint64_t *)((uint64_t)shell_pcb->stack - 0x8)) = 0;
    		*((uint64_t *)((uint64_t)shell_pcb->stack - 0x10)) = 0;
    		*((uint64_t *)((uint64_t)shell_pcb->stack - 0x18)) = 0;
			jmp_usrmode(shell_pcb->entry, shell_pcb->stack);
		}
	
		move_to_next();
	}

	else
	{
		
	}

	/* 
	 * DEAD when process is over
	 */
//	old->proc_state = DEAD; 
}
