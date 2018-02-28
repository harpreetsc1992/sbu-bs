#include <utils.h>
#include <sys/process.h>
#include <sys/page_alloc.h>

int sz = STACK_SIZE;
int usr_sz = USR_STACK_SZ;

struct task_struct*
init_usr_pcb(
         	 char *file_name
        	)
{
    struct task_struct *pcb;
	pcb = (struct task_struct *) kmalloc(sizeof(struct task_struct));
	pcb->mm = (struct mm_struct *) kmalloc(sizeof(struct mm_struct));
	pcb->mm->count = 0;
	pcb->mm->mmap = NULL;

	if (upid == 0)	pcb->pid = ++upid;
	pcb->ppid = upid - 1;
	pcb->state = READY;

	kmemcpy(pcb->pname, file_name, kstrlen((char *)file_name));
	process_count++;
	add_to_ready_list_user(pcb);

	pcb->stack = memmap(NULL, PAGE_SIZE, USR_PERM_BITS, GROWS_DOWN);
	dif_ctxt = 0;

	pcb->pml4e = (uint64_t)pml4_shared;
	pcb->cr3 = (uint64_t)pml4_shared - VIRT_BASE;

	int x = 506, y = 1;
	while (x >= 492)
	{
		pcb->kstack[x--] = y++;
	}

	pcb->kstack[491] = (uint64_t)(&irq0 + 34);
	pcb->ctx.rsp = (uint64_t)&(pcb->kstack[490]);

	pcb->kstack[511] = 0x23;
    pcb->kstack[510]=(uint64_t)(&pcb->stack[511]);

    pcb->kstack[509] = 0x246;
    pcb->kstack[508] = 0x2b;

    load_segment(pcb, file_name, 0);
    pcb->kstack[507] = (uint64_t)pcb->entry;
	
    tlb_flush(pml4_shared - VIRT_BASE);

    return pcb;
}

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

uint64_t
create_usr_pcb(
			   char *file
          	  )
{
    struct task_struct *pcb = NULL;

    pcb = init_usr_pcb(file);
    if(pcb == NULL)
    {
        return 0;
    }

    return (uint64_t)pcb;
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
