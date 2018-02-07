#ifndef _SYSCALL_H
#define _SYSCALL_H

#include <sys/defs.h>
#include <stdio.h>
#include <sys/memory.h>
#include <sys/page_alloc.h>

#define SYSCALL(n) static __inline uint64_t __syscall#n
#define T_SYSCALL 0x80

static inline uint64_t 
__syscall0(
		   uint64_t n
		  )
{
	uint64_t val;
    __asm__ __volatile__(
		"movq %1, %%rax\t\n"
        "int %2\t\n"
        "movq %%rax, %0\t\n"
        :"=r"(val)
        :"r"(n), "i"(T_SYSCALL)
        :"rbp", "rcx", "rbx", "rdx", "rsi", "rdi", "r12", "r11", "r10", "r9", "r8", "r13", "r14", "r15"
        );  
	return val;
}

static inline uint64_t 
__syscall1(
		   uint64_t n1,
		   uint64_t n2
		  )
{
	uint64_t val;
	__asm volatile(
		"movq %1, %%rax\t\n"
        "movq %2, %%rbx\t\n"
		"int %3\t\n"
		"movq %%rax, %0\t\n"
		: "=r"(val)
		: "r"(n1), "r"(n2), "i"(T_SYSCALL)
		:"rbp", "rcx", "rdx", "rsi", "rdi", "r12", "r11", "r10", "r9", "r8", "r13", "r14", "r15"
		);
	return val;

}

static inline uint64_t 
__syscall2(
		   uint64_t n1,
		   uint64_t n2,
		   uint64_t n3
		  )
{
	uint64_t val;
    __asm__ __volatile__(
		"int %1\t\n"
		:"=a"(val)		
		:"i"(T_SYSCALL), "0"(n1), "b"((uint64_t)(n2)), "c"((uint64_t)(n3)) 
		:"cc","memory"
//        "movq %1, %%rdi\t\n" 
//        "movq %2, %%rsi\t\n" 
//        "movq %3, %%rax\t\n"
//        "int $0x80;"
//		"movq %%rax, %0\t\n"
//        :"=a" (val)
//        :"g"(n2), "g"(n3), "a"(n1)
//        :"rdx", "rcx", "rbp", "rbx", "r12", "r11", "r10", "r9", "r8", "r13", "r14", "r15"
    );
	return val;
}

static inline uint64_t
__syscall3(
           uint64_t n1,
           uint64_t n2,
           uint64_t n3,
		   uint64_t n4
          )
{
	uint64_t val;
    __asm__ __volatile__(
		"int %1\t\n"
		:"=a"(val)
		:"i"(T_SYSCALL), "0"(n1), "b"((uint64_t)(n2)), "c"((uint64_t)(n3)), "d"((uint64_t)(n4))
		:"cc","memory"
//        "movq %1, %%rdi\t\n" 
//        "movq %2, %%rsi\t\n"
//		"movq %3, %%rdx\t\n" 
//        "movq %4, %%rax\t\n"
//        "int $0x80;"
//		"movq %%rax, %0\t\n"
//        :"=a" (val)
//        :"g"(n2), "g"(n3), "g"(n4), "a"(n1)
//        :"rcx", "rbp", "rbx", "r12", "r11", "r10", "r9", "r8", "r13", "r14", "r15"
    );
	return val;
}

static inline uint64_t 
__syscall4(
		   uint64_t n1, 
		   uint64_t n2, 
		   uint64_t n3, 
		   uint64_t n4, 
		   uint64_t n5
		  ) 
{
	uint64_t val;
	__asm__ volatile (
		"int %1\t\n"
		:"=a"(val)
		:"i"(T_SYSCALL), "0"(n1), "b"((uint64_t)(n2)), "c"((uint64_t)(n3)), "d"((uint64_t)(n4)),"D"((uint64_t)(n5))
		:"cc","memory"
	);
	return val;
}
#endif
