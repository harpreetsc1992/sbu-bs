#include <sys/idt.h>
#include <sys/kb.h>
#include <sys/isr.h>
#include <sys/io_ports.h>
#include <sys/fault.h>
#include <sys/process.h>
#include <sys/syscall.h>

extern void isr128();

void 
isr_handler(
			regs_t reg
		   )
{
	if (reg.int_no == 32 && reg.int_no == 33)
	{
		irq_handler(reg);
		return;
	}
	else
	{
		if (reg.int_no == 10)
		{
			kprintf("TSS Exception \n");
		}
		else if (reg.int_no == 0)
		{
			kprintf("Divide by Zero\n");
			kshutdown();
		}
		else if (reg.int_no == 14)
		{
			page_fault_handler(reg);
		}
		return;
	}
}

void
irq_handler(
			regs_t reg
		   )
{
	switch (reg.int_no) 
	{
		case 32:
            // Called directly from irq0()
		break;
		case 33:
			keyboard_handler(reg);
		break;
		default:
		break;
	}
    // Send EOI signal to Master PIC
	outb(0x20, 0x20);
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
    ivt_set_gate(0,   (uint64_t)isr0,  0x08, 0x8E);
    ivt_set_gate(10, (uint64_t)isr10, 0x08, 0x8E);
//    ivt_set_gate(13,  (uint64_t)isr13, 0x08, 0x8E);
    ivt_set_gate(14,  (uint64_t)isr14, 0x08, 0x8E);
    ivt_set_gate(128, (uint64_t)isr128, 0x08, 0xEE);

    // IRQs
    ivt_set_gate(32, (uint64_t)irq0, 0x08, 0x8E);
	ivt_set_gate(33, (uint64_t)irq1, 0x08, 0x8E);

	load_idtr((uint64_t)&ivt_ptr);
	enable_interrupt_flag();
}
