#include <sys/process.h>
#include <sys/init.h>

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

//	move_to_end(old);

	dispatch(&old->c_t, &curr->c_t);
	//fn_call();
	//thread1();
	return 1;
}

void
yield(
	 )
{
	//fn_call();

	if (ready_procs > 1)
	{
		if (!schedule(processes, processes->next))
		{
			kprintf("Couldn't schedule the next process\n");
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
