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
.extern enable_interrupt_flag

# Load a new IDT
# parameter 1: address of idtr (%rdi)
.globl load_idtr
.globl isr10
.globl isr_coord
.globl enable_interrupt_flag

load_idtr:
    lidt (%rdi)
    ret

enable_interrupt_flag:
	sti
	ret

isr10:
    cli
    push $0
    push $10
    jmp isr_coord

isr_coord:
    PUSHA
    mov %rsp, %rdi
    call isr_handler
    POPA
    add $0x10, %rsp
    sti
    iretq
