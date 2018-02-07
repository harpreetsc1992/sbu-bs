#include <sys/page_alloc.h>
#include <sys/kern_ops.h>
#include <sys/idt.h>

uint64_t virt_base_for_user = VIRT_BASE;

uint64_t usr_phys_addr = START_PADDR_FOR_USRS;
uint64_t user_addr = START_VADDR_FOR_USRS;

uint64_t pml4_shared;
uint64_t usr_pml;
uint64_t usr_pdp;
uint64_t usr_pd;
uint64_t usr_pt;

void
tlb_flush(
		  uint64_t pt
		 )
{
	__asm__ __volatile__(
	"mov %%rdi, 8(%%rsp)\t\n"
	"invlpg (%%rdi)\t\n"
	"mov %0,%%cr3\t\n"

    "mov %%cr0, %%rax\t\n"
    "or $0x80000001, %%eax\t\n"
    "mov %%rax, %%cr0\t\n"
	::"r"(pt)
);

	return;	
}

uint64_t
get_user_virt_addr(
				   int mapped
				  )
{
	uint64_t buf_addr = user_addr;
	if (!mapped)
	{
		if ((user_addr + PAGE_SIZE) < ((uint64_t)mem_data.physbase + virt_base_for_user))
		{
			user_addr += PAGE_SIZE;
		}
		else
		{
			user_addr = START_VADDR_FOR_USRS;
		}
		return buf_addr;
	}

	return (buf_addr - PAGE_SIZE);
}

uint64_t
get_user_phys_addr(
				   int mapped
				  )
{
	uint64_t buf_addr = usr_phys_addr;
	if (!mapped)
	{
		if ((usr_phys_addr + PAGE_SIZE) < 0x6000000)
		{
			usr_phys_addr += PAGE_SIZE;
		}
		else
		{	
			usr_phys_addr = START_PADDR_FOR_USRS;
		}
		return buf_addr;
	}

	return (buf_addr - PAGE_SIZE);
}

uint64_t*
set_user_pages(
			  )
{
	__volatile__ uint64_t *pml, *pdp, *pd, *ptu;
	uint64_t *ptk;
	uint64_t local_user_addr = get_user_virt_addr(0);
	uint64_t virt;
	uint64_t local_phys_addr = get_user_phys_addr(0);
	uint64_t *ptr;
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

	virt = virt_base_for_user + (uint64_t)mem_data.physbase;

	pml_off = get_pml4_offset(virt);
	pdp_off = get_pdp_offset(virt);
	pd_off  = get_pd_offset(virt);

	while (/*base_addr <= ((uint64_t)mem_data.physfree - 0x1000)*/i < 512)
	{
		*(ptk + get_pt_offset(virt)) = base_addr;
		*(ptk + get_pt_offset(virt)) |= KERN_PERM_BITS;
		virt += PAGE_SIZE;
		base_addr += PAGE_SIZE;
		i++;
	}

    *(pd + pd_off) = ((uint64_t)ptk - virt_base_for_user);
    *(pd + pd_off) |= KERN_PERM_BITS;

    *(pdp + pdp_off) = ((uint64_t)pd - virt_base_for_user);
    *(pdp + pdp_off) |= KERN_PERM_BITS;

    *(pml + pml_off) = ((uint64_t)pdp - virt_base_for_user);
    *(pml + pml_off) |= KERN_PERM_BITS;

	pml_off = get_pml4_offset(local_user_addr);
	pdp_off = get_pdp_offset(local_user_addr);	
	pd_off  = get_pd_offset(local_user_addr);	
	pt_off  = get_pt_offset(local_user_addr);

	*(ptu + pt_off) = local_phys_addr;
	*(ptu + pt_off) |= USR_PERM_BITS;
	usr_pt = (uint64_t) ptu;

    *(pd + pd_off) = ((uint64_t)ptu - virt_base_for_user);
    *(pd + pd_off) |= USR_PERM_BITS;
	usr_pd = (uint64_t) pd;

    *(pdp + pdp_off) = ((uint64_t)pd - virt_base_for_user);
    *(pdp + pdp_off) |= USR_PERM_BITS;
	usr_pdp = (uint64_t) pdp;

    *(pml + pml_off) = ((uint64_t)pdp - virt_base_for_user);
    *(pml + pml_off) |= USR_PERM_BITS;
	usr_pml = (uint64_t) pml;

	if (!(*(ptu + pt_off) && *(pd + pd_off) && *(pdp + pdp_off) && *(pml + pml_off)))
	{ 
		return NULL;
	}

	/*
	 * load user cr3
	 */
//	tlb_flush((uint64_t)pml - virt_base_for_user);
//	usr_pml = (uint64_t)pml - virt_base_for_user;

	ptr = (uint64_t *) local_user_addr;
	pml4_shared = (uint64_t)pml;
	

	return ptr;
}

uint64_t *
get_usr_page(
			 uint32_t sz
			)
{
	struct user_page *upg;
	uint32_t pages = sz/PAGE_SIZE;
	uint16_t modulo = sz % PAGE_SIZE;
	uint64_t *pg_brk = NULL;
	uint64_t *hd = NULL;
	if (modulo > 0)
	{
		pages++;
	}
	
	pg_brk = set_user_pages();
	hd = pg_brk;
	upg = (struct user_page *) pg_brk;
	tlb_flush((uint64_t)pml4_shared - virt_base_for_user);
	upg->pd_rw = READ_WRITE;
    upg->pd_free = IN_USE;
	upg->next = NULL;
	int i = 1;

	while (i < pages)
	{
		pg_brk = set_user_pages();
		upg->next = (struct user_page *) pg_brk;
		if (upg->pd_free == IN_USE)
		{
			continue;
		}
		upg = upg->next;
		upg->pd_rw = READ_WRITE;
    	upg->pd_free = IN_USE;
		upg->next = NULL;
		i++;
	}
	upg = (struct user_page *) hd;

	return hd;
}

uint64_t *
get_usr_page_for_child(
			 		   uint32_t sz
					  )
{
	struct user_page *upg;
	uint32_t pages = sz/PAGE_SIZE;
	uint16_t modulo = sz % PAGE_SIZE;
	uint64_t *pg_brk = NULL;
	uint64_t *hd = NULL;
	if (modulo > 0)
	{
		pages++;
	}
	
	pg_brk = set_user_pages();
	hd = pg_brk;
	upg = (struct user_page *) pg_brk;
	tlb_flush((uint64_t)pml4_shared - virt_base_for_user);
	upg->pd_rw = READONLY_COW;
    upg->pd_free = IN_USE;
	upg->next = NULL;
	int i = 1;

	while (i < pages)
	{
		pg_brk = set_user_pages();
		upg->next = (struct user_page *) pg_brk;
		if (upg->pd_free == IN_USE)
		{
			continue;
		}
		upg = upg->next;
		upg->pd_rw = READONLY_COW;
    	upg->pd_free = IN_USE;
		upg->next = NULL;
		i++;
	}
	upg = (struct user_page *) hd;

	return hd;
}

uint64_t *
get_new_page(
			)
{
	return get_usr_page(PAGE_SIZE);
}

void
add_user_page(
			  struct user_page *upg,
			  uint8_t perm
			 )
{
	uint64_t *addr = (uint64_t *) upg;
	uint64_t pml_off = get_pml4_offset((uint64_t)addr);
	uint64_t pdp_off = get_pdp_offset((uint64_t)addr);	
	uint64_t pd_off  = get_pd_offset((uint64_t)addr);	
	uint64_t pt_off  = get_pt_offset((uint64_t)addr);

	uint64_t *ptu = (uint64_t *) usr_pt;
	uint64_t *pd = (uint64_t *) usr_pd;
	uint64_t *pdp = (uint64_t *) usr_pdp;
	uint64_t *pml = (uint64_t *) pml4_shared;

	*(ptu + pt_off) = (uint64_t)addr;
	*(ptu + pt_off) |= perm;

    *(pd + pd_off) = ((uint64_t)ptu - virt_base_for_user);
    *(pd + pd_off) |= perm;

    *(pdp + pdp_off) = ((uint64_t)pd - virt_base_for_user);
    *(pdp + pdp_off) |= perm;

    *(pml + pml_off) = ((uint64_t)pdp - virt_base_for_user);
    *(pml + pml_off) |= perm;
	tlb_flush((uint64_t)pml4_shared - virt_base_for_user);
	upg->pd_rw = READ_WRITE;
   	upg->pd_free = IN_USE;
	upg->next = NULL;
}
