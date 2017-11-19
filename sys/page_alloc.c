#include <sys/page_alloc.h>

//pg_hdr_t *pages = mem_data.physfree + 0x4000;

//page_desc_t *page_desc = physfree;

//free_list_t free_list;

/*
void
set_page_desc(
			 )
{
//	page_desc[i].pd_free = FREE;
	uint64_t *pml4 = physfree;
	uint64_t *pdp = physfree + 0x1000;
	uint64_t *pd = pdp + 0x1000;
	uint64_t *pt = pd + 0x1000;
	return;
}
*/

#if 0
void
set_page_unused(
			    unsigned long index,
				unsigned cow
			   )
{
	/*
	 * TODO: Check for READ_COW
	 */
	if (cow)
	{
		pages[index].pg_status = USED | ALLOCATED;

		pages[index].pg_perm = READONLY_COW;				
		pages[index].present:1;
	}
	else
	{
		pages[index].pg_status = UNUSED | UNALLOCATED;

		pages[index].pg_perm = READ_ONLY;				
		pages[index].present:0;
	}
}

void
add_to_free_list(
				 uint64_t page_address
				)
{
	int i;
	for (i = 0; i < free_list.count; i++)
	{
		if (page_address < free_list.page_addr[i])
		{
			continue;
		}
		else
		{
			free_list.count++;
			for (j = free_list.count - 1; j > i; j--)
			{
				free_list.page[j + 1] = free_list.page[j];
				free_list.page_addr[j + 1] = free_list.page_addr[j];
			}
			set_page_unused(free_list.page[i], 0);
			free_list.page_addr[i] = page_address;
			return;
		}
	}
}

void
remove_from_free_list(
				 	  unsigned long num_pages	
					 )
{
	int i;

	for (i = 0; i < free_list.count - num_pages; i++)
	{
		free_list.page[i] = free_list.page[i + num_pages];
		free_list.page_addr[i] = free_list.page_addr[i + num_pages];
	}
	free_list.count -= num_pages;
}

void
set_page_used(
			  unsigned long index
			 )
{
	pages[index].pg_status = USED | ALLOCATED;
	pages[index].pg_perm = READ_WRITE;
	pages[index].present:1;
}

void
alloc_pages(
			unsigned long num_pages
		   )
{
	unsigned long j;
	unsigned long i;
	unsigned int count = 0;
	for (i = 0; i < num_pages; i++)
	{
		for (j = 0; (j < NUMBER_OF_PAGES) && (count < num_pages); j++)
		{
			if (free_list.page_addr[i] == &page[j])
			{
				set_page_used(j);
				count++;
			}
		}
	}

	remove_from_free_list(num_pages);
}

void
dealloc_pages(
			  unsigned long page_addresses[]
			 )
{
	unsigned long i;
	
	for (i = 0; page_addresses[i] != 0; i++)
	{
		add_to_free_list(page_addresses[i]);
	}
}
int 
set_pages(
	   	  uint64_t base,
		  uint64_t range,
		  uint64_t kernel_base,
		  uint64_t kernel_range,
		  int counter
		 )
{
	unsigned long i = counter;
	uint64_t address = base;
	int kern_switch = 0;

	while (address <= (range - 0x1000))
	{
		if (i == 0)
		{
			pages[i].addr = base;
			pages[i].prev_addr = base;
		}
		if (i && !kern_switch)	pages[i].addr = address;
		//holes in physical memory - track the addresses in the previous and the next range of addresses
		if (address >= kernel_base - 0x1000)	kern_switch = 1;
	
		if ((kernel_base <= address) && ((kernel_range - 0x1000) >= address)) 
		{
			if ((kernel_range != range) || (kernel_range <= (range - 0x1000)))
			{
				pages[i].next_addr = kernel_range;
			}
			pages[i].prev_addr = pages[i-1].addr;
			address += 0x1000;
			kern_switch = 1;
			continue;
		}
		
		else if (address >= (kernel_base - 0x1000) && kern_switch)
		{
			pages[i].next_addr = kernel_range;
			address = kernel_range;
			i++;
		}
		
		else
		{
			if (address <= (range - 0x2000)) 	pages[i].next_addr = address + 0x1000;

			if (i) 								pages[i].prev_addr = pages[i - 1].addr;

			if (counter && (i == counter))		pages[i - 1].next_addr = pages[i].addr;

			pages[i].pg_perm = READ_ONLY;
			pages[i].present = 0;
			pages[i].pg_status = UNUSED | UNALLOCATED;
/*			
			free_list.page[i] = pages[i];
			free_list.count++;
			free_list.page_addr[i] = (uint64_t)&pages[i];
*/
			address += 0x1000;
			i++;
		}
		kern_switch = 0;
	}

	return i;
}
#endif
/*
void
map_page(
         void * physaddr,
         void * virtualaddr,
         unsigned int flags
        )
{
    unsigned long pdindex = (unsigned long) virtualaddr >> 22;
    unsigned long ptindex = (unsigned long) virtualaddr >> 12 & 0x03FF;

    unsigned long *pd = (unsigned long *) 0xFFFFF000;

    unsigned long *pt = ((unsigned long *) 0xFFC00000) + (0x400 * pdindex);

    pt[ptindex] = ((unsigned long) physaddr) | (flags & 0xFFF) | 0x01;
}

void
*get_physaddr(
              void *virtualaddr
             )
{
    unsigned long pdindex = (unsigned long) virtualaddr >> 22;
    unsigned long ptindex = (unsigned long) virtualaddr >> 12 & 0x03FF;

    unsigned long *pd = (unsigned long *) 0xFFFFF000;

    unsigned long *pt = ((unsigned long *) 0xFFC00000) + (0x400 * pdindex);

    return (void *)((pt[ptindex] & ~0xFFF) + ((unsigned long) virtualaddr & 0xFFF));
}

set_map(
        void *v_addr
       )
{
    void *physaddr = get_physaddr(v_addr);
    map_page(physaddr, v_addr, flags);

}

void
set_pg_dir(
           uint64_t pd_base
          )
{
    __asm__ volatile (
    "mov eax, pd_base;"
    "mov cr3, eax;"
    "mov eax, cr0;"
    "or eax, 0x80010001;"
    "mov cr0, eax;"
    );
}
*/
