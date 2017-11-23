#include <sys/process.h>

//struct PCB *last;
//int flag_for_last;

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

	old->proc_state = DEAD;
	ready_procs--;
	proc_count_in_list--;
//	move_to_end(old);
//	if (!flag_for_last)
//	{
//		last = old;
//		flag_for_last = 1;
//	}

	dispatch(&old->c_t, &curr->c_t/*, &last*/);

//	if (flag_for_last)
//	{
//		last = old;
//	}

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
			kprintf("Couldn't schedule the next process\n");
		}

		remove_from_list();
	
		move_to_next();
	}

	else
	{
		kprintf("No more processes to run");
	}

	/* 
	 * DEAD when process is over
	 */
//	old->proc_state = DEAD; 
}
