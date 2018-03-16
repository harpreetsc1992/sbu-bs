.macro pushal
	push %rax
	push %rbx
	push %rcx
	push %rdx
	push %rbp			

	push %r15
	push %r14
	push %r13
	push %r12
	push %r11
	push %r10
	push %r9
	push %r8
.endm

.macro popal
	pop %r8
	pop %r9
	pop %r10
	pop %r11
	pop %r12
	pop %r13
	pop %r14
	pop %r15

	pop %rbp			
  	pop %rdx
  	pop %rcx
	pop %rbx
	pop %rax
.endm

.globl dispatch
.globl usr_dispatch

dispatch:
	pushal
	push %rsi
	push %rdi		 	/* save me on my stack */

  	// Switch stacks
	mov %rsp, 0x10(%rdi) 	/* save my stack ptr */
	mov	0x8(%rsi), %rsp 	/* switch to next stack */

  	pop %rdi			/* Update me (rdi) to next task */
  	pop %rsi

  	// Load new callee-save registers
//	popal
  	ret
.end

