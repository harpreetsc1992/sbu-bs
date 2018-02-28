#ifndef _FAULT_H
#define _FAULT_H

#include <sys/page_alloc.h>
#include <sys/process.h>
#include <sys/k_utils.h>
#include <sys/tarfs.h>
#include <sys/kern_ops.h>
#include <utils.h>

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
		set_kernel_page_user((uint64_t)ROUNDDOWN(addr, PAGE_SIZE));
	}

	if (error_no == 0b001)
	{	
		/*
		 * Read Access not occuring on present page in Kernel mode
		 */
		kprintf("Kernel Panic\n");
		kshutdown();
	}
	if (error_no == 0b010)
	{
		/*
		 * Write Access not occuring in a non-present page in Kernel mode
		 */
		uint64_t *pml = (uint64_t *)pml4_shared;
		uint64_t *pdp = (uint64_t *)get_pdp(ROUNDDOWN(addr, PAGE_SIZE), get_pml4_offset(ROUNDDOWN(addr, PAGE_SIZE)));
		uint64_t *pd = (uint64_t *)get_pd(ROUNDDOWN(addr, PAGE_SIZE), get_pdp_offset(ROUNDDOWN(addr, PAGE_SIZE)));
		uint64_t *pt = (uint64_t *)get_pt(ROUNDDOWN(addr, PAGE_SIZE), get_pml4_offset(ROUNDDOWN(addr, PAGE_SIZE)));
		*(pt + get_pt_offset(ROUNDDOWN(addr, PAGE_SIZE))) = ((*(pt + get_pt_offset(ROUNDDOWN(addr, PAGE_SIZE))) >> 12) << 12) | KERN_PERM_BITS;

		*(pd + get_pd_offset(addr)) = ((uint64_t)pt - VIRT_BASE);
		*(pdp + get_pdp_offset(addr)) = ((uint64_t)pd - VIRT_BASE);
    	*(pml + get_pml4_offset(addr)) = ((uint64_t)pdp - VIRT_BASE);

		pdp_shared = (uint64_t)pdp;
		pd_shared = (uint64_t)pd;
		pt_shared = (uint64_t)pt;
		tlb_flush(pml4_shared - VIRT_BASE);
	}
	if (error_no == 0b011)
    {
		/*
		 * Kernel faults while writing to a present page
		 */
		kprintf("Kernel Panic\n");
		kshutdown();
	}
	if (error_no == 0b100)
	{
		/*
		 * User not being able to read an unampped page
		 */
		memmap((void *)addr, PAGE_SIZE, COW_PERM_BITS, ACCESSED_NOT_PRESENT);
	}
	if (error_no == 0b101)
	{
		/*
		 * The User is not able to read an unmapped page
		 */
		kprintf("Segfault\n");
		exit_process(1);
	}
	if (error_no == 0b110)
	{	
		/*
		 * User not able to write to a page that is not present
		 */
		memmap((void *)addr, PAGE_SIZE, USR_PERM_BITS, ACCESSED_NOT_PRESENT);
	}
	if (error_no == 0b111)
	{
		/*
		 * User cannot write to a page that is present; COW
		 */
		memmap((void *)addr, PAGE_SIZE, USR_PERM_BITS, SET_COW_RW);
	}
}

#endif
