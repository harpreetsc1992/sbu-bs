#include <sys/page_alloc.h>

uint64_t phys_addr = 0x5000000;

void
set_user_pages(
			  )
{
	uint64_t *pml, *pdp, *pd, *pt;
	uint64_t addr = 0xffffff8000000000;

	pml = (uint64_t *) phys_addr;
	phys_addr += 0x1000;

	pdp = (uint64_t *) phys_addr;
	phys_addr += 0x1000;

	pd = (uint64_t *) phys_addr;
	phys_addr += 0x1000;

	pt = (uint64_t *) phys_addr;
	phys_addr += 0x1000;

    uint64_t pml_off = get_pml4_offset(addr);
    uint64_t pdp_off = get_pdp_offset(addr);
    uint64_t pd_off  = get_pd_offset(addr);

	*(pt + get_pt_offset(addr)) = phys_addr;
	*(pt + get_pt_offset(addr)) |= USR_PERM_BITS;
	
    *(pd + pd_off) = (uint64_t)pt;
    *(pd + pd_off) |= USR_PERM_BITS;

    *(pdp + pdp_off) = (uint64_t)pd;
    *(pdp + pdp_off) |= USR_PERM_BITS;

    *(pml + pml_off) = (uint64_t)pdp;
    *(pml + pml_off) |= USR_PERM_BITS;
}
