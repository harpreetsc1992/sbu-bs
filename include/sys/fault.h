#ifndef _FAULT_H
#define _FAULT_H

#include <sys/page_alloc.h>
#include <sys/process.h>

static void
page_fault_handler(
				   regs_t reg
				  )
{
    uint64_t addr;
    struct user_page *upg;
    uint8_t seg_fault = 0;
    __asm__ __volatile__(
            "movq %%cr2, %0\t\n"
            : "=r" (addr)
            );

	uint64_t kernbase = mem_data.physbase;
	uint64_t kernfree = mem_data.physfree;
	uint64_t pg_sz     = PAGE_SIZE;

    if ((addr >= kernbase) && (addr < kernfree))
    {
        kprintf("Kernel Address Accessed. Exiting\n");
        kshutdown();
    }

    upg = (struct user_page *) addr;

    if ((upg->pd_rw == READONLY_COW) || (((uint64_t)addr & USR_PERM_BITS) != COW_PERM_BITS))
    {
        uint64_t *new_addr = get_new_page();
        kmemcpy((void *)new_addr, (void *)addr, kstrlen((char *)addr));
        new_addr = (uint64_t *)((uint64_t)new_addr | USR_PERM_BITS);
    }
    else if (upg->pd_free == FREE)
    {
        struct vm_area_struct *vma_ptr = uprocs->procs->mm->mmap;
        uint64_t start, end;
        while (vma_ptr != NULL)
        {
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
        seg_fault = 1;
    }

    if (seg_fault)
    {
        kshutdown();
    }
}

#endif
