#include <sys/defs.h>
#include <sys/gdt.h>
#include <sys/kprintf.h>
#include <sys/tarfs.h>
#include <sys/ahci.h>
#include <sys/pci.h>
#include <sys/page_alloc.h>
#include <sys/process.h>
#include <sys/idt.h>
#include <sys/init.h>
#include <sys/timer.h>
#include <sys/pic.h>
#include <sys/kb.h>

#define INITIAL_STACK_SIZE 4096
#define VIDEO_MEM 0xB8000

uint8_t initial_stack[INITIAL_STACK_SIZE]__attribute__((aligned(16)));
uint32_t* loader_stack;
extern char kernmem, physbase;

void start(uint32_t *modulep, void *physbase, void *physfree)
{
  struct smap_t {
    uint64_t base, length;
    uint32_t type;
  }__attribute__((packed)) *smap;
  while(modulep[0] != 0x9001) modulep += modulep[1]+2;
  for(smap = (struct smap_t*)(modulep+2); smap < (struct smap_t*)((char*)modulep+modulep[1]+2*4); ++smap) {
    if (smap->type == 1 /* memory */ && smap->length != 0) {
      kprintf("Available Physical Memory [%p-%p]\n", smap->base, smap->base + smap->length);
    }
  }
  mem_data.physfree = physfree;
  mem_data.physbase = physbase;
  kprintf("physbase %p\n", (uint64_t)mem_data.physbase);
  kprintf("physfree %p\n", (uint64_t)mem_data.physfree);
  kprintf("tarfs in [%p:%p]\n", &_binary_tarfs_start, &_binary_tarfs_end);
}

void boot(void)
{
  // note: function changes rsp, local stack variables can't be practically used
  register char *temp1, *temp2;

  for(temp2 = (char*)(VIDEO_MEM+1); temp2 < (char*)VIDEO_MEM+160*25; temp2 += 2) *temp2 = 7 /* white */;
  __asm__ volatile (
    "cli;"
    "movq %%rsp, %0;"
    "movq %1, %%rsp;"
    :"=g"(loader_stack)
    :"r"(&initial_stack[INITIAL_STACK_SIZE])
  );
  init_gdt();
  start(
    (uint32_t*)((char*)(uint64_t)loader_stack[3] + (uint64_t)&kernmem - (uint64_t)&physbase),
    (uint64_t*)&physbase,
    (uint64_t*)(uint64_t)loader_stack[4]
  );
  set_free_list();
  for(
    temp1 = "!!!!! start() returned !!!!!", temp2 = (char*)(VIRT_BASE + VIDEO_MEM);
    *temp1;
    temp1 += 1, temp2 += 2
  ) *temp2 = *temp1;

	__asm__ __volatile__(
			"cli\t\n"
			);

  set_up_idt();
	init_pic();
	init_timer();
  init_tarfs();
//  checkAllBuses();
	init_queues();
	init_kbd();
	create_pcb(idle);
	create_pcb(init_entry);
	idle();

  while(1) __asm__ volatile ("hlt");
}
