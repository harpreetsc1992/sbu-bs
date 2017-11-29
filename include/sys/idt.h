#ifndef _IDT_H
#define _IDT_H

#include <sys/defs.h>
#include <sys/gdt.h>
#include <sys/kprintf.h>

#define NUMBER_OF_IDT_ENTRIES 256

extern void 
load_idtr(
		  uint64_t
		 );

extern void 
enable_interrupt_flag(
					 );

// ISR and IRQ asm routines
extern void 
isr0(
	);

extern void 
isr10(
	 );

extern void 
isr13(
	 );

extern void 
isr14(
	 );

extern void 
syscall_handler(
			   );

extern void 
irq0(
	);

extern void 
irq1(
	);

struct regs
{
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8, rsi, rbp, rdx, rcx, rbx, rax, rdi; // Registers pushed by PUSHA
    uint64_t int_no, err_no;                            // Interrupt number and error code
    uint64_t rip, cs, rflags, rsp, ss;                  // Registers pushed by the IRETQ 
} __attribute__((packed));

typedef struct regs regs_t;

struct int_vec_t
{
	uint16_t offset_1; // offset bits 0..15
   	uint16_t selector; // a code segment selector in GDT or LDT
   	uint8_t ist;       // bits 0..2 holds Interrupt Stack Table offset, rest of bits zero.
   	uint8_t type_attr; // type and attributes
   	uint16_t offset_2; // offset bits 16..31
   	uint32_t offset_3; // offset bits 32..63
   	uint32_t zero;     // reserved
}__attribute__((packed));

struct int_ptr
{
    uint16_t size;             // Size of IDT table 
    uint64_t base;              // Base address of IDT table 
} __attribute__((packed));

struct int_vec_t ivt[NUMBER_OF_IDT_ENTRIES];
struct int_ptr ivt_ptr;

void
isr_handler(
            regs_t reg
           );

void 
ivt_set_gate(
             int32_t i,
             uint64_t offset,
             uint16_t selector,
             uint8_t flags
            );

void
set_up_idt(
		  );
#endif
