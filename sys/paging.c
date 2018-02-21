#include <sys/page_alloc.h>
#include <sys/kprintf.h>

struct page_desc *pdesc;
struct page_desc *_pdesc;
int virt_to_phy_flag;

unsigned *freelist;

uint64_t virt_base = VIRT_BASE;
uint64_t init_virt_base = VIRT_BASE;

uint64_t end_addr;

int counter;
int flag;
uint64_t end_base_addr;

uint64_t *global_pdp;
uint64_t *global_pml4;
uint64_t *global_pd;

uint64_t pml4_shared;
uint64_t pdp_shared;
uint64_t pd_shared;
uint64_t pt_shared;

void
load_cr3(
         uint64_t lcr3
        )
{
	pml4_shared = lcr3;

    __asm__ __volatile__(
    "mov %0, %%cr3;"
    "mov %%cr0, %%rax;"
    "or $0x80000001, %%eax;"
    "mov %%rax, %%cr0;"
    :: "r"(lcr3));

    return;
}

uint64_t
get_pt_offset(
			  uint64_t _virt_base
		   	 )
{
	return ((_virt_base >> PT_OFF) & PADDING);
}

uint64_t
get_pd_offset(
			  uint64_t _virt_base
		   	 )
{
	return ((_virt_base >> PD_OFF) & PADDING);
}

uint64_t
get_pdp_offset(
			   uint64_t _virt_base
		   	  )
{
	return ((_virt_base >> PDP_OFF) & PADDING);
}

uint64_t
get_pml4_offset(
				uint64_t _virt_base
			   )
{
	return ((_virt_base >> PML_OFF) & PADDING);
}

void*
page_alloc(
		  )
{
	struct page_desc *head;
	if(!virt_to_phy_flag)
	{
		_pdesc = pdesc;
		virt_to_phy_flag = 1;
	}
	pdesc = (struct page_desc *)((uint64_t)_pdesc + VIRT_BASE);
	head = pdesc;
	pdesc->pd_free = IN_USE;
	pdesc->next = (struct page_desc *)((uint64_t)_pdesc + VIRT_BASE + PAGE_SIZE);
	pdesc = pdesc->next;
	_pdesc = _pdesc + PDESC_PG_INC;
	freelist[counter++] = 0;
	return head;
}

void*
kmalloc(
		uint64_t sz
	   )
{
	struct page_desc *head, *trav;

	uint32_t pages = sz/PAGE_SIZE;
	uint32_t modulo = sz % PAGE_SIZE;
	if (modulo > 0)
	{
		pages++;
	}
	
	trav = pdesc;
//	uint32_t count = pages;
/*
	while (count-- > 0)
	{
		if (trav->pd_free != FREE)
		{
			count = pages;
			trav = trav->next;
			continue;
		}
		else
		{
			if ((struct page_desc *)((uint64_t)trav->next + VIRT_BASE) != NULL)		trav = (struct page_desc *)((uint64_t)trav->next + VIRT_BASE);
			else						kprintf("Out of free pages\n No pages left to allocate\n");
		}
	}
*/
	head = (struct page_desc *) page_alloc();
	trav = head;
	pages--;
	while (pages-- > 0)
	{
		trav = page_alloc();
	}
	if (trav);
	
	return (void *)head;
}

void*
get_page(
		)
{
	struct page_desc *head;
	head = pdesc;
	pdesc->pd_free = IN_USE;
	pdesc = pdesc->next;
	freelist[counter++] = 0;
	return head;
}

void*
__get_page(
		  )
{
	return get_page();
}

/*
void 
add_page(
		 uint64_t addr,
		 uint8_t perm
		)
{
	uint64_t *pt3 = (uint64_t *) page_alloc();
	uint64_t virt_end = VIRT_BASE + 3 * TABLE_SIZE * PAGE_SIZE;
	
	uint64_t pt_off, pd_off, pdp_off, pml_off;
	uint64_t *pd, *pdp, *pml;

	pt_off = get_pt_offset(virt_end);
	pd_off = get_pd_offset(virt_end);
	pdp_off = get_pdp_offset(virt_end);
	pml_off = get_pml4_offset(virt_end);

	pd = (uint64_t *)pd_shared;
	pdp = (uint64_t *)pdp_shared;
	pml = (uint64_t *)pml4_shared;

	if (addr != START_VADDR_FOR_USRS)
	{
		*(pt3 + pt_off) = addr;	
		*(pt3 + pt_off) |= USR_PERM_BITS;
		end_base_addr += PAGE_SIZE;
			
		*(pd + pd_off) = (uint64_t)pt3;
		*(pd + pd_off) |= KERN_PERM_BITS;

		*(pdp + pdp_off) = (uint64_t)pd;
		*(pdp + pdp_off) |= KERN_PERM_BITS;
	
		*(pml + pml_off) = (uint64_t)pdp;
		*(pml + pml_off) |= KERN_PERM_BITS;

		pml4_shared = (uint64_t) pml;
		pdp_shared = (uint64_t) pdp;
		pd_shared = (uint64_t) pd;
	
		tlb_flush((uint64_t)pml - VIRT_BASE);
	}
	return;
}
*/

void
set_end_addr(
			 uint64_t pt[]
	   		)
{
	if (!flag)
	{
		end_base_addr = end_addr;
		flag = 1;
	}

	uint64_t *pdp, *pd;
	uint64_t *pml = global_pml4;
	int i = 0;

	uint64_t _virt_base = init_virt_base + end_base_addr;

	pdp = global_pdp;
	pd = global_pd;

	uint64_t pml_off = get_pml4_offset(_virt_base);
	uint64_t pdp_off = get_pdp_offset(_virt_base);
	uint64_t pd_off  = get_pd_offset(_virt_base);

	while (i < TABLE_SIZE)
	{
		pt[get_pt_offset(_virt_base)] = end_base_addr;
		pt[get_pt_offset(_virt_base)] |= KERN_PERM_BITS;
		end_base_addr = end_base_addr + PAGE_SIZE;
		_virt_base += PAGE_SIZE;
		i++;
	}

	*(pd + pd_off) = (uint64_t)pt;
	*(pd + pd_off) |= KERN_PERM_BITS;

	*(pdp + pdp_off) = (uint64_t)pd;
	*(pdp + pdp_off) |= KERN_PERM_BITS;
	
	*(pml + pml_off) = (uint64_t)pdp;
	*(pml + pml_off) |= KERN_PERM_BITS;

	pml4_shared = (uint64_t) pml;
	pdp_shared = (uint64_t) pdp;
	pd_shared = (uint64_t) pd;
	pt_shared = (uint64_t) __get_page();

	return;
}

void
set_initial_addr(			 
				 uint64_t *pt
				)
{
	volatile uint64_t base_addr = BASE_ADDR;
	uint64_t *pdp, *pd;
	uint64_t *_pt;
	uint64_t *pml = global_pml4;

	uint64_t _virt_base = init_virt_base;

	uint64_t pml_off = get_pml4_offset(_virt_base);
	uint64_t pdp_off = get_pdp_offset(_virt_base);
	uint64_t pd_off  = get_pd_offset(_virt_base);

	pdp = global_pdp;
	pd = global_pd;

	_pt = pt;
	while (base_addr <= ((uint64_t)mem_data.physbase - PAGE_SIZE))
	{
		*(pt + get_pt_offset(_virt_base)) = base_addr;
		*(pt + get_pt_offset(_virt_base)) |= KERN_PERM_BITS;
		base_addr = base_addr + PAGE_SIZE;
		_virt_base += PAGE_SIZE;
	}

	*(pd + pd_off) = (uint64_t)_pt;
	*(pd + pd_off) |= KERN_PERM_BITS;

	*(pdp + pdp_off) = (uint64_t)pd;
	*(pdp + pdp_off) |= KERN_PERM_BITS;
		
	*(pml + pml_off) = (uint64_t)pdp;
	*(pml + pml_off) |= KERN_PERM_BITS;
}

void*
set_kpage_tables(
				 void *pml4
	   			)
{
	volatile uint64_t base_addr = (uint64_t)mem_data.physbase;
	uint64_t *pdp, *pd, *pt, *pt_init_addr, *pt_max[NUM_PT];
	uint64_t *pml = pml4;

	uint64_t _virt_base = virt_base;

	uint64_t pml_off = get_pml4_offset(_virt_base);
	uint64_t pdp_off = get_pdp_offset(_virt_base);
	uint64_t pd_off  = get_pd_offset(_virt_base);

	int i = 0;
	int count = 0;

	int count_end = 0;

	pdp = __get_page();
	pd = __get_page();
	pt = __get_page();
	pt_init_addr = __get_page();
	
	for (; count < NUM_PT; count++)
	{
		pt_max[count] = __get_page();
	}

	global_pml4 = pml;
	global_pdp = pdp;
	global_pd = pd;

	kpt = pt;

	set_initial_addr(pt_init_addr);

	end_addr = (uint64_t)mem_data.physbase + (TABLE_SIZE * PAGE_SIZE);

	for (; count_end < NUM_PT; count_end++)
	{
		set_end_addr(pt_max[count_end]);
	}

	while (i < TABLE_SIZE)
	{
		*(pt + get_pt_offset(_virt_base)) = base_addr;
		*(pt + get_pt_offset(_virt_base)) |= KERN_PERM_BITS;
		base_addr = base_addr + PAGE_SIZE;
		_virt_base += PAGE_SIZE;
		i++;
	}

	*(pd + pd_off) = (uint64_t)kpt;
	*(pd + pd_off) |= KERN_PERM_BITS;

	*(pdp + pdp_off) = (uint64_t)pd;
	*(pdp + pdp_off) |= KERN_PERM_BITS;
		
	*(pml + pml_off) = (uint64_t)pdp;
	*(pml + pml_off) |= KERN_PERM_BITS;
	return pml4;
}

void*
set_kernel_pages(
				 void *pml4_cr3
				)
{
	void *pml4;
	void *cr3;

	pml4 = __get_page();
	virt_base = virt_base + (uint64_t)mem_data.physbase;
	cr3 = set_kpage_tables(pml4);
	return cr3;
}

void
set_pml4(
		 void *pml4_cr3
		)
{
	void *base_addr = NULL;
	base_addr = set_kernel_pages(pml4_cr3);

	load_cr3((uint64_t)base_addr);

	return;
}

void
set_pages(
		  void *page_addr
		 )
{
	int i;
	int num_pages = NUMBER_OF_PAGES;
	struct page_desc *head;
	head = page_addr;
	pdesc = page_addr;
	for (i = 0; i < NUMBER_OF_PAGES; i++)
	{
		if(i < (num_pages - 1))
		{
			pdesc->next = pdesc + PDESC_PG_INC;
		}
		else
		{
			pdesc->next = NULL;
			break;
		}
		pdesc->pd_free = FREE;
		pdesc = pdesc->next;
	}
	pdesc = head;
}

void
set_free_list(
			 )
{
	int i;
	freelist = mem_data.physfree;

	unsigned *address = freelist;

	for (i = 0; i < NUMBER_OF_PAGES; i++)
	{
		*freelist = 1;
		freelist += 0x1;
	}

	set_pages(freelist);

	set_pml4(freelist);

	freelist = address;
	
	freelist = (unsigned *)(init_virt_base + (uint64_t)freelist);
	
	return;
}
