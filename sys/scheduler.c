#include <sys/process.h>
#include <sys/init.h>
#include <sys/swtch_rings.h>

void
usr_schedule(
			)
{
	uint32_t i = process_count, j = 0;
	while ((curr_upcb->ppid != ready_queue[i - 1].pid) && (ready_queue[i - 1].pid != 0))
	{
		if (i == process_count - 1)
		{
			break;
		}
		if ((i % process_count) == 0)
			i = 1;
		else i++;
	}
	if (i == process_count - 1)
	{
		while ((curr_upcb->pid != ready_queue[j].ppid) && (ready_queue[j].pid != 0))
		{
			j++;
		}
	}
	i -= 1;
	if (curr_upcb->ppid == ready_queue[i].pid)
	{
		curr_upcb = &ready_queue[i];
		tlb_flush(curr_upcb->cr3);
		pml4_shared = curr_upcb->pml4e;
		load_segment(curr_upcb, curr_upcb->pname, 0);
		memmap(curr_upcb->stack, PAGE_SIZE, USR_PERM_BITS, GROWS_DOWN);
//		if (curr_upcb->pid == 1)
//			kprintf("sbush> ");
		jmp_usrmode(curr_upcb->entry, curr_upcb->stack);
	}
	else if (curr_upcb->pid == ready_queue[j].ppid)
	{
		curr_upcb = &ready_queue[j];
		tlb_flush(curr_upcb->cr3);
		pml4_shared = curr_upcb->pml4e;
		load_segment(curr_upcb, curr_upcb->pname, 0);
		memmap(curr_upcb->stack, PAGE_SIZE, USR_PERM_BITS, GROWS_DOWN);
//		if (curr_upcb->pid == 1)
//			kprintf("sbush> ");
		jmp_usrmode(curr_upcb->entry, curr_upcb->stack);
	}
	else
	{
		idle();
	}
}

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
			kprintf("sbush> ");
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
