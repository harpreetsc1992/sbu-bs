#include <sys/page_alloc.h>
#include <sys/kern_ops.h>
#include <sys/idt.h>

uint64_t phys_addr = 0x5000000;
uint64_t virt_base_for_user = VIRT_BASE;

void
tlb_flush(
		  uint64_t pt
		 )
{
	__asm__ __volatile__(
//	"mov %%rax, 8(%%rsp)\t\n"
//	"invlpg (%%rax)\t\n"
	"mov %0,%%cr3\t\n"
//	"mov %%cr3,%0\t\n"	
    "mov %%cr0, %%rax\t\n"
    "or $0x80000001, %%eax\t\n"
    "mov %%rax, %%cr0\t\n"
	::"r"(pt)
);
/*
	"mov %0,%%cr3\t\n"
//	"mov %%cr3,%0\t\n"
	::"r"(pt)
*/
	return;	
}

void
set_user_pages(
			  )
{
	__volatile__ uint64_t *pml, *pdp, *pd, *ptu;
	uint64_t *ptk;
	uint64_t addr = 0x0000000000000000;
	uint64_t virt;
	int *ptr;
	int i = 0;

	__volatile__ uint64_t pml_off;
	__volatile__ uint64_t pdp_off;
	__volatile__ uint64_t pd_off;
	__volatile__ uint64_t pt_off;

	uint64_t base_addr = (uint64_t)mem_data.physbase;

	pml = page_alloc();
	pdp = page_alloc();
	pd = page_alloc();
	ptk = page_alloc();
	ptu = page_alloc();

	*(pml + 1) = 0x0000000000000000;
	*(pdp + 1) = 0x0000000000000000;
	*(pd + 1) = 0x0000000000000000;
	*(ptk + 1) = 0x0000000000000000;
	*(ptu + 1) = 0x0000000000000000;

	virt = virt_base_for_user + (uint64_t)mem_data.physbase;

	pml_off = get_pml4_offset(virt);
	pdp_off = get_pdp_offset(virt);
	pd_off  = get_pd_offset(virt);

	while (/*base_addr <= ((uint64_t)mem_data.physfree - 0x1000)*/i < 512)
	{
		*(ptk + get_pt_offset(virt)) = base_addr;
		*(ptk + get_pt_offset(virt)) |= KERN_PERM_BITS;
		virt += 0x1000;
		base_addr += 0x1000;
		i++;
	}

    *(pd + pd_off) = ((uint64_t)ptk - virt_base_for_user);
    *(pd + pd_off) |= KERN_PERM_BITS;

    *(pdp + pdp_off) = ((uint64_t)pd - virt_base_for_user);
    *(pdp + pdp_off) |= KERN_PERM_BITS;

    *(pml + pml_off) = ((uint64_t)pdp - virt_base_for_user);
    *(pml + pml_off) |= KERN_PERM_BITS;

	pml_off = get_pml4_offset(addr);
	pdp_off = get_pdp_offset(addr);	
	pd_off  = get_pd_offset(addr);	
	pt_off  = get_pt_offset(addr);

	*(ptu + pt_off) = phys_addr;
	*(ptu + pt_off) |= USR_PERM_BITS;

    *(pd + pd_off) = ((uint64_t)ptu - virt_base_for_user);
    *(pd + pd_off) |= USR_PERM_BITS;

    *(pdp + pdp_off) = ((uint64_t)pd - virt_base_for_user);
    *(pdp + pdp_off) |= USR_PERM_BITS;

    *(pml + pml_off) = ((uint64_t)pdp - virt_base_for_user);
    *(pml + pml_off) |= USR_PERM_BITS;

	if (!(*(ptu + pt_off) && *(pd + pd_off) && *(pdp + pdp_off) && *(pml + pml_off)))
	{ 
		return;
	}

	/*
	 * load user cr3
	 */
	tlb_flush((uint64_t)pml - virt_base_for_user);

	ptr = (int *) addr;
	*ptr = 0x1;

	return;
}
