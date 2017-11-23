.globl dispatch

dispatch:
			push %rsi
//			push %r15
//			push %r14
//			push %r13
//			push %r12
//			push %r11
//			push %r10
//			push %r9
//			push %r8
//			push %rax
//    		push %rbx
//			push %rcx
//    		push %rdx
//    		push %rbp			

		  	// Switch stacks
			push %rdi		 	/* save me on my stack */
    		push %rdx
			mov %rsp, 0x8(%rdi) 	/* save my stack ptr */
			mov	0x8(%rsi), %rsp 	/* switch to next stack */
		  	pop %rdx
			mov %rdi, (%rdx)
		  	pop %rdi			/* Update me (rdi) to next task */

		  	// Load new callee-save registers
//    		pop %rbp			
//		  	pop %rdx
//		  	pop %rcx
//    		pop %rbx
//			pop %rax
//			pop %r8
//			pop %r9
//			pop %r10
//			pop %r11
//			pop %r12
//			pop %r13
//			pop %r14
//			pop %r15
		  	pop %rsi
		  	ret
