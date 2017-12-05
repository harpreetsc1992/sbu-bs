#ifndef _PAGE_ALLOC_H
#define _PAGE_ALLOC_H

#include <sys/defs.h>

#define PAGE_SIZE 4096
#define NUMBER_OF_PAGES 24576
#define NUM_PT 47
#define VIRT_BASE 0xFFFFFFFF80000000
#define KERN_PERM_BITS 0x3
#define USR_PERM_BITS 0x7

uint64_t pml4_shared;
uint64_t pdp_shared;
uint64_t pd_shared;
uint64_t pt_shared;

uint64_t *kpt;

typedef enum
{
    READ_WRITE,
    READ_ONLY,
    READONLY_COW
} pg_perm_t;

typedef enum
{
	IN_USE,
	FREE
} pd_status_t;

struct page_desc
{
	pd_status_t pd_free:1;
	struct page_desc *next;
};

typedef struct m_data
{	
	void* kernmem;
	void* physfree;
	void* physbase;
	void* base;
	void* range;
} m_data_t;

m_data_t mem_data;
/*
void
remove_from_free_list(
                      unsigned long num_pages
                     );

void
set_page_used(
              unsigned long index
             );

void
alloc_pages(
            unsigned long num_pages
           );

void
dealloc_pages(
              unsigned long page_addresses[]
             );
*/

void
load_cr3(
         uint64_t lcr3
        );

void
tlb_flush(
          uint64_t pt
         );

uint64_t
get_user_virt_addr(
                  );

uint64_t
get_user_phys_addr(
                  );

uint64_t
get_pt_offset(
              uint64_t _virt_base
             );

uint64_t
get_pd_offset(
              uint64_t _virt_base
             );

uint64_t
get_pdp_offset(
               uint64_t _virt_base
              );

uint64_t
get_pml4_offset(
                uint64_t _virt_base
               );

void*
page_alloc(
          );

void*
get_page(
        );

void*
__get_page(
          );

void
set_user_pages(
			  );

void
set_page_unused(
             unsigned long index,
                unsigned cow
               );

void
add_to_free_list(
                 uint64_t page_address
                );

void
set_end_addr(
             uint64_t pt[]
            );

void
set_initial_addr(
                 uint64_t *pt
                );

void*
set_kpage_tables(
                 void *pml4
                );

void*
set_kernel_pages(
	             void *pml4_cr3
    	        );

void
set_pml4(
		  void *pml4_cr3
		 );

void
set_pages(
		  void *page_addr
         );

void 
set_free_list(
			 );

void*
set_kernel_pages(
				 void *pml4_cr3
				);

static inline void
set_usr_pt(
		   uint64_t *pt,
		   uint64_t *pd,
		   uint64_t *pdp,
		   uint64_t *pml4,
		   uint64_t *page_addr,
		   uint64_t *pt_entry, 
		   uint64_t *pd_entry, 
		   uint64_t *pdp_entry, 
		   uint64_t *pml4_entry
		  )
{
	*pt_entry = (uint64_t)page_addr | 0x7;
	*pd_entry = (uint64_t)pt | 0x7;
	*pdp_entry = (uint64_t)pd | 0x7;
	*pml4_entry = (uint64_t)pdp | 0x7;
	
	return;
}

static inline void
traverse_usr_pts(
				 uint64_t *page_addr
		  	 	)
{
	uint64_t *pd_entry, *pdp_entry, *pt_entry, *pml4_entry;
	uint64_t *pd, *pdp, *pt, *pml4;
	pml4 = (uint64_t *)((uint64_t) page_addr & 0xffff000000000000); 
	pml4_entry = (uint64_t *)((((uint64_t)pml4 >> 39) | get_pml4_offset((uint64_t)page_addr)) << 39);
	pdp = pml4_entry;
	pdp_entry = (uint64_t *)((((uint64_t)pdp >> 30) | get_pdp_offset((uint64_t)page_addr)) << 30);
	pd = pdp_entry;
	pd_entry = (uint64_t *)((((uint64_t)pd >> 21) | get_pd_offset((uint64_t)page_addr)) << 21);
	pt = pd_entry;
	pt_entry = (uint64_t *)((((uint64_t)pt >> 12) | get_pt_offset((uint64_t)page_addr)) << 12);

	set_usr_pt(pt, pd, pdp, pml4, page_addr,
			   pt_entry, pd_entry, pdp_entry, pml4_entry);

	return;
}

#endif
