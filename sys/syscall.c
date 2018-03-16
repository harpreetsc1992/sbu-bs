#include <sys/syscall.h>
#include <sys/isr.h>
#include <sys/process.h>
#include <sys/tarfs.h>
#include <sys/timer.h>
#include <sys/kprintf.h>

void 
isr_handler128(
			   struct stack_frame *sf
			  )
{
	uint64_t syscall_no = 0;
	uint64_t buf, arg1, arg2;
	uint64_t size;
	uint64_t *ptr;
	uint64_t val;
	//Save register values
	__asm__ __volatile__(
			"movq %%rax, %0\t\n"
			"movq %%rbx, %1\t\n"
			"movq %%rcx, %2\t\n"
			"movq %%rdx, %3\t\n"
			: "=g"(syscall_no), "=g"(buf), "=g"(arg1), "=g"(arg2)
			:: "rax", "rsi", "rcx"
			);  

	if (syscall_no == 2) // printf
	{
		kprintf((char *) buf);
	}
	else if (syscall_no == 3) // kmalloc
	{
		__asm__ __volatile__(
				"movq %%rbx, %0\t\n"
				:"=b"(size)
				:
				);

		ptr = (uint64_t *)page_alloc();

		__asm__ __volatile__(
				"movq %0, %%rax\t\n" // load memory address from malloc into return address
				:: "r" (ptr)
				);
	}
	else if (syscall_no == 4) // fork
	{
		val = fork_process();
		if (tick % 120 != 0)
		{
			__asm__ __volatile__(
				"movq $0, %%rax\t\n" // load fork child id as return value
				:: "r" (val)
				);
		}
		else 
		{
		__asm__ __volatile__(
				"movq %0, %%rax\t\n" // load fork child id as return value
				:: "r" (val)
				);
		}
	}
	else if (syscall_no == 5)  // exit
	{
		exit_process(buf);
	}
	else if (syscall_no == 6) // sleep
	{
		val = sleep_proc(buf);
		__asm__ __volatile__(
				"movq %0, %%rax\t\n"
				:: "b" (val));
		__asm__("hlt");
	}
	else if (syscall_no == 7) //malloc
	{
		void *val = malloc(buf);
		__asm__ __volatile__(
				"movq %0, %%rax\t\n" //return value is the address returned by malloc
				:: "b" (val)); 
	}
	else if (syscall_no == 8) //getpid
	{
		val = get_pid();
		__asm__ __volatile__(
				"movq %0, %%rax\t\n" //return the pid
				:: "b" (val));
	}
	else if (syscall_no == 9) //getppid
	{
		val = get_ppid();
		__asm__ __volatile__(
				"movq %0, %%rax\t\n" //return the parent pid
				:: "b" (val));
	}
	else if (syscall_no == 10) //execvpe
	{
		val = exec((char *) buf, (char **) arg1, (char **) arg2);
		__asm__ __volatile__(
				"movq %0, %%rax\t\n" //return the next process pid 
				:: "b" (val)
				);
	}
	else if (syscall_no == 11) // scanf
	{
		__asm__ __volatile__(
				"sti\t\n"
				);
		char* buf;
		__asm__ __volatile__(
				"movq %%rbx, %0\t\n"
				: "=b" (buf)
				:
				);
//		scanf(buf);
	}
	else if (syscall_no == 12) // opendir
	{
		uint64_t hdr = opendir((char *)buf);
		__asm__ __volatile__(
				"movq %0, %%rax\t\n"
				:: "a" ((uint64_t) hdr)
				: "cc", "memory"
				);
	}
	else if (syscall_no == 13) // read dir
	{
		uint64_t val = readdir(buf);
		__asm__ __volatile__(
				"movq %0, %%rax\t\n" //return the directory addr hdr 
				:: "b" (val)
				: "cc", "memory"
				);
	}
	else if (syscall_no == 14) // open file
	{
		FILE *fd;
		fd = (FILE *)openfile((char *)buf);
		__asm__ __volatile__(
		        "movq %0, %%rax\t\n"
		        :: "a" ((uint64_t) fd)
		        : "cc", "memory"
		        ); 
	}
	else if (syscall_no == 15) // read file
	{
		uint64_t bytes_read = readfile((FILE *) buf, (int) arg1, arg2);
		__asm__ __volatile__(
				"movq %0, %%rax\t\n"
				:: "a" ((uint64_t)bytes_read)
				: "cc", "memory"
				); 
	}
	else if (syscall_no == 16) //waitpid
	{
		val = syswaitpid(buf, arg1);
		__asm__ __volatile__(
				"movq %0, %%rax\t\n"
				:: "b"(val)
				);
	}
	else if (syscall_no == 17) //ps
	{
		dp();
	}
	else if (syscall_no == 18) //sleepshell
	{
		__asm__ __volatile__(
				"sti\t\n"
				);
		sleep_shell(buf);
	}
	else if(syscall_no == 19) //shutdown
	{
		kshutdown();
	}
	else if(syscall_no == 20) //putc
	{
		val = putc(buf);
		__asm__ __volatile__(
				"movq %0, %%rax\t\n"
				:: "b"(val)
				);
	}
	else if (syscall_no == 21) //kill
	{
		kill_process(buf);
//		__asm__ __volatile__(
//			"movq %0, %%rax\t\n"
//			:: "b"(val)
//			);
	}
	else if (syscall_no == 22) //clear
	{
		clear_screen();
	}
}
