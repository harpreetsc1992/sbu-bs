.globl jmp_usrmode
.extern tst_usr_fn

jmp_usrmode:
     mov $0x23, %ax
     mov %ax, %ds
     mov %ax, %es 
     mov %ax, %fs 
     mov %ax, %gs 
 
     mov %rsp, %rax
     push $0x23 
     push %rax 
     pushf
     push $0x1B
     push tst_usr_fn  
     iret
.end
