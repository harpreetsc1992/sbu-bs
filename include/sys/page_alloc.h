#ifndef _PAGE_ALLOC_H
#define _PAGE_ALLOC_H

#include <sys/defs.h>
#include <sys/kern_ops.h>

#define PAGE_SIZE 0x1000
#define NUMBER_OF_PAGES 24576
#define NUM_PT 47
#define VIRT_BASE 0xFFFFFFFF80000000
#define KERN_PERM_BITS 0x2
#define USR_PERM_BITS 0x7
#define COW_PERM_BITS 0x5
#define START_PADDR_FOR_USRS 0x5000000
#define START_VADDR_FOR_USRS 0x0000000080000000
#define TABLE_SIZE 512

extern uint64_t pml4_shared;
extern uint64_t pdp_shared;
extern uint64_t pd_shared;
extern uint64_t pt_shared;

uint64_t *kpt;
extern uint64_t usr_pml;
extern uint64_t usr_pdp;
extern uint64_t usr_pd;
extern uint64_t usr_pt;
extern uint64_t *global_pml4;
extern uint64_t knl_pt1;
extern uint64_t knl_pt2;
extern uint16_t pt_count;
extern uint16_t pd_count;
extern uint16_t pdp_count;
extern uint8_t dif_ctxt;
extern int pt_flag, pd_flag, pdp_flag;

typedef enum
{
    READ_ONLY,
    READ_WRITE,
    READONLY_COW
} pg_perm_t;

typedef enum
{
	FREE,
	IN_USE
} pd_status_t;

struct page_desc
{
	pd_status_t pd_free:1;
	struct page_desc *next;
};

struct user_page
{
	pg_perm_t pd_rw;
	pd_status_t pd_free:1;
	struct user_page *next;
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
				   int mapped
                  );

uint64_t
get_user_phys_addr(
				   int mapped
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

void *
kmalloc(
		uint64_t sz
	   );

void*
get_page(
        );

void*
__get_page(
          );

uint64_t*
set_user_pages(
			  );

uint64_t *
get_usr_page(
             uint32_t sz
            );

void
allocate_page(
			  uint64_t *va,
			  uint64_t *pa,
			  uint32_t sz
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

uint64_t *
get_usr_page_for_child(
                       uint32_t sz
                      );

uint64_t *
get_new_page(
            );

void
add_user_page(
              struct user_page *upg,
              uint8_t perm
             );
#endif
