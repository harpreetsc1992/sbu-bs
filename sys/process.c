#include <sys/tarfs.h>
#include <sys/timer.h>

uint32_t child_pid;
uint16_t p_id, ready_procs;
uint16_t upid;
uint64_t ready_queue[NUM_PROCS];
uint16_t process_count;

struct vm_area_struct *
malloc_vma(
		   struct mm_struct *mm
		  )
{
	struct vm_area_struct *vm;

	if (!mm->mmap)
	{
		vm = (struct vm_area_struct *) kmalloc(sizeof(struct vm_area_struct));
		mm->mmap = vm;
		mm->count++;
	
		return vm;
	}
	else
	{
		vm = mm->mmap;
		
		while (NULL != vm->vm_next)
		{
			vm = vm->vm_next;
		}
//		uint64_t *addr = (uint64_t *)ROUNDDOWN((void *)(vm + 1), PAGE_SIZE);
		
//		while ((uint64_t)addr <= (uint64_t)(vm + 1))
//		{
//			addr = (uint64_t *) get_usr_page(PAGE_SIZE);
//		}
		vm->vm_next = (struct vm_area_struct *)(vm + 1);
		mm->count++;

		return vm->vm_next;
	}
}


void 
allocate_regions(
				 void *va, 
				 size_t len	
				) 
{
	uint64_t *va_start = (uint64_t *)ROUNDDOWN(va, PAGE_SIZE);
	uint64_t *va_end = (uint64_t *)ROUNDUP((char *)va + len, PAGE_SIZE);

	uint32_t sz = (uint32_t)((uint64_t)va_end - (uint64_t)va_start);
	int num = sz/PAGE_SIZE;

	for(int i = 0; i < num; i++)
	{
		add_user_page(va + i * PAGE_SIZE, USR_PERM_BITS);
	}
}

void
load_segment(
			 struct task_struct *pcb,
			 char *file,
			 size_t sz
			)
{
	int offset = file_exists(file);
	
	if (offset == 0 || offset == 999)
	{
		offset = 0;
    }
	else
	{
		Elf64_Ehdr *elf = (Elf64_Ehdr *)(&_binary_tarfs_start + offset);
		Elf64_Phdr *ph;
		
		ph = (Elf64_Phdr *)((uint64_t)elf + elf->e_phoff);
		int i;
		
		for (i = 0; i < elf->e_phnum; i++)
		{
			ph = (Elf64_Phdr *)(((uint64_t)elf + elf->e_phoff) + i);
			if (ph->p_type != ELF_PROG_LOAD)
			{
				continue;
			}
			else
			{
				if (ph->p_filesz > ph->p_memsz)
				{
					kprintf("Invalid file size\n");
				}
		
				allocate_regions((void *)ph->p_vaddr, ph->p_memsz);
				kmemcpy((char*) ph->p_vaddr, (void *) ((uint64_t)elf + (uint64_t)ph->p_offset), ph->p_filesz);
		
				if (ph->p_filesz < ph->p_memsz)
				{
					memset((char*) ph->p_vaddr + ph->p_filesz, 0, ph->p_memsz - ph->p_filesz);
				}
		
		        // Switch back to kernel's address space
				struct vm_area_struct *vm;
				vm = malloc_vma(pcb->mm);
				vm->vm_start = ph->p_vaddr;
				vm->vm_end = vm->vm_start + ph->p_memsz;
				vm->vm_mmsz = ph->p_memsz;
				vm->vm_next = NULL;
				vm->vm_file = (uint64_t)elf;
				vm->vm_flags = ph->p_flags;
				vm->vm_offset = ph->p_offset;  
		    }
		}
		pcb->entry = elf->e_entry;
		pcb->heap_vma = (struct vm_area_struct *)get_usr_page(PAGE_SIZE);
		
		struct vm_area_struct *tmp = pcb->mm->mmap;
		
		while(tmp->vm_next != NULL)
		{
			tmp = tmp->vm_next;  // go to the last vma
		}
		
		pcb->heap_vma->vm_start = pcb->heap_vma->vm_end = ALIGN_DOWN((uint64_t)(tmp->vm_end + PAGE_SIZE));
		pcb->heap_vma->vm_mmsz = PAGE_SIZE;
		
		allocate_regions((void *)pcb->heap_vma->vm_start, pcb->heap_vma->vm_mmsz);
	}
}

uint64_t
get_pid(
	   )
{
	return (uint64_t)curr_upcb->pid;
}

uint64_t
get_ppid(
		)
{
	return (uint64_t)curr_upcb->ppid;
}

int
fork_process(
			)
{
	struct task_struct *parent_pcb = curr_upcb;
	struct task_struct *child_pcb = (struct task_struct *)kmalloc(sizeof(struct task_struct));
	parent_pcb = (struct task_struct *)((uint64_t)parent_pcb & COW_PERM_BITS);
	child_pcb = (struct task_struct *)((uint64_t)child_pcb & COW_PERM_BITS);
	if(NULL == child_pcb)
	{
		kprintf("Out of Memory\n");
		return -1;
	}
	
	child_pcb->mm = (struct mm_struct *)kmalloc(sizeof(struct task_struct));
	child_pcb->mm->count = 0;
	child_pcb->mm->mmap = NULL;
	
	//fill the entry of pcb
	child_pcb->pid = ++upid;
	child_pid = child_pcb->pid;
	child_pcb->ppid = parent_pcb->pid;
	
	child_pcb->state = READY;

	++process_count;
	add_to_ready_list_user(child_pcb);
	
	kmemcpy(child_pcb->pname, parent_pcb->pname, kstrlen(parent_pcb->pname));        
	
	child_pcb->pml4e = (uint64_t)global_pml4;
	
	child_pcb->stack = get_usr_page(PAGE_SIZE);
	child_pcb->cr3 = pml4_shared - VIRT_BASE;
	tlb_flush(child_pcb->cr3);
	child_pcb->stack=parent_pcb->stack;
	
	child_pcb->kstack[511] = 0x23; //ss
	child_pcb->kstack[510] = child_pcb->stack[511]; //rsp
	
	child_pcb->kstack[509] = 0x200286; //rflags
	child_pcb->kstack[508] = 0x2b; //cs
	child_pcb->kstack[507] = parent_pcb->kstack[505];  //rip entry point scroll down to see the value
	
	child_pcb->kstack[491] = (uint64_t)(&irq0 + 34);
	child_pcb->kstack[490] = 16; //for extra rbx
	
	child_pcb->kstack[506] = 0; //rax return to child

	int x = 505, y = 503;
    while (x >= 492)
    {
        child_pcb->kstack[x--] = parent_pcb->kstack[y--];
    }

	child_pcb->ctx.rsp = (uint64_t)&(child_pcb->kstack[490]);
	
	struct vm_area_struct* parent_vm = parent_pcb->mm->mmap;
	struct vm_area_struct* child_vm;
	while(NULL != parent_vm)
	{
		child_vm = malloc_vma(child_pcb->mm);
		child_vm->vm_start = parent_vm->vm_start;
		child_vm->vm_end = parent_vm->vm_end;
		child_vm->vm_mmsz = parent_vm->vm_mmsz;
		child_vm->vm_next = NULL;
		child_vm->vm_file = parent_vm->vm_file;
		child_vm->vm_flags = parent_vm->vm_flags;
		child_vm->vm_offset = parent_vm->vm_offset;
//		allocate_regions(child_pcb, (void*) parent_vm->vm_start, parent_vm->vm_mmsz);
	    
	    for(int j = 0; j < parent_vm->vm_mmsz; j++)
	    {
	        *((uint64_t *)(parent_vm->vm_start + j)) = *((uint64_t *)((uint64_t)parent_vm->vm_file + parent_vm->vm_offset + j));
	    }
	    parent_vm = parent_vm->vm_next;
	}

	child_pcb->heap_vma = (struct vm_area_struct *)get_usr_page(PAGE_SIZE);

	struct vm_area_struct *tmp = child_pcb->mm->mmap;
	while(tmp->vm_next != NULL)
	{
		tmp = tmp->vm_next;		// go to the last vma
	}
	
	child_pcb->heap_vma->vm_start = child_pcb->heap_vma->vm_end = ALIGN_DOWN((uint64_t)(tmp->vm_end + PAGE_SIZE));

	child_pcb->heap_vma->vm_mmsz = PAGE_SIZE;
//	allocate_regions(child_pcb, (void *)child_pcb->heap_vma->vm_start, child_pcb->heap_vma->vm_mmsz); 
	
	/* switch back to caller address space before we return */
	tlb_flush((uint64_t)parent_pcb->cr3);
	
	return child_pid;
}

void
exit_process(
	 		 int state
			)  
{
	curr_upcb->state = ZOMBIE;  
	zombie_queue[curr_upcb->pid].exit_status = state;

  	// now update the ready_queue and process_count
  	// we are sure that current running process is "next" 

	int i = 1;         
	while (i <= process_count)  //find the pcb in ready_queue 
    { 
		if (ready_queue[i] == (uint64_t)curr_upcb)
		{ 
			break;
		}
		i++; 
    }
           
    if (i < process_count)  // update the ready_queue 
    {
		while ((i + 1) <= process_count)
		{
			ready_queue[i] =  ready_queue[i+1];
			i++;
		}
    }

	process_count--;

//	TODO: free(curr_upcb);
 
	/* 
	 * Load kernel's rsp and cr3 register 
	 */  
	load_krsp();

	__asm__ __volatile__(
			"pop %rbx\t\n"
			);  // extra rbx pop 
	__asm__ __volatile__(
			"pop %rax\t\n"
			);  // get the isr addr
	__asm__ __volatile__(
			"jmp *%rax\t\n"
			); 
} 

uint64_t
_sleep(
	  )
{
	struct task_struct *pcb = curr_upcb;   
	// save current rsp
	__asm__(
			"movq %%rsp, %0\t\n"
			:"=g"((uint64_t)pcb->ctx.rsp)
		   );
  
	/*
	 * Load kernel's rsp & cr3
	 */
	load_krsp();

	__asm__ __volatile__(
			"pop %rbx\t\n"
			);  // extra reg schedule;
	__asm__ __volatile__(
			"pop %rax\t\n"
			);  // isr next instruction
	__asm__ __volatile__(
			"jmp *%rax\t\n"
			);

	return tick; 
}

uint64_t
sleep_proc(
	  	   uint64_t time
	 	  )
{
	struct task_struct *pcb = curr_upcb;
	pcb->sleep_time = time;
	// TODO: save current process rsp
	return _sleep();
}

void*
malloc(
	   uint64_t size
	  )
{
	struct task_struct *pcb = curr_upcb;
	uint64_t old = pcb->heap_vma->vm_end;
	pcb->heap_vma->vm_end = pcb->heap_vma->vm_end + size;   

	if (pcb->heap_vma->vm_end - pcb->heap_vma->vm_start > PAGE_SIZE)
	{
		return NULL;
	}

	return (void *)old;   
}

uint64_t
exec(
	 char *arg1,
	 char **arg2,
	 char **arg3
	)
{
	/*************** use kmalloc to access it further when u switch the stack  *******/
	char *tmp_pathname = arg1;
	//char **tmp_argument = (char **)arg2;
	//char **tmp_env = (char **)arg3;
	char pathname[1024];
	kstrcpy(pathname, tmp_pathname);

	struct task_struct *pcb = (struct task_struct *)create_usr_pcb(pathname);
	
	struct task_struct *current_pcb = curr_upcb;
	
	pcb->pid = current_pcb->pid;
	pcb->ppid = current_pcb->ppid;
	//TODO: correct execvpe	exit(0);
	
	return 0;
}

int
syswaitpid(
		   int pid, 
		   int status
	   	  )
{
	struct task_struct *pcb = curr_upcb;
	struct task_struct *child_pcb;
	uint64_t ppid = pcb->pid;
	int i = 1;
	child_pcb = (struct task_struct *)ready_queue[i];
	if (pid < 0) //find any child and check for the status
	{
		for (i = 1; i <= process_count; i++)
		{
			child_pcb = (struct task_struct *)ready_queue[i];
			if(child_pcb->ppid == ppid )
			{
			    pcb->wait_pid = child_pcb->pid;
			    break;
			}
		}
		
		if(i > process_count) //no child process found go and resume execution
		{
			status = 0;
			return 0;
		}
		else if(ZOMBIE == child_pcb->state) // child is already dead
		{
			status = zombie_queue[child_pcb->pid].exit_status;
			return status;
		}
		else
		{
			pcb->state = WAIT;
			pcb->wait_pid = child_pcb->pid;
			__asm__ __volatile__(
					"int $0x81\t\n"
					); 			//yield
			return 0;  					//just return it won't schedule
		}
	}
	else
	{
		for (i = 1; i <= process_count; i++)
		{
			child_pcb = (struct task_struct *)ready_queue[i];
			if (child_pcb->pid == pid)	// found the child
		    {
				pcb->state = WAIT;
				pcb->wait_pid = child_pcb->pid;
		    	break;
			}
		}
		if (i > process_count) 			//no child process found go and resume execution
		{
			status = 0;
			pcb->wait_pid = 0;			// remove the wait
			pcb->state = READY;
			return 0;
		}
		else if (ZOMBIE == child_pcb->state) // child is already dead
		{
			status = zombie_queue[child_pcb->pid].exit_status;
			pcb->wait_pid = 0; 			// remove the wait
			pcb->state = READY;
			return status;
		}
		else
		{
			pcb->state = WAIT;
		    pcb->wait_pid = child_pcb->pid;
		    __asm__ __volatile__(
					"int $0x81\t\n"
					); 			//yield
		    return 0;					//just return it won't schedule
		}
	}
}

void
dp(
  )
{
	struct task_struct *pcb;
	int i;
	for(i = 1; i <= process_count; i++)
	{
		pcb = (struct task_struct *)ready_queue[i];
		kprintf("%p\n", pcb);
	} 
}

void
kshutdown(
		 )
{
	// Load kernel's rsp and cr3
	load_krsp();
	int i = 1;

    uint64_t num = process_count;

	while(num--) 
	{
		if(i < process_count)
		{
			while((i + 1) <= process_count)
			{
				ready_queue[i] = ready_queue[i + 1];
				i++;
			}
		}

		process_count = 0;
	}

	//init();
}
