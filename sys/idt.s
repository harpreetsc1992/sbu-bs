.macro PUSHA
    push %rdi
    push %rax
    push %rbx
    push %rcx
    push %rdx
    push %rbp
    push %rsi
    push %r8
    push %r9
    push %r10
    push %r11
    push %r12
    push %r13
    push %r14
    push %r15
.endm

.macro POPA
    pop %r15
    pop %r14
    pop %r13
    pop %r12
    pop %r11
    pop %r10
    pop %r9
    pop %r8
    pop %rsi
    pop %rbp
    pop %rdx
    pop %rcx
    pop %rbx
    pop %rax
    pop %rdi
.endm

.extern isr_handler
.extern irq_handler
.extern enable_interrupt_flag
.extern page_fault_handler
.extern timer_callback
.extern isr_handler128
.extern jmp_usrmode

# Load a new IDT
# parameter 1: address of idtr (%rdi)
.globl load_idtr
.globl isr0
.globl isr10
.globl isr14
.globl isr128
.globl irq0
.globl irq1
.globl isr_coord
.globl enable_interrupt_flag

load_idtr:
    lidt (%rdi)
    ret

enable_interrupt_flag:
	sti
	ret

isr0:
	cli
    pushq $0
    pushq $0
    jmp isr_coord

isr10:
    cli
    push $0
    push $10
    jmp isr_coord

isr14:
	PUSHA
	cli
    pushq $14
    jmp isr_coord

isr128:
	PUSHA
	call isr_handler128
	POPA
	iretq

irq0:
	cli
	PUSHA
	movq %rsp, %rdi
	call timer_callback
	POPA
	sti
	iretq

irq1:
	PUSHA
	push $0x0
	push $0x21
	jmp isr_coord

isr_coord:
    PUSHA
//	mov %rsp, %rdi
    call isr_handler
    POPA
	jmp jmp_usrmode
//    add $0x10, %rsp
//    sti
//    iretq
.end
