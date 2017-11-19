#ifndef _PROCESS_H
#define _PROCESS_H

#include <sys/defs.h>
#include <sys/page_alloc.h>
#include <sys/file.h>
#include <sys/kprintf.h>

#define STACK_SIZE 256 

int p_id, ready_procs, proc_count_in_list;

struct PCB *curr_pcb;

struct PCB *processes;

typedef enum 
{ 
	UNUSED, 
	SLEEPING, 
	READY, 
	RUNNING, 
	DEAD,	
	ZOMBIE 
} proc_state_t;

struct context
{
	uint64_t rip, rsp;
};

struct PCB 
{
	uint64_t kstack[STACK_SIZE];
	struct context c_t;
//	uint64_t rsp;
	uint64_t pid;
	proc_state_t proc_state;
	int exit_status;
	struct PCB *next;
};

void
dispatch(
		);

struct PCB*
init_pcb(
         void (*t)()
        );

int
create_pcb(
          );

void
thread1(
	   );

static inline void
move_to_end(
			struct PCB *pcb
		   )
{
	struct PCB *head = processes;
	while(processes->next != head)
	{
		processes = processes->next;
	}
	
	processes->next = pcb;

	processes = processes->next;
	processes->next = head->next;

	head = NULL;	
}

static inline void
add_to_ready_list(
                  struct PCB *pcb
                 )
{
	proc_count_in_list++;
	if (1 == proc_count_in_list)
	{
    	processes = pcb;
    	processes->next = processes;
	}
	else
	{
		
	}
}

int 
fn_call(
	   );

#if 0
struct context 
{
  	uint32_t edi;
  	uint32_t esi;
  	uint32_t ebx;
  	uint32_t ebp;
  	uint32_t eip;
};

struct trapframe 
{
  	// registers as pushed by pusha
  	uint32_t edi;
  	uint32_t esi;
  	uint32_t ebp;
  	uint32_t oesp;      // useless & ignored
  	uint32_t ebx;
  	uint32_t edx;
  	uint32_t ecx;
  	uint32_t eax;

  	// rest of trap frame
  	uint16_t gs;
  	uint16_t padding1;
  	uint16_t fs;
  	uint16_t padding2;
  	uint16_t es;
  	uint16_t padding3;
  	uint16_t ds;
  	uint16_t padding4;
  	uint32_t trapno;

  	// below here defined by x86 hardware
  	uint32_t err;
  	uint32_t eip;
  	uint16_t cs;
  	uint16_t padding5;
  	uint32_t eflags;

  	// below here only when crossing rings, such as from user to kernel
  	uint32_t esp;
  	uint16_t ss;
  	uint16_t padding6;
};

// Per-process state
struct proc 
{
	uint64_t sz;                 // Size of process memory (bytes)
	uint64_t *p_tbl;             // Page table
  	char *kstack;                // Bottom of kernel stack for this process
  	enum procstate state;        // Process state
  	volatile int pid;            // Process ID
  	struct proc *parent;         // Parent process
  	struct trapframe *tf;        // Trap frame for current syscall
  	struct context *context;     // swtch() here to run process
  	void *chan;                  // If non-zero, sleeping on chan
  	int killed;                  // If non-zero, have been killed
  	struct file *ofile[NOFILE];  // Open files
  	struct inode *cwd;           // Current directory
  	char name[16];               // Process name (debugging)
};
#endif
#endif
