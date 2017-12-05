#ifndef _SYSCALL_H
#define _SYSCALL_H

#include <sys/defs.h>

#define SYSCALL(n) static __inline uint64_t __syscall#n

static __inline uint64_t 
__syscall0(
		   uint64_t n
		  )
{
	uint64_t val;
    __asm__ __volatile__(
		"movq %1,%%rax\t\n"
        "int $0x80\t\n"
        "movq %%rax,%0\t\n"
        :"=r"(val)
        :"r"(n)
        :"rbp", "rcx", "rbx", "rdx", "rsi", "rdi", "r12", "r11", "r10", "r9", "r8", "r13", "r14", "r15"//Clobberred registers
        );  
	return val;
}

static __inline uint64_t 
__syscall1(
		   uint64_t n1,
		   uint64_t n2
		  )
{
	uint64_t val;
	__asm volatile(
		"movq %1,%%rax\t\n"
        "movq %2,%%rbx\t\n"
		"int $0x80\t\n"
		"movq %%rax,%0\t\n"
		:"=r"(val)
		:"r"(n1),"r"(n2)
		:"rbp", "rcx", "rdx", "rsi", "rdi", "r12", "r11", "r10", "r9", "r8", "r13", "r14", "r15"//Clobberred registers
		);
	return val;

}

static __inline uint64_t 
__syscall2(
		   uint64_t n1,
		   uint64_t n2,
		   uint64_t n3
		  )
{
	uint64_t val;
    __asm__ __volatile__(
        "movq %1, %%rdi\t\n" 
        "movq %2, %%rsi\t\n" 
        "movq %3, %%rax\t\n"
        "int $0x80;"
		"movq %%rax, %0\t\n"
        :"=a" (val)
        :"g"(n2), "g"(n3), "a"(n1)
        :"rdx", "rcx", "rbp", "rbx", "r12", "r11", "r10", "r9", "r8", "r13", "r14", "r15"
    );
	return val;
}

static __inline uint64_t
__syscall3(
           uint64_t n1,
           uint64_t n2,
           uint64_t n3,
		   uint64_t n4
          )
{
	uint64_t val;
    __asm__ __volatile__(
        "movq %1, %%rdi\t\n" 
        "movq %2, %%rsi\t\n"
		"movq %3, %%rdx\t\n" 
        "movq %4, %%rax\t\n"
        "int $0x80;"
		"movq %%rax, %0\t\n"
        :"=a" (val)
        :"g"(n2), "g"(n3), "g"(n4), "a"(n1)
        :"rcx", "rbp", "rbx", "r12", "r11", "r10", "r9", "r8", "r13", "r14", "r15"
    );
	return val;
}

#endif
