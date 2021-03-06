#include <sys/process.h>

uint16_t proc_count_in_list = 0, idx_pcb = 0;

void
move_to_end(
            struct PCB *pcb
           )
{
    struct PCB *head = processes;

    if (pcb->proc_state != READY)
    {
        pcb->proc_state = READY;
    }

    if (processes == processes->next)
    {
        return;
    }

    while(processes->next != head)
    {
        processes = processes->next;
    }

    processes->next = pcb;

    processes = processes->next;
    processes/*->next*/ = head;

    head = NULL;
}

void
add_to_ready_list(
                  struct PCB *pcb
                 )
{
    proc_count_in_list++;
    if (1 == proc_count_in_list)
    {
        processes = pcb;
        processes->next = processes;
    }
    else
    {
        processes->next = pcb;
    }
}

void
add_to_ready_list_user(
                       struct task_struct *pcb
                      )
{
	uint16_t uproc_count_list = process_count;
    if (1 == uproc_count_list)
    {
        ready_queue[uproc_count_list - 1] = *pcb;
		curr_upcb = &ready_queue[uproc_count_list - 1];
    }
    else
    {
        ready_queue[uproc_count_list - 1] = *pcb;
    }
}

void
move_to_next_pcb(
				)
{
	if ((process_count != 0) && (ready_queue[idx_pcb].pid != ready_queue[idx_pcb + 1].pid))
    {
        ready_queue[idx_pcb] = ready_queue[idx_pcb + 1];
		idx_pcb++;
		while (ready_queue[idx_pcb].state != READY) idx_pcb++;
		if (ready_queue[idx_pcb].pid != 0)	curr_upcb = &ready_queue[idx_pcb];
#ifndef SCHEDULE_POLICY
    process_count--;
#endif
    }
    else
    {
        kprintf("Error: No processes to move to\n");
        return;
    }	
}

void
move_to_next(
            )
{
    if ((processes->next != NULL) && (processes->next != processes))
    {
        processes = processes->next;
#ifndef SCHEDULE_POLICY
    ready_procs--;
#endif
    }
    else
    {
        kprintf("Error: No processes to move to\n");
        return;
    }
}
