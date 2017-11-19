#ifndef _PAGE_ALLOC_H
#define _PAGE_ALLOC_H

#include <sys/defs.h>

#define PAGE_SIZE 4096
#define NUMBER_OF_PAGES 24576
#define NUM_PT 40

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
             uint64_t *pt[]
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

#endif
