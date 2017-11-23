#include <sys/idt.h>

void
isr_handler(
			regs_t reg
		   )
{
	kprintf("TSS Exception\n");
	return;
}

void 
ivt_set_gate(
			 int32_t i, 
			 uint64_t offset, 
			 uint16_t selector, 
			 uint8_t flags
			)
{
    ivt[i].offset_1 = (offset & 0xFFFFUL);
    ivt[i].offset_2 = (offset >> 16) & 0xFFFFUL;
    ivt[i].offset_3 = (offset >> 32) & 0xFFFFFFFFUL;

    ivt[i].selector = selector;
    ivt[i].type_attr = flags;
    
    ivt[i].ist = 0x0;
    ivt[i].zero = 0x0;
}

void 
set_up_idt(
		  )
{
	ivt_ptr.size = sizeof(struct int_vec_t) * NUMBER_OF_IDT_ENTRIES;
	ivt_ptr.base = (uint64_t)&ivt;

	// ISRs
//    ivt_set_gate(0,   (uint64_t)isr0,  0x08, 0x8E);
    ivt_set_gate(10, (uint64_t)isr10, 0x08, 0x8E);
//    ivt_set_gate(13,  (uint64_t)isr13, 0x08, 0x8E);
//    ivt_set_gate(14,  (uint64_t)isr14, 0x08, 0x8E);
//    ivt_set_gate(128, (uint64_t)syscall_handler, 0x08, 0xEE);

    // IRQs
//    ivt_set_gate(32, (uint64_t)irq0, 0x08, 0x8E);
//	ivt_set_gate(33, (uint64_t)irq1, 0x08, 0x8E);

	load_idtr((uint64_t)&ivt_ptr);
	enable_interrupt_flag();
}
