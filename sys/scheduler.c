#include <sys/processes.h>

schedule(
		 struct PCB *curr,
		 struct PCB *next
		)
{
	old = NULL;
	
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

	move_to_end(&old);

	dispatch(&old->proc_state, &curr->proc_state);
}

rr_scheduler(
			)
{
	schedule();
	old->proc_state = READY;
	
	/* 
	 * DEAD when process is over
	 */
	old->proc_state = DEAD; 
}
