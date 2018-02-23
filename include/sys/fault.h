#ifndef _FAULT_H
#define _FAULT_H

#include <sys/page_alloc.h>
#include <sys/process.h>
#include <sys/k_utils.h>
#include <sys/tarfs.h>

static void
page_fault_handler(
				   regs_t reg
				  )
{
    uint64_t addr;
	uint64_t error_no = reg.err_no; 
    __asm__ __volatile__(
            "movq %%cr2, %0\t\n"
            : "=r" (addr)
            );

	uint64_t kernbase = (uint64_t)(VIRT_BASE + mem_data.physbase);
	uint64_t kernfree = (uint64_t)(VIRT_BASE + mem_data.physfree);

    if ((addr >= kernbase) && (addr < kernfree))
    {
        kprintf("Kernel Address Accessed. Exiting\n");
        kshutdown();
    }

	if (addr > VIRT_BASE && error_no == 0x7)
	{
//		set_kernel_page_user(addr);
	}

	else if(error_no & 0x1)
	{	
		/*
		 * Page is present; No need to add a new page as of now.
		 */
		if ((error_no & 0x3) == 0x3)
    	{
			/*
			 * The Page is present, but there is a write protection fault
			 */
			if ((error_no & 0x7) == 0x7)
			{
				/*
				 * User Mode trying to access Kernel; Not permitted.
				 */
				kprintf("Can't access Kernel Page\n");
			}
			else
			{
				/*
				 * Kernel Mode caused page fault; Kernel panic; Shutdown.
				 */
				kshutdown();
			}
		}
		else if ((error_no & 0x5) == 0x5)
		{
			/*
			 * Write access not permitted on this COW page. Give a different page.
			 */
			uint64_t *new_addr = (uint64_t *) memmap((void *)addr, PAGE_SIZE, USR_PERM_BITS, SET_COW_RW);
        	kmemcpy((void *)new_addr, (void *)addr, kstrlen((char *)addr));
		}
		else
		{
			kprintf("Kernel Panic\n");
			kshutdown();
		}
	}
	else
	{
		struct vm_area_struct *vma_ptr = curr_upcb->mm->mmap;
        uint64_t start, end;
        while (vma_ptr != NULL)
        {
			dif_ctxt = 0;
           	start = vma_ptr->vm_start;
           	end = vma_ptr->vm_end;
           	if (addr >= start && addr < end)
           	{
				memmap((void *) addr, PAGE_SIZE, USR_PERM_BITS, ACCESSED_NOT_PRESENT);
           	}
           	vma_ptr = vma_ptr->vm_next;
		}	
	}
	/*
	        uint64_t *new_addr = get_new_page();
        	kmemcpy((void *)new_addr, (void *)addr, kstrlen((char *)addr));
        	new_addr = (uint64_t *)((uint64_t)new_addr | USR_PERM_BITS);
			
        	struct vm_area_struct *vma_ptr = curr_upcb->mm->mmap;
        	uint64_t start, end;
        	while (vma_ptr != NULL)
        	{
				dif_ctxt = 0;
            	start = vma_ptr->vm_start;
            	end = vma_ptr->vm_end;
            	if (addr >= start && addr < end)
            	{
                	add_user_page(upg, USR_PERM_BITS);
                	break;
            	}
            	vma_ptr = vma_ptr->vm_next;
        	}
        	if (vma_ptr == NULL)
        	{
            	seg_fault = 1;
        	}
	
		}
		else
		{
			char *tmp = vma_data(&sz);
			kmemcpy((char *)upg, (void *)tmp, sz);
		}
    }
    else if (((uint64_t)addr & USR_PERM_BITS) != COW_PERM_BITS)
    {
        uint64_t *new_addr = get_new_page();
        kmemcpy((void *)new_addr, (void *)addr, kstrlen((char *)addr));
        new_addr = (uint64_t *)((uint64_t)new_addr | USR_PERM_BITS);
    }    
	else
    {
        seg_fault = 1;
    }

    if (seg_fault)
    {
        kshutdown();
    }
	*/
}

#endif
