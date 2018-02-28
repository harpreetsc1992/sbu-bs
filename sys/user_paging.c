#include <sys/page_alloc.h>
#include <sys/kern_ops.h>
#include <sys/idt.h>
#include <sys/process.h>

uint64_t virt_base_for_user = VIRT_BASE;

uint64_t usr_phys_addr = START_PADDR_FOR_USRS;
uint64_t user_addr = START_VADDR_FOR_USRS;

uint64_t pml4_shared;
uint64_t usr_pml;
uint64_t usr_pdp;
uint64_t usr_pd;
uint64_t usr_pt;
uint64_t knl_pt1;
uint64_t knl_pt2;

uint16_t pt_count;
uint16_t pd_count;
uint16_t pdp_count;

uint8_t dif_ctxt = 1;

int pt_flag, pd_flag, pdp_flag;

uint64_t pml_same_ctxt = PML_SAME_CTXT;

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

uint64_t
increment_pml(
			 )
{
	pml_same_ctxt += PML_INC;
	if (pml_same_ctxt == PML_MAX_INC)
	{
		kprintf("Run out of PML offsets\n PMLs will be flushed\n");
		pml_same_ctxt = PML_SAME_CTXT;
	}
	return pml_same_ctxt;
}

void
add_user_page(
			  uint64_t virt,
			  uint64_t *pdp,
			  uint64_t *pd,
			  uint64_t *ptu,
			  uint64_t perm
			 )
{
	uint64_t pml_off = get_pml4_offset(virt);
	uint64_t pdp_off = get_pdp_offset(virt);
	uint64_t pd_off  = get_pd_offset(virt);
	uint64_t pt_off  = get_pt_offset(virt);

	uint64_t local_phys_addr = get_user_phys_addr(0);
	uint64_t *pml = (uint64_t *)pml4_shared;

	*(ptu + pt_off) = local_phys_addr;
	*(ptu + pt_off) |= perm;
	usr_pt = (uint64_t) ptu;

    *(pd + pd_off) = ((uint64_t)ptu - virt_base_for_user);
    *(pd + pd_off) |= perm;
	usr_pd = (uint64_t) pd;

	*(pdp + pdp_off) = ((uint64_t)pd - virt_base_for_user);
    *(pdp + pdp_off) |= perm;
	usr_pdp = (uint64_t) pdp;

    *(pml + pml_off) = ((uint64_t)pdp - virt_base_for_user);
    *(pml + pml_off) |= perm;
	pml4_shared = usr_pml = (uint64_t) pml;
}

void
set_new_kpts(
			uint64_t *ptk,
			uint64_t virt,
			uint64_t *pdp,
			uint64_t *pd,
			uint64_t *pml,
			uint64_t phys_addr
		   )
{
	uint64_t pml_off = get_pml4_offset(virt);
	uint64_t pdp_off = get_pdp_offset(virt);
	uint64_t pd_off = get_pd_offset(virt);
	int i = 0;

	while (i < TABLE_SIZE)
	{
		*(ptk + get_pt_offset(virt)) = phys_addr;
		*(ptk + get_pt_offset(virt)) |= KERN_PERM_BITS;
		virt += PAGE_SIZE;
		phys_addr += PAGE_SIZE;
		i++;
	}

    *(pd + pd_off) = ((uint64_t)ptk - virt_base_for_user);
    *(pd + pd_off) |= KERN_PERM_BITS;

    pd_shared = (uint64_t) pd;

    *(pdp + pdp_off) = ((uint64_t)pd - virt_base_for_user);
    *(pdp + pdp_off) |= KERN_PERM_BITS;
    pdp_shared = (uint64_t) pdp;

    *(pml + pml_off) = ((uint64_t)pdp - virt_base_for_user);
    *(pml + pml_off) |= KERN_PERM_BITS;
    pml4_shared = (uint64_t) pml;
}

void *
get_pt(
	   uint64_t addr,
	   uint64_t pd_lkup
	  )
{
	if ((*((uint64_t *)pd_lkup + get_pd_offset(addr)) && 0x1) == 0x1)
	{
		return (void *)(*((uint64_t *)pd_lkup + get_pd_offset(addr)) + VIRT_BASE);
	}
	else
	{
		void *page = (void *) page_alloc();
		return page;
	}
}

void *
get_pd(
	   uint64_t addr,
	   uint64_t pdp_lkup
	  )
{
	if ((*((uint64_t *)pdp_lkup + get_pdp_offset(addr)) && 0x1) == 0x1)
	{
		return (void *)(*((uint64_t *)pdp_lkup + get_pdp_offset(addr)) + VIRT_BASE);
	}
	else
	{		
		void *page = (void *) page_alloc();
		return page;
	}
}

void *
get_pdp(
		 uint64_t addr,
		 uint64_t pml_lkup
		)
{		
	if ((*((uint64_t *)pml_lkup + get_pml4_offset(addr)) && 0x1) == 0x1)
	{
		return (void *)(*((uint64_t *)pml_lkup + get_pml4_offset(addr)) + VIRT_BASE);
	}
	else
	{
		void *page = (void *) page_alloc();
		return page;
	}
}

void
set_kernel_page_user(
					 uint64_t addr
					)
{
	addr = (addr >> 12) << 12;
	uint64_t *pml = (uint64_t *)pml4_shared;
	uint64_t *pdp = (uint64_t *)ROUNDDOWN((uint64_t)get_pdp(addr, (uint64_t)pml), PAGE_SIZE);
	uint64_t *pd = (uint64_t *) ROUNDDOWN((uint64_t)get_pd(addr, (uint64_t)pdp), PAGE_SIZE);
	uint64_t *pt = (uint64_t *)ROUNDDOWN((uint64_t)get_pt(addr, (uint64_t)pd), PAGE_SIZE);

	uint64_t phys_addr = *(pt + get_pt_offset(addr));
		
	*(pt + get_pt_offset(addr)) = phys_addr;
	*(pt + get_pt_offset(addr)) |= USR_PERM_BITS;

    *(pd + get_pd_offset(addr)) = ((uint64_t)pt - virt_base_for_user);
	*(pd + get_pd_offset(addr)) = ((*(pd + get_pd_offset(addr)) >> 12) << 12);
	 *(pd + get_pd_offset(addr)) |= USR_PERM_BITS;

    *(pdp + get_pdp_offset(addr)) = ((uint64_t)pd - virt_base_for_user);
	*(pdp + get_pdp_offset(addr)) = ((*(pdp + get_pdp_offset(addr)) >> 12) << 12);
	*(pdp + get_pdp_offset(addr)) |= USR_PERM_BITS;

    *(pml + get_pml4_offset(addr)) = ((uint64_t)pml - virt_base_for_user);
	*(pml + get_pml4_offset(addr)) = ((*(pml + get_pml4_offset(addr)) >> 12) << 12);
	 *(pml + get_pml4_offset(addr)) |= USR_PERM_BITS;

	tlb_flush(pml4_shared - virt_base_for_user);
}

void
change_permissions(
				   void *addr,
				   uint64_t perm
				  )
{
	uint64_t *pml, *pdp, *pd, *pt;
	uint64_t virt = (uint64_t) addr;
	uint64_t pt_off;
	pt_off  = get_pt_offset(virt);
	
	pml = (uint64_t *)pml4_shared;
	pdp = (uint64_t *)ROUNDDOWN((uint64_t)get_pdp((uint64_t)addr, (uint64_t)pml), PAGE_SIZE);
	pd = (uint64_t *) ROUNDDOWN((uint64_t)get_pd((uint64_t)addr, (uint64_t)pdp), PAGE_SIZE);
	pt = (uint64_t *)ROUNDDOWN((uint64_t)get_pt((uint64_t)addr, (uint64_t)pd), PAGE_SIZE);
	
	*(pt + pt_off) = ((*(pt + pt_off) >> 12) << 12) | perm;
	usr_pt = (uint64_t) pt;

	*(pd + (get_pd_offset((uint64_t)addr))) = ((uint64_t)pt - virt_base_for_user);
	*(pdp + (get_pdp_offset((uint64_t)addr))) = ((uint64_t)pd - virt_base_for_user);
	*(pml + (get_pml4_offset((uint64_t)addr))) = ((uint64_t)pdp - virt_base_for_user);

	usr_pd = (uint64_t)pd;
	usr_pdp = (uint64_t)pdp;
	usr_pml = (uint64_t)pml;

//    *(pd + pd_off) = ((uint64_t)pt - virt_base_for_user);
//    *(uint64_t *)((*(pd + pd_off) >> ) << 3) |= perm;
//	usr_pd = (uint64_t) pd;

//	*(pdp + pdp_off) = ((uint64_t)pd - virt_base_for_user);
//    *(uint64_t *)((*(pdp + pdp_off) >> 3) << 3) |= perm;
//	usr_pdp = (uint64_t) pdp;

//	pml = (uint64_t *)pml4_shared;
//    *(pml + pml_off) = ((uint64_t)pdp - virt_base_for_user);
//    *(uint64_t *)((*(pml + pml_off) >> 3) << 3) |= perm;
//	pml4_shared = usr_pml = (uint64_t) pml;		
}

void *
memmap(
	 void *addr, 
	 size_t length, 
	 uint64_t prot, 
	 uint64_t flags
	)
{
	uint64_t *ptkern, *ptk1, *ptk2, *init_ptk, *ptu, *pdp, *pd, *pml;
	
	uint64_t base_addr = (uint64_t) mem_data.physbase;
	uint64_t kern_end = (uint64_t) mem_data.physbase + TABLE_SIZE * PAGE_SIZE;
	uint64_t kern_extra = (uint64_t) mem_data.physbase + 2 * TABLE_SIZE * PAGE_SIZE;
	uint64_t init_addr = 0x0;
	
	uint64_t virt_base_addr = VIRT_BASE + base_addr;
	uint64_t virt_kern_end = VIRT_BASE + kern_end;
	uint64_t virt_kern_extra = VIRT_BASE + kern_extra;
	uint64_t virt_init_addr = VIRT_BASE;

	int pages = length/PAGE_SIZE;
	int modulo = length % PAGE_SIZE;

	if (modulo) pages++;

	int i;

	switch (flags)
	{
		case ACCESSED_NOT_PRESENT: /* Page Fault error code 0x4 */
			/*
			 * New PDP and PD for user pages
			 */
			pdp = (uint64_t *)ROUNDDOWN(get_pdp((uint64_t)addr, pml4_shared), PAGE_SIZE);
			usr_pdp = (uint64_t)pdp;
			pd = (uint64_t *)ROUNDDOWN(get_pd((uint64_t)addr, (uint64_t)pdp), PAGE_SIZE);
			usr_pd = (uint64_t)pd;
			ptu = (uint64_t *)ROUNDDOWN(get_pt((uint64_t)addr, (uint64_t)pd), PAGE_SIZE);
			usr_pt = (uint64_t)ptu;
			
			while (pages--)
			{
				add_user_page((uint64_t)ROUNDDOWN((uint64_t)addr, PAGE_SIZE), pdp, pd, ptu, prot);
				if (pages) 	addr = (void *)((uint64_t)addr + PAGE_SIZE);
			}
		break;
		case SET_COW_RW:	/* Page fault error code 0x5 */
			addr = (void *) get_user_virt_addr(0);
			pdp = (uint64_t *)ROUNDDOWN(get_pdp((uint64_t)addr, pml4_shared), PAGE_SIZE);
			usr_pdp = (uint64_t)pdp;
			pd = (uint64_t *)ROUNDDOWN(get_pd((uint64_t)addr, (uint64_t)pdp), PAGE_SIZE);
			usr_pd = (uint64_t)pd;
			ptu = (uint64_t *)ROUNDDOWN(get_pt((uint64_t)addr, (uint64_t)pd), PAGE_SIZE);
			usr_pt = (uint64_t)ptu;

			add_user_page((uint64_t)ROUNDDOWN((uint64_t)addr, PAGE_SIZE), pdp, pd, ptu, prot);
		break;
		case RW_TO_COW:		/* Not a case of page fault. Use during fork */
			i = 0;
			while (i < pages)
			{
				change_permissions((uint64_t *)ROUNDDOWN((uint64_t)addr + i * PAGE_SIZE, PAGE_SIZE), prot);
				i++;
			}
		break;
		case NEW_PAGE:			/* Used when a pcb element asks for a page in user space */
			/*
			 * Traverse Page Tables
			 */
			if (!dif_ctxt)
			{
				addr = (void *) (get_user_virt_addr(0) + increment_pml());
			}
			else
			{
				addr = (void *) (get_user_virt_addr(0));
			}
			pdp = (uint64_t *)ROUNDDOWN(get_pdp((uint64_t)addr, (uint64_t)pml4_shared), PAGE_SIZE);
			usr_pdp = (uint64_t)pdp;
			pd = (uint64_t *)ROUNDDOWN(get_pd((uint64_t)addr, (uint64_t)pdp), PAGE_SIZE);
			usr_pd = (uint64_t)pd;
			ptu = (uint64_t *)ROUNDDOWN(get_pt((uint64_t)addr, (uint64_t)pd), PAGE_SIZE);
			usr_pt = (uint64_t)ptu;
			while (pages--)
			{
				add_user_page((uint64_t)addr, pdp, pd, ptu, USR_PERM_BITS);
				if (pages) addr = (void *)((uint64_t)addr + PAGE_SIZE);
			}
		break;
		case GROWS_DOWN:
			/*
			 * Traverse Page Tables
			 */
//			if (!dif_ctxt)
//			{
//				addr = (void *) (get_user_virt_addr(0) + increment_pml());
//			}
//			else
//			{
				addr = (void *) (get_user_virt_addr(0));
//			}
			pdp = (uint64_t *)ROUNDDOWN(get_pdp((uint64_t)addr, (uint64_t)pml4_shared), PAGE_SIZE);
			usr_pdp = (uint64_t)pdp;
			pd = (uint64_t *)ROUNDDOWN(get_pd((uint64_t)addr, (uint64_t)pdp), PAGE_SIZE);
			usr_pd = (uint64_t)pd;
			ptu = (uint64_t *)ROUNDDOWN(get_pt((uint64_t)addr, (uint64_t)pd), PAGE_SIZE);
			usr_pt = (uint64_t)ptu;
			while (pages--)
			{
				add_user_page((uint64_t)addr - PAGE_SIZE, pdp, pd, ptu, USR_PERM_BITS);
				if (pages) addr = (void *)((uint64_t)addr + PAGE_SIZE);
			}
		break;
		default: 		//Context Switch
			init_ptk = page_alloc();
			ptkern = page_alloc();
			ptk1 = page_alloc();
			ptk2 = page_alloc();
			ptu = page_alloc();
			pd = page_alloc();
			pdp = page_alloc();
			pml = page_alloc();
		
			addr = (void *)get_user_virt_addr(0);
			/*
			 * Initial addresses VIRT_BASE->VIRT_BASE+physbase
			 */
			set_new_kpts(init_ptk, virt_init_addr, pdp, pd, pml, init_addr);
			/*
			 * Kernel addresses VIRT_BASE+physbase->VIRT_BASE+physbase+512*PAGE_SIZE
			 */
			set_new_kpts(ptkern, virt_base_addr, pdp, pd, pml, base_addr);
			/*
			 * Above Kernel addresses VIRT_BASE+physbase+512*PAGE_SIZE->VIRT_BASE+physbase+2*512*PAGE_SIZE
			 */
			set_new_kpts(ptk1, virt_kern_end, pdp, pd, pml, kern_end);
			/*
			 * Extra Addresses from 0xFFFFFFFF80400000->0xFFFFFFFF80600000
			 */
			set_new_kpts(ptk2, virt_kern_extra, pdp, pd, pml, kern_extra);
		
			/*
			 * User page
			 */
			while (pages--)
			{
				add_user_page((uint64_t)addr, pdp, pd, ptu, USR_PERM_BITS);
				if (pages) addr = (void *)((uint64_t)addr + PAGE_SIZE);
			}
		break;
	}
	tlb_flush((uint64_t)pml4_shared - VIRT_BASE);
	return addr;
}
#if 0
uint64_t*
set_user_pages(
			  )
{
	__volatile__ uint64_t *pml, *pdp, *pd, *ptu;
	uint64_t *ptk1, *ptk2;
	uint64_t local_user_addr = get_user_virt_addr(0);
	uint64_t virt, virt_ext;
	uint64_t local_phys_addr = get_user_phys_addr(0);
	uint64_t *ptr;
	int i = 0; 

	__volatile__ uint64_t pml_off;
	__volatile__ uint64_t pdp_off;
	__volatile__ uint64_t pd_off;
	__volatile__ uint64_t pt_off;

	uint64_t base_addr = (uint64_t)mem_data.physbase;

	if (dif_ctxt)
	{
		pml = page_alloc();
		pdp = page_alloc();
		pd = page_alloc();
		ptk1 = page_alloc();
		ptk2 = page_alloc();
		ptu = page_alloc();
	}
	else
	{
		pml = (uint64_t *) usr_pml;
		pdp = (uint64_t *) usr_pdp;
		pd  = (uint64_t *) usr_pd;
		ptk1 = (uint64_t *) knl_pt1;
		ptk2 = (uint64_t *) knl_pt2;
		ptu = (uint64_t *) usr_pt;
	}

	virt = virt_base_for_user + (uint64_t)mem_data.physbase;
	virt_ext = virt_base_for_user + (uint64_t)mem_data.physbase + (PAGE_SIZE * TABLE_SIZE);

	pml_off = get_pml4_offset(virt);
	pdp_off = get_pdp_offset(virt);
	pd_off  = get_pd_offset(virt);
	if (dif_ctxt)
	{
		while (i < TABLE_SIZE)
		{
			*(ptk1 + get_pt_offset(virt)) = base_addr;
			*(ptk1 + get_pt_offset(virt)) |= KERN_PERM_BITS;
			virt += PAGE_SIZE;
			base_addr += PAGE_SIZE;
			i++;
		}
		knl_pt1 = (uint64_t)ptk1;
		i = 0;
		while (i < TABLE_SIZE)
		{
			*(ptk2 + get_pt_offset(virt)) = base_addr;
			*(ptk2 + get_pt_offset(virt)) |= KERN_PERM_BITS;
			virt += PAGE_SIZE;
			base_addr += PAGE_SIZE;
			i++;
		}
		knl_pt2 = (uint64_t)ptk2;
	}
	
   	if (pd_count >= TABLE_SIZE)
	{
		pd_flag = 1;
		pd = page_alloc();
		pd_count = 0;
	}

 	*(pd + pd_off) = ((uint64_t)ptk1 - virt_base_for_user);
    *(pd + pd_off) |= KERN_PERM_BITS;
	pd_count++;

	pd_off = get_pd_offset(virt_ext);
	
	*(pd + pd_off) = ((uint64_t)ptk2 - virt_base_for_user);
    *(pd + pd_off) |= KERN_PERM_BITS;
	pd_count++;	

	usr_pd = (uint64_t) pd;	

   	if (pdp_count >= TABLE_SIZE)
	{
		pdp_flag = 1;
		pdp = page_alloc();
		pdp_count = 0;
	}

 	*(pdp + pdp_off) = ((uint64_t)pd - virt_base_for_user);
    *(pdp + pdp_off) |= KERN_PERM_BITS;
	usr_pdp = (uint64_t) pdp;
	pdp_count++;

    *(pml + pml_off) = ((uint64_t)pdp - virt_base_for_user);
    *(pml + pml_off) |= KERN_PERM_BITS;
	usr_pml = (uint64_t) pml;

	pml_off = get_pml4_offset(local_user_addr);
	pdp_off = get_pdp_offset(local_user_addr);	
	pd_off  = get_pd_offset(local_user_addr);	
	pt_off  = get_pt_offset(local_user_addr);

	*(ptu + pt_off) = local_phys_addr;
	*(ptu + pt_off) |= USR_PERM_BITS;
	usr_pt = (uint64_t) ptu;

   	if (pd_count >= TABLE_SIZE)
	{
		pd_flag = 1;
		pd = page_alloc();
		pd_count = 0;
	}

    *(pd + pd_off) = ((uint64_t)ptu - virt_base_for_user);
    *(pd + pd_off) |= USR_PERM_BITS;
	usr_pd = (uint64_t) pd;
	pd_count++;

   	if (pdp_count >= TABLE_SIZE)
	{
		pdp_flag = 1;
		pdp = page_alloc();
		pdp_count = 0;
	}

    *(pdp + pdp_off) = ((uint64_t)pd - virt_base_for_user);
    *(pdp + pdp_off) |= USR_PERM_BITS;
	usr_pdp = (uint64_t) pdp;
	pdp_count++;

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
	tlb_flush((uint64_t)pml - virt_base_for_user);

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
	upg->pd_rw = READ_WRITE;
    upg->pd_free = IN_USE;
	upg->next = NULL;
	int i = 1;

	while (i < pages)
	{
		dif_ctxt = 0;		
		pg_brk = set_user_pages();
		upg->next = (struct user_page *) pg_brk;
//		if (upg->next->pd_free != FREE)
//		{
//			continue;
//		}
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
	upg->pd_rw = READONLY_COW;
    upg->pd_free = IN_USE;
	upg->next = NULL;
	int i = 1;

	while (i < pages)
	{
		dif_ctxt = 0;
		pg_brk = set_user_pages();
		upg->next = (struct user_page *) pg_brk;
//		if (upg->next->pd_free == IN_USE)
//		{
//			continue;
//		}
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

	uint64_t local_phys_addr = get_user_phys_addr(0);
	uint64_t base_addr = (uint64_t)mem_data.physbase;
	uint64_t vid_addr = (uint64_t) 0x0;

	uint64_t *ptu, *pd, *pdp, *pml, *ptk1, *ptk2, *pt_v;
	if (!dif_ctxt)
	{
		ptu = (uint64_t *) usr_pt;
		pd = (uint64_t *) usr_pd;
		pdp = (uint64_t *) usr_pdp;
		pml = (uint64_t *) pml4_shared;
	}
	else
	{
		ptu = page_alloc();
		usr_pt = (uint64_t) ptu;
		pd = page_alloc();
		usr_pd = (uint64_t) pd;
		pdp = page_alloc();
		usr_pdp = (uint64_t) pdp;
		pml = page_alloc();	
		pml4_shared = pml;
		while (i < TABLE_SIZE)
        {
            *(ptk1 + get_pt_offset(addr)) = base_addr;
            *(ptk1 + get_pt_offset(addr)) |= KERN_PERM_BITS;
            addr += PAGE_SIZE;
            base_addr += PAGE_SIZE;
            i++;
        }
        knl_pt1 = (uint64_t)ptk1;
        i = 0;
        while (i < TABLE_SIZE)
        {
            *(ptk2 + get_pt_offset(addr)) = base_addr;
            *(ptk2 + get_pt_offset(addr)) |= KERN_PERM_BITS;
            addr += PAGE_SIZE;
            base_addr += PAGE_SIZE;
            i++;
        }
        knl_pt2 = (uint64_t)ptk2;
	 	i = 0;
        while (i < TABLE_SIZE)
        {
            *(ptk3 + get_pt_offset(addr)) = vid_addr;
            *(ptk3 + get_pt_offset(addr)) |= KERN_PERM_BITS;
            addr += PAGE_SIZE;
            vid_addr += PAGE_SIZE;
            i++;
        }
        knl_pt2 = (uint64_t)ptk2;	
	}

	*(ptu + pt_off) = (uint64_t) local_phys_addr;
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

void
set_user_perm(
			  uint64_t addr
			 )
{
	struct user_page *upg = (struct user_page *) addr;
	
	uint64_t *pt = (uint64_t *)page_alloc();
	uint64_t *pd = (uint64_t *)usr_pd;
	uint64_t *pdp = (uint64_t *)usr_pdp;
	uint64_t *pml = (uint64_t *)pml4_shared;
	
	uint64_t pml_off = get_pml4_offset((uint64_t)addr);
	uint64_t pdp_off = get_pdp_offset((uint64_t)addr);	
	uint64_t pd_off  = get_pd_offset((uint64_t)addr);	
	uint64_t pt_off  = get_pt_offset((uint64_t)addr);

	uint64_t phys_addr = get_user_phys_addr(0); 

	*(pt + pt_off) = phys_addr;
	*(ptu + pt_off) |= USR_PERM_BITS;

    *(pd + pd_off) = ((uint64_t)ptu - virt_base_for_user);
    *(pd + pd_off) |= USR_PERM_BITS;

    *(pdp + pdp_off) = ((uint64_t)pd - virt_base_for_user);
    *(pdp + pdp_off) |= USR_PERM_BITS;

    *(pml + pml_off) = ((uint64_t)pdp - virt_base_for_user);
    *(pml + pml_off) |= USR_PERM_BITS;
	tlb_flush((uint64_t)pml - virt_base_for_user);
	upg->pd_rw = READ_WRITE;
   	upg->pd_free = IN_USE;
	upg->next = NULL;	
}
/*
void
map_page(
		 uint64_t va,
		 uint64_t pa
		)
{
	uint64_t pml_off = get_pml4_offset(va);
    uint64_t pdp_off = get_pdp_offset(va);
    uint64_t pd_off  = get_pd_offset(va);
    uint64_t pt_off  = get_pt_offset(va);
	
	uint64_t *ptu = (uint64_t *) usr_pt;
	uint64_t *pd = (uint64_t *) usr_pd;
	uint64_t *pdp = (uint64_t *) usr_pdp;
	uint64_t *pml = (uint64_t *) pml4_shared;
	*(ptu + pt_off) = pa;
	*(ptu + pt_off) |= USR_PERM_BITS;

    *(pd + pd_off) = ((uint64_t)ptu - virt_base_for_user);
    *(pd + pd_off) |= USR_PERM_BITS;

    *(pdp + pdp_off) = ((uint64_t)pd - virt_base_for_user);
    *(pdp + pdp_off) |= USR_PERM_BITS;

    *(pml + pml_off) = ((uint64_t)pdp - virt_base_for_user);
    *(pml + pml_off) |= USR_PERM_BITS;
	tlb_flush((uint64_t)pml4_shared - virt_base_for_user);
	upg->pd_rw = READ_WRITE;
   	upg->pd_free = IN_USE;
	upg->next = NULL;	

	return;
}

void
allocate_page(
              uint64_t *va,
              uint64_t *pa,
              uint32_t sz
             )
{
	struct user_page *upg;
	uint32_t pages = sz/PAGE_SIZE;
	uint16_t modulo = sz % PAGE_SIZE;
	uint64_t *pg_brk = NULL;
	uint64_t *hd = NULL;	

	upg = (struct user_page *) va;
	if (modulo > 0)
	{
		pages++;
	}
	
	pg_brk = map_page(va, pa);
	hd = pg_brk;
	upg = (struct user_page *) pg_brk;
//	tlb_flush((uint64_t)pml4_shared - virt_base_for_user);
	upg->pd_rw = READONLY_COW;
    upg->pd_free = IN_USE;
	upg->next = NULL;
	int i = 1;

	while (i < pages)
	{
		dif_ctxt = 0;
		pg_brk = map_page((uint64_t) va + PAGE_SIZE, (uint64_t) pa + PAGE_SIZE);
		upg->next = (struct user_page *) pg_brk;
//		tlb_flush((uint64_t)pml4_shared - virt_base_for_user);
		if (upg->next->pd_free == IN_USE)
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
*/
#endif
